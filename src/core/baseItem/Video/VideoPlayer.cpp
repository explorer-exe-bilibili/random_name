#include "core/baseItem/Video/VideoPlayer.h"
#include "core/baseItem/Video/HWAccelMonitor.h"

#include "core/log.h"
#include "core/baseItem/Bitmap.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
}

using namespace core;

VideoPlayer::VideoPlayer() : playing(false), loop(false), shouldExit(false) {
    Log << Level::Info << "视频播放器创建成功" << op::endl;
}

VideoPlayer::~VideoPlayer() {
    stop();
    Log << Level::Info << "视频播放器销毁成功" << op::endl;
}

bool VideoPlayer::load(const std::string& path) {
    stop();
    videoPath = path;
    Log << Level::Info << "加载视频文件: " << videoPath << op::endl;
    formatContext = avformat_alloc_context();
    if (!formatContext) {
        Log << Level::Error << "无法分配格式上下文" << op::endl;
        return false;
    }
    if (avformat_open_input(&formatContext, videoPath.c_str(), nullptr, nullptr) < 0) {
        Log << Level::Error << "无法打开视频文件: " << videoPath << op::endl;
        return false;
    }

    // 获取视频流信息
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        Log << Level::Error << "无法获取视频流信息" << op::endl;
        return false;
    }

    // 查找视频流
    videoStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            videoStream = formatContext->streams[i];
            break;
        }
    }

    if (videoStreamIndex == -1) {
        Log << Level::Error << "未找到视频流" << op::endl;
        cleanup();
        return false;
    }

    fps=av_q2d(videoStream->avg_frame_rate);
    frameDelay = 1.0 / fps;

    // 获取视频编解码器
    const AVCodec* codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (!codec) {
        Log << Level::Error << "无法找到视频编解码器" << op::endl;
        cleanup();
        return false;
    }

    // 获取视频流的解码器上下文
    codecContext = avcodec_alloc_context3(nullptr);
    if (!codecContext) {
        Log << Level::Error << "无法分配解码器上下文" << op::endl;
        cleanup();
        return false;
    }

    if(avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar)){
        Log << Level::Error << "无法复制编解码器参数" << op::endl;
        cleanup();
        return false;
    }

    // 打开解码器
    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        Log << Level::Error << "无法打开解码器" << op::endl;
        cleanup();
        return false;
    }

    width = codecContext->width;
    height = codecContext->height;

    // 分配图像帧
    frame = av_frame_alloc();
    rgbFrame = av_frame_alloc();
    if (!frame || !rgbFrame) {
        Log << Level::Error << "无法分配图像帧" << op::endl;
        cleanup();
        return false;
    }

    // 为RGB帧分配缓冲区
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes);
    if (!buffer) {
        Log << Level::Error << "无法分配RGB帧缓冲区" << op::endl;
        cleanup();
        return false;
    }
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24, width, height, 1);

    //初始化SWS上下文
    swsContext = sws_getContext(width, height,
         codecContext->pix_fmt, width, height, 
         AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, 
         nullptr, nullptr);
    if (!swsContext) {
        Log << Level::Error << "无法初始化SWS上下文" << op::endl;
        cleanup();
        return false;
    }

    //分配数据包
    packet = av_packet_alloc();
    if (!packet) {
        Log << Level::Error << "无法分配数据包" << op::endl;
        cleanup();
        return false;
    }    Log << Level::Info << "视频文件加载成功: " << videoPath 
    << " (" << width << "x" << height << " @ " << fps << "fps)" << op::endl;
    
    // 尝试初始化硬件加速
    if (initHardwareAcceleration()) {
        Log << Level::Info << "成功启用硬件加速解码" << op::endl;
    } else {
        Log << Level::Info << "使用软件解码" << op::endl;
    }
    
    return true;
}

void VideoPlayer::play() {
    if (playing) return;

    playing = true;
    if(!formatContext || videoStreamIndex == -1){
        Log << Level::Error << "视频文件未加载或无效" << op::endl;
        playing = false;
        return;
    }    shouldExit = false;
    
    // 确保没有正在运行的解码线程    
    if (decoderThread.joinable()) {
        decoderThread.join();
    }
    
    // 根据是否启用硬件加速选择解码线程函数
    if (useHardwareAccel) {
        decoderThread = std::thread(&VideoPlayer::decodeThreadHW, this);
        Log << Level::Info << "视频开始播放 (硬件加速)" << op::endl;
    } else {
        decoderThread = std::thread(&VideoPlayer::decodeThread, this);
        Log << Level::Info << "视频开始播放 (软件解码)" << op::endl;
    }
}

