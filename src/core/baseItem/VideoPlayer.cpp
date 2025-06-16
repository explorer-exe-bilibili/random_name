#include "core/baseItem/VideoPlayer.h"

#include "core/log.h"
#include "core/baseItem/Bitmap.h"
#include "core/decrash/ErrorRecovery.h"
#include "core/decrash/VideoPlayerErrorRecovery.h"
#include "core/decrash/MemoryMonitor.h"

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

std::atomic<float> VideoPlayer::volume{1.0f};

VideoPlayer::VideoPlayer() : playing(false), loop(false), shouldExit(false) {
    // 初始化错误恢复系统
    errorRecovery = std::make_unique<VideoPlayerErrorRecovery>();
    mainThreadId = std::this_thread::get_id();
    
    if(!SDL_WasInit(SDL_INIT_AUDIO)){
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        Log << Level::Info << "SDL音频子系统初始化成功" << op::endl;
    }
    
    // 初始化音视频同步相关参数
    audioClock = 0.0;
    videoClock = 0.0;
    frameTimer = 0.0;
    videoCurrentPts = 0.0;
    videoPtsDrift = 0.0;
    lastFramePts = 0.0;
    frameLastDelay = 0.0;
    startTime = std::chrono::steady_clock::now();
    
    Log << Level::Info << "视频播放器创建成功" << op::endl;
}

VideoPlayer::~VideoPlayer() {
    stop();
    cleanup();
    Log << Level::Info << "视频播放器销毁成功" << op::endl;
}

bool VideoPlayer::load(const std::string& path) {
    // 使用错误恢复执行加载
    return ErrorRecovery::executeWithRetry<bool>([this, &path]() {
        return loadInternal(path);
    }, {.maxRetries = 3, .baseDelay = std::chrono::milliseconds(500)}, 
       ErrorRecovery::ErrorType::FFMPEG_DECODE);
}

bool VideoPlayer::loadInternal(const std::string& path) {
    isCleanedUp = false;
    static std::mutex ffmpeg_init_mutex;
    std::lock_guard<std::mutex> lock(ffmpeg_init_mutex);
    
    stop();
    cleanup();
    videoPath = path;
    Log << Level::Info << "加载视频文件: " << videoPath << op::endl;
    
    try {
        formatContext = nullptr;
        codecContext = nullptr;
        audioCodecContext = nullptr;
        formatContext = avformat_alloc_context();
        if (!formatContext) {
            Log << Level::Error << "无法分配格式上下文" << op::endl;
            return false;
        }
        if (avformat_open_input(&formatContext, videoPath.c_str(), nullptr, nullptr) < 0) {
            Log << Level::Error << "无法打开视频文件: " << videoPath << op::endl;
            return false;
        }

        int ret = avformat_find_stream_info(formatContext, nullptr);
        if (ret < 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(errbuf, sizeof(errbuf), ret);
            Log << Level::Error << "无法获取视频流信息: " << errbuf << op::endl;
            cleanup();
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
        if(audioStreamIndex >= 0) {
            // 获取音频编解码器
            const AVCodec* audioCodec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);
            if (!audioCodec) {
                Log << Level::Warn << "无法找到音频编解码器" << op::endl;
                audioStreamIndex = -1;
            }
            audioCodecContext = avcodec_alloc_context3(nullptr);
            if (!audioCodecContext) {
                Log << Level::Warn << "无法分配音频解码器上下文" << op::endl;
                audioStreamIndex = -1;
            }
            if(avcodec_parameters_to_context(audioCodecContext, formatContext->streams[audioStreamIndex]->codecpar)){
                Log << Level::Warn << "无法复制音频编解码器参数" << op::endl;
            }
            // 打开音频解码器
            if( avcodec_open2(audioCodecContext, audioCodec, nullptr) < 0) {
                Log << Level::Warn << "无法打开音频解码器" << op::endl;
                audioStreamIndex = -1;
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
        }
        if (audioStreamIndex >= 0) {
            if(audioCodecContext!=nullptr){
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
                audioStreamIndex = -1;
            } else {
                Log << Level::Info << "音频设备打开成功，设备ID: " << audioDeviceID 
                    << ", 采样率: " << obtainedSpec.freq << "Hz, 声道: " << (int)obtainedSpec.channels << op::endl;
            }
            
            // 设置音频重采样参数 - 使用兼容的API
            swrContext = swr_alloc();
            if (!swrContext) {
                Log << Level::Warn << "无法分配音频重采样上下文" << op::endl;
                audioStreamIndex = -1;
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
                    audioStreamIndex = -1;
                } else {
                    Log << Level::Info << "音频重采样初始化成功 - 输入采样率: " << audioCodecContext->sample_rate 
                        << "Hz, 输出采样率: " << obtainedSpec.freq << "Hz, 声道: " << audioCodecContext->ch_layout.nb_channels << op::endl;
                }
            }
            } // SDL音频设备打开结束
            else {
                Log << Level::Warn << "没有音频流或音频设备未正确初始化" << op::endl;
                audioStreamIndex = -1; // 如果没有音频流，则将索引设置为-1
            }
        }
        Log << Level::Info << "视频文件加载成功: " << videoPath
            << " (" << width << "x" << height << " @ " << fps << "fps)" << op::endl;

        return true;
        
    } catch (const std::exception& e) {
        Log << Level::Error << "视频加载失败: " << e.what() << op::endl;
        cleanup();
        throw;
    }
}

