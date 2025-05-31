#include "core/baseItem/Video/VideoPlayer.h"

#include "core/log.h"
#include "core/baseItem/Bitmap.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}
#include <SDL2/SDL.h>

using namespace core;

VideoPlayer::VideoPlayer() : playing(false), loop(false), shouldExit(false) {
    if(!SDL_WasInit(SDL_INIT_AUDIO)){
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        Log << Level::Info << "SDL音频子系统初始化成功" << op::endl;
    }
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
    audioStreamIndex = -1;
    videoStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    audioStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (videoStreamIndex == -1) {
        Log << Level::Error << "未找到视频流" << op::endl;
        cleanup();
        return false;
    }
    if( audioStreamIndex == -1) {
        Log << Level::Warn << "未找到音频流" << op::endl;
    }

    fps=av_q2d(formatContext->streams[videoStreamIndex]->avg_frame_rate);
    frameDelay = 1.0 / fps;

    // 获取视频编解码器
    const AVCodec* codec = avcodec_find_decoder(formatContext->streams[videoStreamIndex]->codecpar->codec_id);
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
    if(audioStreamIndex != -1) {
        const AVCodec* audioCodec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);
        if (!audioCodec) {
            Log << Level::Warn << "无法找到音频编解码器" << op::endl;
        }
        audioCodecContext = avcodec_alloc_context3(nullptr);
        if (!audioCodecContext) {
            Log << Level::Warn << "无法分配音频解码器上下文" << op::endl;
        }
        if(avcodec_parameters_to_context(audioCodecContext, formatContext->streams[audioStreamIndex]->codecpar)){
            Log << Level::Warn << "无法复制音频编解码器参数" << op::endl;
        }
        // 打开音频解码器
        if( avcodec_open2(audioCodecContext, audioCodec, nullptr) < 0) {
            Log << Level::Warn << "无法打开音频解码器" << op::endl;
            cleanup();
            return false;
        }
    }


    width = codecContext->width;
    height = codecContext->height;

    // 分配音频帧
    frame = av_frame_alloc();
    // 分配图像帧
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
    }    if(audioStreamIndex>=0){
        SDL_AudioSpec audioSpec, obtainedSpec;
        audioSpec.freq = audioCodecContext->sample_rate;
        audioSpec.format = AUDIO_S16SYS; // 使用16位有符号整数格式
        audioSpec.channels = audioCodecContext->ch_layout.nb_channels;
        audioSpec.samples = 4096; // 减小音频缓冲区大小以降低延迟
        audioSpec.callback = nullptr; // 使用SDL_QueueAudio()而不是回调
        audioSpec.userdata = nullptr; // 不需要用户数据
        
        audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &audioSpec, &obtainedSpec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
        if (audioDeviceID == 0) {
            Log << Level::Warn << "无法打开SDL音频设备: " << SDL_GetError() << op::endl;
        } else {
            Log << Level::Info << "音频设备打开成功，设备ID: " << audioDeviceID 
                << ", 采样率: " << obtainedSpec.freq << "Hz, 声道: " << (int)obtainedSpec.channels << op::endl;
        }
        
        // 设置音频重采样参数 - 使用兼容的API
        swrContext = swr_alloc();
        if (!swrContext) {
            Log << Level::Warn << "无法分配音频重采样上下文" << op::endl;
        } else {
            // 设置输入输出格式
            av_opt_set_chlayout(swrContext, "in_chlayout", &audioCodecContext->ch_layout, 0);
            av_opt_set_chlayout(swrContext, "out_chlayout", &audioCodecContext->ch_layout, 0);
            av_opt_set_int(swrContext, "in_sample_rate", audioCodecContext->sample_rate, 0);
            av_opt_set_int(swrContext, "out_sample_rate", obtainedSpec.freq, 0); // 使用实际获得的采样率
            av_opt_set_sample_fmt(swrContext, "in_sample_fmt", audioCodecContext->sample_fmt, 0);
            av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
            
            if (swr_init(swrContext) < 0) {
                Log << Level::Warn << "无法初始化音频重采样上下文" << op::endl;
                swr_free(&swrContext);
                swrContext = nullptr;
            } else {
                Log << Level::Info << "音频重采样初始化成功 - 输入采样率: " << audioCodecContext->sample_rate 
                    << "Hz, 输出采样率: " << obtainedSpec.freq << "Hz, 声道: " << audioCodecContext->ch_layout.nb_channels << op::endl;
            }
        }
    }
    Log << Level::Info << "视频文件加载成功: " << videoPath
        << " (" << width << "x" << height << " @ " << fps << "fps)" << op::endl;

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
    if(audioStreamIndex>=0 && audioDeviceID > 0){
        // 确保音频设备已打开并开始播放
        SDL_PauseAudioDevice(audioDeviceID, 0); // 启动音频设备
        Log << Level::Info << "音频设备已启动，ID: " << audioDeviceID << op::endl;
    } else {
        Log << Level::Warn << "没有音频流或音频设备未正确初始化" << op::endl;
    }
    // 确保没有正在运行的解码线程    
    if (decoderThread.joinable()) {
        decoderThread.join();
    }  
    if (decoderThreadAudio.joinable()) {
        decoderThreadAudio.join();
    }
    if (decoderThreadVideo.joinable()) {
        decoderThreadVideo.join();
    }

    decoderThread = std::thread(&VideoPlayer::decodeThread, this);
    decoderThreadVideo=std::thread(&VideoPlayer::decodeThreadVideo,this);
    if(audioStreamIndex>=0) decoderThreadAudio = std::thread(&VideoPlayer::decodeThreadAudio, this);
    Log << Level::Info << "视频开始播放 (软件解码)" << op::endl;
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
    if (decoderThreadVideo.joinable()) {
        Log << Level::Info << "等待解码线程结束..." << op::endl;
        decoderThreadVideo.join();
        Log << Level::Info << "解码线程已结束" << op::endl;
    }
    if (decoderThreadAudio.joinable()) {
        Log << Level::Info << "等待解码线程结束..." << op::endl;
        decoderThreadAudio.join();
        Log << Level::Info << "解码线程已结束" << op::endl;
    }    // 停止音频播放
    if (audioStreamIndex >= 0 && audioDeviceID > 0) {
        SDL_PauseAudioDevice(audioDeviceID, 1); // 暂停音频播放
        SDL_ClearQueuedAudio(audioDeviceID); // 清空音频队列
        Log << Level::Info << "音频播放已暂停" << op::endl;
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
    
    // frameData 会在函数结束时自动释放
    return bitmap;
}