void VideoPlayer::stop() {
    if (!playing && !decoderThread.joinable()) return;

    // 设置退出标志
    shouldExit = true;
    playing = false;

    // 等待解码线程结束
    if (decoderThread.joinable()) {
        Log << Level::Info << "等待解码线程结束..." << op::endl;
        decoderThread.join();
        Log << Level::Info << "解码线程已结束" << op::endl;
    }

    // 清理资源
    cleanup();
}

void VideoPlayer::setLoop(bool enable) {
    loop = enable;
}

std::shared_ptr<Bitmap> VideoPlayer::getCurrentFrame() {
    // 添加额外的检查以防止在视频已停止时访问互斥锁
    if (!playing && shouldExit) {
        return nullptr;
    }
    
    if (!frameReady) {
        return nullptr;
    }
    
    std::shared_ptr<FrameData> frameData;
    {
        std::lock_guard<std::mutex> lock(frameMutex);
        if (!currentFrameData) {
            return nullptr;
        }
        
        // 创建一个新的FrameData对象并复制数据
        frameData = std::make_shared<FrameData>(currentFrameData->width, currentFrameData->height);
        memcpy(frameData->data, currentFrameData->data, currentFrameData->width * currentFrameData->height * 3);
    }
    
    // 在主线程中创建Bitmap对象，不会有跨线程问题
    if (!frameData || !frameData->data) {
        return nullptr;
    }    
    
    // 创建位图（此时在主线程中，可以安全创建纹理），使用RGB格式避免数据转换
    std::shared_ptr<Bitmap> bitmap = std::make_shared<Bitmap>(frameData->width, frameData->height, true, true);
    
    // 直接从原始数据创建纹理，使用RGB格式避免RGB到RGBA的转换，大幅减少内存使用
    bitmap->CreateFromRGBData(frameData->data, frameData->width, frameData->height, true, true);
    
    // 更新性能监控数据
    if (HWAccelMonitor::getInstance().isHardwareAccelEnabled()) {
        // 如果启用了硬件加速但仍使用软件解码路径，可能是硬件渲染失败，记录下来
        HWAccelMonitor::getInstance().logHWError("硬件加速已启用但使用了软件解码路径");
    }
    HWAccelMonitor::getInstance().updateHWFrameCount();
    
    // frameData 会在函数结束时自动释放
    return bitmap;
}

void VideoPlayer::pause() {
    if (!playing) return;
    playing = false;
    Log << Level::Info << "视频播放已暂停" << op::endl;
}

void VideoPlayer::resume() {
    if (playing || !formatContext || videoStreamIndex == -1) return;
    playing = true;
    Log << Level::Info << "视频播放已恢复" << op::endl;
}

void VideoPlayer::setVolume(int volume) {
    // 注意：此处需要访问音频流和设置音量。
    // 由于当前实现可能没有处理音频流，这里先记录音量值但不应用
    Log << Level::Info << "设置音量: " << volume << "% (注意：当前实现不包含音频处理)" << op::endl;
}

void VideoPlayer::update() {
    // 此方法可以在主线程中调用，用于状态更新
    // 对于简单实现，不需要额外操作，因为解码线程已经在处理帧
}

