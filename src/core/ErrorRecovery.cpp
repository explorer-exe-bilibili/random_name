#include "core/ErrorRecovery.h"
#include <thread>
#include <iostream>

namespace core {

std::atomic<int> ErrorRecovery::consecutiveErrors{0};
std::mutex ErrorRecovery::errorMutex;
std::unordered_map<ErrorRecovery::ErrorType, std::function<void()>> ErrorRecovery::errorHandlers;

void ErrorRecovery::registerErrorHandler(ErrorType type, std::function<void()> handler) {
    std::lock_guard<std::mutex> lock(errorMutex);
    errorHandlers[type] = handler;
}

void ErrorRecovery::logError(ErrorType type, const std::string& message) {
    std::string typeStr;
    switch (type) {
        case ErrorType::MEMORY_ALLOCATION: typeStr = "内存分配"; break;
        case ErrorType::OPENGL_CONTEXT: typeStr = "OpenGL上下文"; break;
        case ErrorType::FFMPEG_DECODE: typeStr = "FFmpeg解码"; break;
        case ErrorType::THREAD_SYNC: typeStr = "线程同步"; break;
        case ErrorType::RESOURCE_LEAK: typeStr = "资源泄漏"; break;
        case ErrorType::VIDEO_DECODE: typeStr = "视频解码"; break;
        case ErrorType::AUDIO_DECODE: typeStr = "音频解码"; break;
        default: typeStr = "未知"; break;
    }
    
    Log << Level::Error << "[" << typeStr << "错误] " << message << op::endl;
}

bool ErrorRecovery::shouldAbort() {
    return consecutiveErrors.load() >= MAX_CONSECUTIVE_ERRORS;
}

void ErrorRecovery::reset() {
    consecutiveErrors = 0;
}

} // namespace core