void VideoPlayer::play() {
    if (playing) return;

    playing = true;
    if(!formatContext || videoStreamIndex == -1){
        Log << Level::Error << "视频文件未加载或无效" << op::endl;
        playing = false;
        return;
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
    // 重置同步时钟
    startTime = std::chrono::steady_clock::now();
    audioClock = 0.0;
    videoClock = 0.0;
    frameTimer = 0.0;
    videoCurrentPts = 0.0;
    videoPtsDrift = 0.0;
    lastFramePts = 0.0;
    frameLastDelay = 0.0;
    shouldExit = false;
    isFinished = false;
    reachedEOF = false;
    videoDecodeFinished = false;
    audioDecodeFinished = false;
    if(audioStreamIndex>=0 && audioDeviceID > 0){
        // 确保音频设备已打开并开始播放
        SDL_PauseAudioDevice(audioDeviceID, 0); // 启动音频设备
        Log << Level::Info << "音频设备已启动，ID: " << audioDeviceID << op::endl;
    } else {
        Log << Level::Warn << "没有音频流或音频设备未正确初始化" << op::endl;
        audioStreamIndex = -1;
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

    // 使用超时等待线程结束，避免无限等待
    auto waitForThread = [](std::thread& t, const std::string& name) {
        if (t.joinable()) {
            Log << Level::Info << "等待" << name << "线程结束..." << op::endl;
            // 使用detach而不是join，让线程在后台自然结束
            t.detach();
            Log << Level::Info << name << "线程已分离" << op::endl;
        }
    };

    waitForThread(decoderThread, "解码");
    waitForThread(decoderThreadVideo, "视频解码");
    waitForThread(decoderThreadAudio, "音频解码");// 停止音频播放
    if (audioStreamIndex >= 0 && audioDeviceID > 0) {
        SDL_PauseAudioDevice(audioDeviceID, 1); // 暂停音频播放
        SDL_ClearQueuedAudio(audioDeviceID); // 清空音频队列
        Log << Level::Info << "音频播放已暂停" << op::endl;
    }
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
    isFinished = false;
    reachedEOF = false;
    videoDecodeFinished = false;
    audioDecodeFinished = false;
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
    VideoPlayer::volume = volume / 100.0f;

    Log << Level::Info << "设置音量: " << volume << "% (内部值: " << VideoPlayer::volume.load() << ")" << op::endl;
}

void VideoPlayer::applyVolume(uint8_t* audioBuffer, int bufferSize, int channels) {
    if (!audioBuffer || bufferSize <= 0) {
        return;
    }

    float currentVolume = VideoPlayer::volume.load();
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
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 减少睡眠时间
            continue;
        }
        if(packetVideoQueue.size()>20||packetAudioQueue.size()>200){
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
                Log << Level::Info << "已读取完所有数据包" << op::endl;
                if (loop) {
                    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
                    continue;
                }
                else{
                    reachedEOF = true; // 标记已读取完所有数据包，但不立即设置播放完成
                    break; // 直接退出，不需要等待
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
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    playing=false;
    isFinished=true;
}

void VideoPlayer::decodeThreadVideo() {
    char buf[AV_ERROR_MAX_STRING_SIZE]{0};
    while (!shouldExit) {
        auto now = std::chrono::steady_clock::now();
        if (!playing) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 减少睡眠时间
            isFinished=true; // 如果暂停或停止，直接设置为完成状态
            continue;
        }
        
        // 检查是否没有更多视频包并且已到达EOF
        if (packetVideoQueue.empty()) {
            if (reachedEOF.load()) {
                Log << Level::Info << "视频队列为空且已到达EOF，视频解码完成" << op::endl;
                videoDecodeFinished = true;
                // 检查是否可以设置播放完成
                if (audioStreamIndex < 0 || audioDecodeFinished.load()) {
                    Log << Level::Info << "所有解码完成，设置播放完成" << op::endl;
                    isFinished = true;
                }
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 减少睡眠时间
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

        // 获取视频帧的PTS并进行同步
        double videoPts = convertPtsToSeconds(frame->pts, formatContext->streams[videoStreamIndex]->time_base);
        videoPts = synchronizeVideo(frame, videoPts);
        
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
        
        // 计算同步延迟
        auto now_ = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now_ - now;
        if(audioStreamIndex>=0){
            // 基于音视频同步的帧延迟计算
            double audioTime = getAudioClock();
            double diff = videoPts - audioTime;
            
            // 同步阈值检查
            double delay = frameLastDelay.load();
            if (delay <= 0 || delay >= 1.0) {
                delay = 1.0 / fps; // 默认帧间隔
            }
            
            if (std::abs(diff) < syncThreshold) {
                // 在同步范围内，正常播放
            } else if (diff < -syncThreshold) {
                // 视频滞后，减少延迟
                delay = delay * 0.8;
            } else if (diff > syncThreshold) {
                // 视频超前，增加延迟
                delay = delay + diff;
            }
            
            frameLastDelay = delay;
            
            // 计算实际需要等待的时间
            int sleepTime = static_cast<int>((delay - elapsed.count()) * 1000);
            if (sleepTime > 0 && sleepTime < 100) { // 限制在合理范围内
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            }
        }
        else{
            // 音频不存在
            std::this_thread::sleep_for(std::chrono::milliseconds(int(1000/fps-elapsed.count()*1000)));
        }
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
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 减少睡眠时间
            continue;
        }

        // 检查是否没有更多音频包并且已到达EOF
        if (packetAudioQueue.empty()) {
            if (reachedEOF.load()) {
                Log << Level::Info << "音频队列为空且已到达EOF，音频解码完成" << op::endl;
                audioDecodeFinished = true;
                // 检查是否可以设置播放完成
                if (videoDecodeFinished.load()) {
                    Log << Level::Info << "所有解码完成，设置播放完成" << op::endl;
                    isFinished = true;
                }
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 减少睡眠时间
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
                    Log << Level::Info << "音频解码器收到EOF，音频解码完成" << op::endl;
                    audioDecodeFinished = true;
                    // 检查是否可以设置播放完成
                    if (videoDecodeFinished.load()) {
                        Log << Level::Info << "所有解码完成，设置播放完成" << op::endl;
                        isFinished = true;
                    }
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

            // 获取音频帧的PTS并转换为秒
            double audioPts = convertPtsToSeconds(audioFrame->pts, formatContext->streams[audioStreamIndex]->time_base);
            
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
                    
                    // 更新音频时钟
                    updateAudioClock(audioPts, actualBufferSize);
                    
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

    // 验证frame参数的有效性
    if (width <= 0 || height <= 0) {
        Log << Level::Error << "无效的视频尺寸: " << width << "x" << height << op::endl;
        return nullptr;
    }

    // 获取RGB数据
    unsigned char* rgbData = frame->data[0];
    int rgbLinesize = frame->linesize[0];
    
    // 验证linesize的合理性
    if (rgbLinesize < width * 3) {
        Log << Level::Error << "RGB行大小异常: " << rgbLinesize << " < " << (width * 3) << op::endl;
        return nullptr;
    }
    
    // 创建帧数据结构
    std::shared_ptr<FrameData> frameData = std::make_shared<FrameData>(width, height);
    if (!frameData || !frameData->data) {
        Log << Level::Error << "无法分配FrameData内存" << op::endl;
        return nullptr;
    }
    
    // 如果帧行大小等于宽度*3（每像素3字节RGB），可以直接使用数据
    if (rgbLinesize == width * 3) {
        size_t copySize = static_cast<size_t>(width) * height * 3;
        memcpy(frameData->data, rgbData, copySize);
    } else {
        // 处理行大小不等于宽度*3的情况（通常因为内存对齐）
        for (int y = 0; y < height; y++) {
            size_t destOffset = static_cast<size_t>(y) * width * 3;
            size_t srcOffset = static_cast<size_t>(y) * rgbLinesize;
            size_t copySize = static_cast<size_t>(width) * 3;
            
            // 边界检查
            if (destOffset + copySize > static_cast<size_t>(width) * height * 3) {
                Log << Level::Error << "目标缓冲区越界" << op::endl;
                return nullptr;
            }
            
            memcpy(frameData->data + destOffset, rgbData + srcOffset, copySize);
        }
    }
    
    return frameData;
}

// 清理资源并重置状态
void VideoPlayer::cleanup() {    
    if(isCleanedUp)return; // 如果已经清理过，直接返回
    isCleanedUp = true; // 设置清理标志，防止重复清理
    // 添加互斥锁保护，确保 cleanup 只能被一个线程执行
    static std::mutex cleanup_mutex;
    std::lock_guard<std::mutex> cleanup_lock(cleanup_mutex);
    
    // 添加标志位防止重复清理
    static bool cleaning_up = false;
    if (cleaning_up) {
        return;
    }
    cleaning_up = true;
    
    try {
        // 清理队列（先清理队列，避免后续访问已释放的上下文）
        while (!packetVideoQueue.empty()) {
            AVPacket* pkt = packetVideoQueue.front();
            packetVideoQueue.pop();
            if (pkt) {
                av_packet_free(&pkt);
            }
        }
        while (!packetAudioQueue.empty()) {
            AVPacket* pkt = packetAudioQueue.front();
            packetAudioQueue.pop();
            if (pkt) {
                av_packet_free(&pkt);
            }
        }
        
        // 清理音频设备（在清理音频上下文之前）
        if (audioDeviceID > 0) {
            SDL_PauseAudioDevice(audioDeviceID, 1);
            SDL_ClearQueuedAudio(audioDeviceID);
            SDL_CloseAudioDevice(audioDeviceID);
            audioDeviceID = 0;
            Log << Level::Info << "音频设备已关闭" << op::endl;
        }
        
        // 清理音频重采样上下文
        if (swrContext) {
            swr_free(&swrContext);
            swrContext = nullptr;
        }
        
        // 清理音频解码器上下文
        if (audioCodecContext) {
            avcodec_free_context(&audioCodecContext);
            audioCodecContext = nullptr;
        }
        
        // 清理视频相关资源
        if (rgbFrame) {
            if (rgbFrame->data[0]) {
                av_freep(&rgbFrame->data[0]);
            }
            av_frame_free(&rgbFrame);
            rgbFrame = nullptr;
        }
        
        if (frame) {
            av_frame_free(&frame);
            frame = nullptr;
        }
        
        // 清理图像转换上下文
        if (swsContext) {
            sws_freeContext(swsContext);
            swsContext = nullptr;
        }
        
        // 清理视频解码器上下文
        if (codecContext) {
            avcodec_free_context(&codecContext);
            codecContext = nullptr;
        }
        
        // 最后清理格式上下文（这是最容易出错的地方）
        if (formatContext) {
            // 确保格式上下文是有效的
            AVFormatContext* temp = formatContext;
            formatContext = nullptr; // 先置空，防止重复释放
            
            // 检查指针有效性
            if (temp && temp != (AVFormatContext*)0xdeadbeef) {
                avformat_close_input(&temp);
            }
        }
        
        // 清理其他缓冲区
        if (rgbBuffer) {
            rgbBuffer = nullptr;
            if (rgbBufferSize > 0) {
                MemoryMonitor::getInstance().trackDeallocation(rgbBufferSize);
                rgbBufferSize = 0;
            }
        }
        
        // 重置状态变量
        playing = false;
        shouldExit = true;
        loop = false;
        frameReady = false;
        isFinished = false;
        reachedEOF = false;
        videoDecodeFinished = false;
        audioDecodeFinished = false;
        audioStreamIndex = -1;
        videoStream = nullptr;
        videoStreamIndex = -1;
        width = 0;
        height = 0;
        frameDelay = 0.0;
        
        // 重置音视频同步状态
        audioClock = 0.0;
        videoClock = 0.0;
        frameTimer = 0.0;
        videoCurrentPts = 0.0;
        videoPtsDrift = 0.0;
        lastFramePts = 0.0;
        frameLastDelay = 0.0;
        
        // 重置帧缓冲区
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            currentFrameData = nullptr;
            frameReady = false;
        }
        
    } catch (const std::exception& e) {
        Log << Level::Error << "清理过程中发生异常: " << e.what() << op::endl;
    } catch (...) {
        Log << Level::Error << "清理过程中发生未知异常" << op::endl;
    }
    
    cleaning_up = false;
}
// ================= 音视频同步功能实现 =================

double VideoPlayer::convertPtsToSeconds(int64_t pts, AVRational timeBase) {
    if (pts == AV_NOPTS_VALUE) {
        return 0.0;
    }
    return av_q2d(timeBase) * pts;
}

double VideoPlayer::getAudioClock() {
    std::lock_guard<std::mutex> lock(clockMutex);
    return audioClock.load();
}

double VideoPlayer::getVideoClock() {
    std::lock_guard<std::mutex> lock(clockMutex);
    return videoClock.load();
}

double VideoPlayer::synchronizeVideo(AVFrame* srcFrame, double pts) {
    double frame_delay;
    
    if (pts != 0.0) {
        // 如果有有效的PTS，使用它
        videoClock = pts;
    } else {
        // 否则，根据上一帧计算
        pts = videoClock.load();
    }
    
    // 更新视频时钟下一帧时间
    frame_delay = av_q2d(formatContext->streams[videoStreamIndex]->time_base);
    // 如果是重复帧，调整延迟
    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);
    videoClock = pts + frame_delay;
    
    return pts;
}

void VideoPlayer::updateAudioClock(double audioTimestamp, int audioDataSize) {
    std::lock_guard<std::mutex> lock(clockMutex);
    
    // 计算音频时长
    double audioDuration = 0.0;
    if (audioCodecContext && audioDataSize > 0) {
        // 计算当前音频块的播放时长
        int bytesPerSample = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        int totalBytes = audioDataSize;
        int samplesCount = totalBytes / (bytesPerSample * audioCodecContext->ch_layout.nb_channels);
        audioDuration = (double)samplesCount / audioCodecContext->sample_rate;
    }
    
    // 更新音频时钟
    audioClock = audioTimestamp + audioDuration;
}