void VideoPlayer::decodeThread(){
    int ret=0;
    bool frameFinished=false;

    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);

    while(!shouldExit){
        if(!playing){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        // 读取视频帧
        ret = av_read_frame(formatContext, packet);
        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                Log << Level::Info << "视频播放完毕" << op::endl;
                if (loop) {
                    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
                    continue;
                }
                else{
                    break;
                }
            }
            Log << Level::Error << "读取视频帧失败" << op::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 添加延迟，避免无限循环消耗CPU
            continue;
        }

        if (packet->stream_index == videoStreamIndex) {
            // 解码视频帧
            ret = avcodec_send_packet(codecContext, packet);
            if (ret < 0) {
                Log << Level::Error << "发送视频帧失败" << op::endl;
                av_packet_unref(packet);
                continue;
            }

            ret = avcodec_receive_frame(codecContext, frame);
            if (ret < 0) {
                Log << Level::Error << "接收视频帧失败" << op::endl;
                av_frame_unref(frame);
                av_packet_unref(packet);
                continue;
            }

            frameFinished = (ret == 0);
                if (frameFinished) {                //将帧转换为RGB格式
                sws_scale(swsContext, frame->data, frame->linesize,
                     0, height, rgbFrame->data, rgbFrame->linesize);
                
                // 将帧转换为FrameData，不涉及OpenGL操作
                std::shared_ptr<FrameData> frameData = convertFrameToFrameData(rgbFrame);
                
                // 如果成功获取到帧数据，更新当前帧
                if (frameData) {
                    std::lock_guard<std::mutex> lock(frameMutex);
                    currentFrameData = frameData;
                    frameReady = true;
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(int(frameDelay*1000)));
            }
        }

        av_packet_unref(packet);
    }
    playing=false;
}

std::shared_ptr<FrameData> VideoPlayer::convertFrameToFrameData(AVFrame* frame) {
    if (!frame || !frame->data[0]) {
        Log << Level::Error << "空视频帧，无法转换为帧数据" << op::endl;
        return nullptr;
    }

    // 获取RGB数据
    unsigned char* rgbData = frame->data[0];
    int rgbLinesize = frame->linesize[0];
    
    // 创建帧数据结构
    std::shared_ptr<FrameData> frameData = std::make_shared<FrameData>(width, height);
    
    // 如果帧行大小等于宽度*3（每像素3字节RGB），可以直接使用数据
    if (rgbLinesize == width * 3) {
        memcpy(frameData->data, rgbData, width * height * 3);
    } else {
        // 处理行大小不等于宽度*3的情况（通常因为内存对齐）
        for (int y = 0; y < height; y++) {
            // 复制每一行，忽略填充字节
            memcpy(frameData->data + y * width * 3, rgbData + y * rgbLinesize, width * 3);
        }
    }
    
    return frameData;
}

// 清理资源并重置状态
void VideoPlayer::cleanup() {
    // 先清理硬件加速资源
    if (hwFrame) {
        av_frame_free(&hwFrame);
        hwFrame = nullptr;
    }
    
    if (hwDeviceCtx) {
        av_buffer_unref(&hwDeviceCtx);
        hwDeviceCtx = nullptr;
    }
    
    // 清理其他资源
    if (swsContext) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }
    
    if (packet) {
        av_packet_free(&packet);
        packet = nullptr;
    }
    
    if (rgbFrame) {
        av_freep(&rgbFrame->data[0]);
        av_frame_free(&rgbFrame);
        rgbFrame = nullptr;
    }
    
    if (frame) {
        av_frame_free(&frame);
        frame = nullptr;
    }
    
    if (codecContext) {
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
    }
    
    if (formatContext) {
        avformat_close_input(&formatContext);
        formatContext = nullptr;
    }
    
    videoStreamIndex = -1;
    width = 0;
    height = 0;
    frameDelay = 0.0;
    useHardwareAccel = false;
    hwType = AV_HWDEVICE_TYPE_NONE;
    
    // 重置帧缓冲区
    std::lock_guard<std::mutex> lock(frameMutex);
    currentFrameData = nullptr;
    frameReady = false;
}