void VideoPlayer::pause() {
    if (!playing) return;
    playing = false;
    
    // 暂停音频播放
    if (audioStreamIndex >= 0 && audioDeviceID > 0) {
        SDL_PauseAudioDevice(audioDeviceID, 1); // 暂停音频设备
        Log << Level::Info << "音频播放已暂停" << op::endl;
    }
    
    Log << Level::Info << "视频播放已暂停" << op::endl;
}

void VideoPlayer::resume() {
    if (playing || !formatContext || videoStreamIndex == -1) return;
    playing = true;
    
    // 恢复音频播放
    if (audioStreamIndex >= 0 && audioDeviceID > 0) {
        SDL_PauseAudioDevice(audioDeviceID, 0); // 恢复音频设备
        Log << Level::Info << "音频播放已恢复" << op::endl;
    }
    
    Log << Level::Info << "视频播放已恢复" << op::endl;
}

void VideoPlayer::setVolume(int volume) {
    // 限制音量范围在0-100之间
    volume = std::max(0, std::min(100, volume));
    
    // 将音量从0-100范围转换为0.0-1.0范围
    this->volume = volume / 100.0f;
    
    Log << Level::Info << "设置音量: " << volume << "% (内部值: " << this->volume.load() << ")" << op::endl;
}

void VideoPlayer::applyVolume(uint8_t* audioBuffer, int bufferSize, int channels) {
    if (!audioBuffer || bufferSize <= 0) {
        return;
    }
    
    float currentVolume = volume.load();
    if (currentVolume >= 1.0f) {
        // 音量为最大时，无需处理
        return;
    }
    
    // 音频数据格式为 AUDIO_S16SYS (16位有符号整数)
    int16_t* samples = reinterpret_cast<int16_t*>(audioBuffer);
    int sampleCount = bufferSize / sizeof(int16_t);
    
    for (int i = 0; i < sampleCount; i++) {
        // 应用音量并防止溢出
        float sample = static_cast<float>(samples[i]) * currentVolume;
        
        // 限制在16位有符号整数范围内
        if (sample > 32767.0f) {
            samples[i] = 32767;
        } else if (sample < -32768.0f) {
            samples[i] = -32768;
        } else {
            samples[i] = static_cast<int16_t>(sample);
        }
    }
}

