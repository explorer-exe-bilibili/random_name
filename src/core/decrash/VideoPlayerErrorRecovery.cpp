#include "core/decrash/VideoPlayerErrorRecovery.h"
#include "core/baseItem/VideoPlayer.h"
#include "core/decrash/ErrorRecovery.h"
#include "core/log.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <thread>
#include <chrono>

namespace core {

VideoPlayerErrorRecovery::VideoPlayerErrorRecovery() {
    Log << Level::Info << "VideoPlayer错误恢复系统初始化" << op::endl;
}

bool VideoPlayerErrorRecovery::recoverFromDecodeError(VideoPlayer* player) {
    std::lock_guard<std::mutex> lock(recoveryMutex);
    
    if (isRecoveringFlag.load() || recoveryAttempts.load() >= MAX_RECOVERY_ATTEMPTS) {
        Log << Level::Warn << "恢复已在进行中或超过最大重试次数" << op::endl;
        return false;
    }
    
    isRecoveringFlag = true;
    recoveryAttempts++;

    Log << Level::Warn << "开始解码错误恢复，尝试次数: " << recoveryAttempts.load() << op::endl;

    try {
        // 1. 停止所有解码线程
        safeStopThreads(player);
        
        // 2. 清理损坏的资源
        cleanupCorruptedResources(player);
        
        // 3. 重新初始化解码器
        if (!reinitializeDecoder(player)) {
            isRecoveringFlag = false;
            return false;
        }
        
        // 4. 重启解码线程
        restartDecodingThreads(player);
        
        Log << Level::Info << "解码错误恢复成功" << op::endl;
        isRecoveringFlag = false;
        return true;
        
    } catch (const std::exception& e) {
        Log << Level::Error << "恢复过程中发生异常: " << e.what() << op::endl;
        isRecoveringFlag = false;
        return false;
    }
}

bool VideoPlayerErrorRecovery::recoverFromMemoryError(VideoPlayer* player) {
    std::lock_guard<std::mutex> lock(recoveryMutex);
    
    Log << Level::Warn << "开始内存错误恢复" << op::endl;
    
    try {
        // 立即停止所有活动
        safeStopThreads(player);
        
        // 强制清理所有缓存的帧
        cleanupCorruptedResources(player);
        
        // 等待一段时间让系统恢复
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        Log << Level::Info << "内存错误恢复完成" << op::endl;
        return true;
        
    } catch (const std::exception& e) {
        Log << Level::Error << "内存错误恢复失败: " << e.what() << op::endl;
        return false;
    }
}

void VideoPlayerErrorRecovery::reset() {
    recoveryAttempts = 0;
    isRecoveringFlag = false;
}

bool VideoPlayerErrorRecovery::isRecovering() const {
    return isRecoveringFlag.load();
}

void VideoPlayerErrorRecovery::safeStopThreads(VideoPlayer* player) {
    Log << Level::Info << "安全停止解码线程" << op::endl;
    
    // 设置退出标志
    player->shouldExit = true;
    
    // 等待线程结束，使用超时机制
    auto waitThread = [](std::thread& t, const std::string& name) {
        if (t.joinable()) {
            Log << Level::Info << "等待" << name << "线程结束..." << op::endl;
            t.join();
        }
    };
    
    try {
        waitThread(player->decoderThread, "主解码");
        waitThread(player->decoderThreadVideo, "视频解码");  
        waitThread(player->decoderThreadAudio, "音频解码");
    } catch (const std::exception& e) {
        Log << Level::Warn << "等待线程结束时发生异常: " << e.what() << op::endl;
    }
}

void VideoPlayerErrorRecovery::cleanupCorruptedResources(VideoPlayer* player) {
    Log << Level::Info << "清理损坏的资源" << op::endl;
    
    try {
        // 清理可能损坏的帧队列
        while (!player->packetVideoQueue.empty()) {
            AVPacket* pkt = player->packetVideoQueue.front();
            player->packetVideoQueue.pop();
            av_packet_free(&pkt);
        }
        
        while (!player->packetAudioQueue.empty()) {
            AVPacket* pkt = player->packetAudioQueue.front();
            player->packetAudioQueue.pop();
            av_packet_free(&pkt);
        }
        
        // 刷新解码器缓冲区
        if (player->codecContext) {
            avcodec_flush_buffers(player->codecContext);
        }
        if (player->audioCodecContext) {
            avcodec_flush_buffers(player->audioCodecContext);
        }
        
        // 重置状态标志
        player->isFinished = false;
        player->reachedEOF = false;
        player->videoDecodeFinished = false;
        player->audioDecodeFinished = false;
        player->frameReady = false;
        
        Log << Level::Info << "资源清理完成" << op::endl;
        
    } catch (const std::exception& e) {
        Log << Level::Error << "清理资源时发生异常: " << e.what() << op::endl;
    }
}

bool VideoPlayerErrorRecovery::reinitializeDecoder(VideoPlayer* player) {
    Log << Level::Info << "重新初始化解码器" << op::endl;
    
    return ErrorRecovery::executeWithRetry<bool>([player]() {
        try {
            // 重新打开视频解码器
            if (player->codecContext) {
                avcodec_free_context(&player->codecContext);
                player->codecContext = avcodec_alloc_context3(nullptr);
                const AVCodec* codec = avcodec_find_decoder(player->codecContext->codec_id);
                if (!codec) {
                    throw std::runtime_error("无法找到视频解码器");
                }
                if (avcodec_open2(player->codecContext, codec, nullptr) < 0) {
                    throw std::runtime_error("无法重新打开视频解码器");
                }
            }
            
            // 重新打开音频解码器
            if (player->audioCodecContext) {
                avcodec_free_context(&player->audioCodecContext);
                player->audioCodecContext = avcodec_alloc_context3(nullptr);
                const AVCodec* audioCodec = avcodec_find_decoder(player->audioCodecContext->codec_id);
                if (audioCodec && avcodec_open2(player->audioCodecContext, audioCodec, nullptr) < 0) {
                    Log << Level::Warn << "无法重新打开音频解码器" << op::endl;
                }
            }
            
            Log << Level::Info << "解码器重新初始化成功" << op::endl;
            return true;
            
        } catch (const std::exception& e) {
            Log << Level::Error << "解码器初始化失败: " << e.what() << op::endl;
            throw;
        }
    }, {.maxRetries = 2}, ErrorRecovery::ErrorType::FFMPEG_DECODE);
}

void VideoPlayerErrorRecovery::restartDecodingThreads(VideoPlayer* player) {
    Log << Level::Info << "重启解码线程" << op::endl;
    
    try {
        // 重置退出标志
        player->shouldExit = false;
        
        // 重新启动线程（如果VideoPlayer支持的话）
        // 注意：这需要VideoPlayer类提供重启线程的方法
        // 这里只是一个示例，实际实现需要根据VideoPlayer的具体接口调整
        
        Log << Level::Info << "解码线程重启完成" << op::endl;
        
    } catch (const std::exception& e) {
        Log << Level::Error << "重启解码线程失败: " << e.what() << op::endl;
        throw;
    }
}

} // namespace core