bool VideoPlayer::initHardwareAcceleration() {
    // 尝试可用的硬件加速类型
    AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
    while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
        // 按优先级尝试不同的硬件加速类型
        Log << Level::Info << "尝试硬件加速类型: " << (char*)(av_hwdevice_get_type_name(type)) << op::endl;
            if(type!=AV_HWDEVICE_TYPE_NONE){
                const AVCodec* hwCodec = avcodec_find_decoder_by_name(
                    (std::string(avcodec_get_name(videoStream->codecpar->codec_id)) + "_" + 
                    av_hwdevice_get_type_name(type)).c_str());
                
                // 如果没有找到特定硬件编解码器，尝试使用通用硬件加速
                if (!hwCodec) {
                    hwCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
                }

                if (!hwCodec) {
                    Log << Level::Info << "找不到支持 " << (char*)(av_hwdevice_get_type_name(type))
                        << " 的解码器" << op::endl;
                    continue;
                }
                
                // 检查是否支持硬件加速
                if (hwCodec->capabilities & AV_CODEC_CAP_HARDWARE) {
                    Log << Level::Info << "尝试初始化 " << (char*)(av_hwdevice_get_type_name(type)) 
                        << " 硬件加速" << op::endl;
                    
                // 创建硬件加速上下文
                int ret = av_hwdevice_ctx_create(&hwDeviceCtx, type, nullptr, nullptr, 0);
                if (ret < 0) {
                    char errBuf[AV_ERROR_MAX_STRING_SIZE];
                    av_strerror(ret, errBuf, AV_ERROR_MAX_STRING_SIZE);
                    Log << Level::Warn << "硬件加速创建失败: " << errBuf << op::endl;
                    continue;
                }
                
                // 清理之前的解码器上下文并创建新的
                avcodec_free_context(&codecContext);
                codecContext = avcodec_alloc_context3(hwCodec);
                if (!codecContext) {
                    Log << Level::Error << "分配硬件解码器上下文失败" << op::endl;
                    av_buffer_unref(&hwDeviceCtx);
                    hwDeviceCtx = nullptr;
                    continue;
                }
                
                // 设置解码器参数
                if (avcodec_parameters_to_context(codecContext, videoStream->codecpar) < 0) {
                    Log << Level::Error << "复制解码器参数失败" << op::endl;
                    av_buffer_unref(&hwDeviceCtx);
                    hwDeviceCtx = nullptr;
                    continue;
                }
                
                // 设置硬件加速上下文
                codecContext->hw_device_ctx = av_buffer_ref(hwDeviceCtx);
                
                // 打开编解码器
                if (avcodec_open2(codecContext, hwCodec, nullptr) < 0) {
                    Log << Level::Warn << "打开硬件编解码器失败" << op::endl;
                    av_buffer_unref(&hwDeviceCtx);
                    hwDeviceCtx = nullptr;
                    continue;
                }
                
                // 分配硬件帧
                hwFrame = av_frame_alloc();
                if (!hwFrame) {
                    Log << Level::Error << "分配硬件帧失败" << op::endl;
                    av_buffer_unref(&hwDeviceCtx);
                    hwDeviceCtx = nullptr;
                    continue;
                }
                
                hwType = type;
                useHardwareAccel = true;
                
                Log << Level::Info << "成功初始化 " << (char*)av_hwdevice_get_type_name(type)
                    << " 硬件加速" << op::endl;
                
                // 通知硬件加速监控
                HWAccelMonitor::getInstance().setHardwareAccelEnabled(true);
                HWAccelMonitor::getInstance().setHWAccelType(av_hwdevice_get_type_name(type));
                
                return true;
            }
        }
    }
    
    // 如果所有硬件加速都失败，回退到软件解码
    Log << Level::Info << "无法初始化硬件加速，使用软件解码" << op::endl;
    useHardwareAccel=false;
    // 确保使用软件解码器
    const AVCodec* swCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (!swCodec) {
        Log << Level::Error << "找不到软件解码器" << op::endl;
        return false;
    }
    
    // 重置解码器上下文
    avcodec_free_context(&codecContext);
    codecContext = avcodec_alloc_context3(swCodec);
    if (!codecContext) {
        Log << Level::Error << "分配软件解码器上下文失败" << op::endl;
        return false;
    }
    
    if (avcodec_parameters_to_context(codecContext, videoStream->codecpar) < 0) {
        Log << Level::Error << "复制软件解码器参数失败" << op::endl;
        return false;
    }
    
    if (avcodec_open2(codecContext, swCodec, nullptr) < 0) {
        Log << Level::Error << "打开软件解码器失败" << op::endl;
        return false;
    }
    
    return false;
}