void VideoPlayer::decodeThread(){
    int ret=0;
    bool frameFinished=false;

    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);

    while(!shouldExit){
        if(!playing){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }        if(packetVideoQueue.size()>20||packetAudioQueue.size()>20){
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 避免过多包积压
            continue;
        }
        AVPacket* packet = av_packet_alloc();
        if (!packet) {
            Log << Level::Error << "无法分配AVPacket" << op::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 添加延迟，避免无限循环消耗CPU
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
            packetVideoQueue.push(packet);
        }
        else if (packet->stream_index == audioStreamIndex) {
            packetAudioQueue.push(packet);
        }
        else {
            av_packet_unref(packet); // 释放不需要的包
            continue;
        }
        
    }
    playing=false;
}

void VideoPlayer::decodeThreadVideo() {
    char buf[AV_ERROR_MAX_STRING_SIZE]{0};
    while (!shouldExit) {
        auto now = std::chrono::steady_clock::now();
        if (!playing || packetVideoQueue.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        AVPacket* packet = packetVideoQueue.front();
        packetVideoQueue.pop();
        if(!packet) continue;
        int ret = avcodec_send_packet(codecContext, packet);
        if (ret < 0) {
            Log << Level::Error << "发送视频包失败: " << av_make_error_string(buf, sizeof(buf), ret) << op::endl;
            av_packet_unref(packet);
            continue;
        }

        ret = avcodec_receive_frame(codecContext, frame);
        if (ret < 0) {
            Log << Level::Error << "接收视频帧失败: " << av_make_error_string(buf, sizeof(buf), ret) << op::endl;
            av_packet_unref(packet);
            continue;
        }

        // 转换为RGB格式
        sws_scale(swsContext, frame->data, frame->linesize, 0, height, rgbFrame->data, rgbFrame->linesize);

        // 将RGB帧转换为FrameData
        std::shared_ptr<FrameData> frameData = convertFrameToFrameData(rgbFrame);
        
        if (frameData) {
            std::lock_guard<std::mutex> lock(frameMutex);
            currentFrameData = frameData;
            frameReady = true;
        }
        
        av_packet_unref(packet); // 释放包
        auto now_ = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now_ - now;
        int sleeptime = 1000 / fps - elapsed.count() * 1000; // 计算帧间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime));
    }
}

void VideoPlayer::decodeThreadAudio() {
    bool audioEOF = false;
    AVFrame* audioFrame = av_frame_alloc();
    if (!audioFrame) {
        Log << Level::Error << "无法分配音频帧" << op::endl;
        return;
    }
    
    while (!shouldExit) {
        if (!playing) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        // 如果队列为空且未到EOF，等待更多数据
        if (packetAudioQueue.empty() && !audioEOF) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        AVPacket* packet = nullptr;
        if (!packetAudioQueue.empty()) {
            packet = packetAudioQueue.front();
            packetAudioQueue.pop();
        }
        
        char buf[AV_ERROR_MAX_STRING_SIZE]{0};
        int ret = 0;
        
        // 发送数据包到解码器
        if (audioEOF || !packet) {
            ret = avcodec_send_packet(audioCodecContext, nullptr); // 刷新解码器
        } else {
            ret = avcodec_send_packet(audioCodecContext, packet);
        }
        
        if (ret < 0) {
            if (ret == AVERROR(EAGAIN)) {
                // 解码器需要更多输出空间，继续尝试接收帧
            } else if (ret == AVERROR_EOF) {
                Log << Level::Info << "音频解码器到达EOF" << op::endl;
                audioEOF = true;
            } else {
                Log << Level::Warn << "发送音频包失败: " << av_make_error_string(buf, sizeof(buf), ret) << op::endl;
            }
            if (packet) av_packet_unref(packet);
        } else {
            if (packet) av_packet_unref(packet);
        }
        
        // 尝试接收解码后的帧
        while (true) {
            ret = avcodec_receive_frame(audioCodecContext, audioFrame);
            if (ret < 0) {
                if (ret == AVERROR(EAGAIN)) {
                    // 需要更多输入数据
                    break;
                } else if (ret == AVERROR_EOF) {
                    Log << Level::Info << "音频解码完成，退出音频线程" << op::endl;
                    av_frame_free(&audioFrame);
                    return;
                } else {
                    Log << Level::Warn << "接收音频帧失败: " << av_make_error_string(buf, sizeof(buf), ret) << op::endl;
                    break;
                }
            }

            // 处理有效的音频帧
            if (!swrContext) {
                Log << Level::Warn << "音频重采样上下文无效，跳过音频帧" << op::endl;
                continue;
            }

            // 计算输出样本数
            int64_t outSamples = swr_get_out_samples(swrContext, audioFrame->nb_samples);
            if (outSamples <= 0) {
                Log << Level::Warn << "无法计算输出样本数: " << (int)outSamples << op::endl;
                continue;
            }
            
            // 分配音频缓冲区
            int bufferSize = av_samples_get_buffer_size(nullptr, audioCodecContext->ch_layout.nb_channels, 
                                                       (int)outSamples, AV_SAMPLE_FMT_S16, 1);
            if (bufferSize <= 0) {
                Log << Level::Warn << "无法计算音频缓冲区大小: " << bufferSize << op::endl;
                continue;
            }
            
            uint8_t* audioBuffer = (uint8_t*)av_malloc(bufferSize);
            if (!audioBuffer) {
                Log << Level::Error << "无法分配音频缓冲区，大小: " << bufferSize << " 字节" << op::endl;
                continue;
            }
            
            // 执行音频重采样
            int convertedSamples = swr_convert(swrContext, &audioBuffer, (int)outSamples, 
                                             (const uint8_t**)audioFrame->data, audioFrame->nb_samples);
            if (convertedSamples < 0) {
                Log << Level::Error << "音频重采样失败: " << av_make_error_string(buf, sizeof(buf), convertedSamples) << op::endl;
                av_free(audioBuffer);
                continue;
            }
            
            // 重新计算实际使用的缓冲区大小
            int actualBufferSize = av_samples_get_buffer_size(nullptr, audioCodecContext->ch_layout.nb_channels, 
                                                             convertedSamples, AV_SAMPLE_FMT_S16, 1);
            
            // 控制音频队列大小，避免过度缓冲
            if (actualBufferSize > 0 && audioDeviceID > 0) {
                Uint32 queuedSize = SDL_GetQueuedAudioSize(audioDeviceID);
                // 如果队列过满，等待一段时间再尝试
                while (queuedSize > 16384 && playing && !shouldExit) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    queuedSize = SDL_GetQueuedAudioSize(audioDeviceID);
                }
                  if (playing && !shouldExit) {
                    // 应用音量控制
                    applyVolume(audioBuffer, actualBufferSize, audioCodecContext->ch_layout.nb_channels);
                    
                    if (SDL_QueueAudio(audioDeviceID, audioBuffer, actualBufferSize) < 0) {
                        Log << Level::Warn << "SDL_QueueAudio失败: " << SDL_GetError() << op::endl;
                    }
                }
            }
            
            av_free(audioBuffer);
        }
        
        // 避免CPU占用过高
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    av_frame_free(&audioFrame);
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
    // 清理其他资源
    if (swsContext) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
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
    }    // 清理音频相关资源
    if (audioCodecContext) {
        avcodec_free_context(&audioCodecContext);
        audioCodecContext = nullptr;
    }
    if (swrContext) {
        swr_free(&swrContext);
        swrContext = nullptr;
    }
    if (audioDeviceID > 0) {
        SDL_CloseAudioDevice(audioDeviceID);
        audioDeviceID = 0;
        Log << Level::Info << "音频设备已关闭" << op::endl;
    }
    // 清理队列
    while (!packetVideoQueue.empty()) {
        AVPacket* pkt = packetVideoQueue.front();
        packetVideoQueue.pop();
        av_packet_free(&pkt);
    }
    while (!packetAudioQueue.empty()) {
        AVPacket* pkt = packetAudioQueue.front();
        packetAudioQueue.pop();
        av_packet_free(&pkt);
    }
    // 重置状态
    playing = false;
    shouldExit = true;
    loop = false;
    frameReady = false;
    audioStreamIndex = -1;
    videoStream = nullptr;
    videoStreamIndex = -1;
    width = 0;
    height = 0;
    frameDelay = 0.0;
    
    // 重置帧缓冲区
    std::lock_guard<std::mutex> lock(frameMutex);
    currentFrameData = nullptr;
    frameReady = false;
}