void VideoPlayer::decodeThreadHW() {
    int ret = 0;
    bool frameFinished = false;

        // 确保帧已正确分配
    if (!frame) {
        frame = av_frame_alloc();
        if (!frame) {
            Log << Level::Error << "无法分配软件帧" << op::endl;
            return;
        }
    }

    if (!hwFrame) {
        hwFrame = av_frame_alloc();
        if (!hwFrame) {
            Log << Level::Error << "无法分配硬件帧" << op::endl;
            return;
        }
    }

    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);

    while (!shouldExit) {
        if (!playing) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        // 读取视频帧
        ret = av_read_frame(formatContext, packet);
        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                Log << Level::Info << "视频播放完毕" << op::endl;
                if (loop) {
                    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
                    continue;
                }
                else {
                    break;
                }
            }
            Log << Level::Error << "读取视频帧失败" << op::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (packet->stream_index == videoStreamIndex) {
            // 解码视频帧
            ret = avcodec_send_packet(codecContext, packet);
            if (ret < 0) {
                Log << Level::Error << "发送视频帧失败" << op::endl;
                av_packet_unref(packet);
                continue;
            }

            // 清除旧帧数据
            av_frame_unref(hwFrame);

            ret = avcodec_receive_frame(codecContext, hwFrame);
            if (ret < 0) {
                if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                    Log << Level::Error << "接收视频帧失败: "  << op::endl;
                }
                av_packet_unref(packet);
                continue;
            }

            // 将硬件帧转换为软件帧
            if (hwFrame->format == codecContext->pix_fmt) {
                // 转换格式
                ret = av_hwframe_transfer_data(frame, hwFrame, 0);
                if (ret < 0) {
                    Log << Level::Error << "硬件帧转换失败" << op::endl;
                    av_packet_unref(packet);
                    continue;
                }
                
                // 复制时间戳和其他元数据
                frame->pts = hwFrame->pts;
                frame->pkt_dts = hwFrame->pkt_dts;
                frame->best_effort_timestamp = hwFrame->best_effort_timestamp;
                
                
                // 转换为RGB格式
                if (!swsContext) {
                    // 确保有正确的 sws 上下文
                    swsContext = sws_getContext(
                        frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
                        frame->width, frame->height, AV_PIX_FMT_RGB24,
                        SWS_BILINEAR, nullptr, nullptr, nullptr
                    );
                    
                    if (!swsContext) {
                        Log << Level::Error << "无法创建 SwsContext" << op::endl;
                        av_packet_unref(packet);
                        continue;
                    }
                    
                    // 确保 RGB 帧已分配
                    if (!rgbFrame) {
                        rgbFrame = av_frame_alloc();
                        if (!rgbFrame) {
                            Log << Level::Error << "无法分配 RGB 帧" << op::endl;
                            av_packet_unref(packet);
                            continue;
                        }
                        
                        rgbFrame->format = AV_PIX_FMT_RGB24;
                        rgbFrame->width = frame->width;
                        rgbFrame->height = frame->height;
                        
                        ret = av_frame_get_buffer(rgbFrame, 32);
                        if (ret < 0) {
                            Log << Level::Error << "无法为 RGB 帧分配缓冲区" << op::endl;
                            av_packet_unref(packet);
                            continue;
                        }
                    }
                }
                
                // 转换像素格式
                sws_scale(swsContext, 
                          frame->data, frame->linesize, 0, frame->height,
                          rgbFrame->data, rgbFrame->linesize);
                
                // 计算正确的显示时间
                double pts = frame->pts * av_q2d(videoStream->time_base);
                
                // 将帧数据传递给主线程
                std::shared_ptr<FrameData> frameData = convertFrameToFrameData(rgbFrame);
                if (frameData) {
                    std::lock_guard<std::mutex> lock(frameMutex);
                    currentFrameData = frameData;
                    frameReady = true;
                }
                
                // 添加延迟以匹配帧率
                int64_t currentTime = av_gettime_relative();
                static int64_t lastFrameTime = 0;
                if (lastFrameTime > 0) {
                    int64_t sleepTime = static_cast<int64_t>(frameDelay * 1000000) - 
                                       (currentTime - lastFrameTime);
                    if (sleepTime > 0) {
                        std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
                    }
                }
                lastFrameTime = av_gettime_relative();
                
                // 更新硬件加速性能监控
                HWAccelMonitor::getInstance().updateHWFrameCount();
            }
        }
        
        av_packet_unref(packet);
    }
    
    // 设置状态为停止
    playing = false;
    
    Log << Level::Info << "硬件加速解码线程结束" << op::endl;
}