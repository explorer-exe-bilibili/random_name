#pragma once

#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>
#include "core/log.h"
#undef T

namespace core {

class ErrorRecovery {
public:
    enum class ErrorType {
        MEMORY_ALLOCATION,
        OPENGL_CONTEXT,
        FFMPEG_DECODE,
        THREAD_SYNC,
        RESOURCE_LEAK,
        VIDEO_DECODE,
        AUDIO_DECODE
    };

    struct RetryConfig {
        int maxRetries = 3;
        std::chrono::milliseconds baseDelay{100};
        float backoffMultiplier = 2.0f;
        bool exponentialBackoff = true;
    };
    template<typename T>
    static T executeWithRetry(
        std::function<T()> operation,
        const RetryConfig& config = RetryConfig{3,std::chrono::milliseconds(100),2.0f,true},
        ErrorType errorType = ErrorType::MEMORY_ALLOCATION
    );

    static void registerErrorHandler(ErrorType type, std::function<void()> handler);
    static void logError(ErrorType type, const std::string& message);
    static bool shouldAbort();
    static void reset();

private:
    static std::atomic<int> consecutiveErrors;
    static std::mutex errorMutex;
    static std::unordered_map<ErrorType, std::function<void()>> errorHandlers;
    static const int MAX_CONSECUTIVE_ERRORS = 10;
};

// 模板函数实现
template<typename T>
T ErrorRecovery::executeWithRetry(
    std::function<T()> operation,
    const RetryConfig& config,
    ErrorType errorType
) {
    int attempt = 0;
    std::chrono::milliseconds delay = config.baseDelay;

    while (attempt < config.maxRetries) {
        try {
            T result = operation();
            consecutiveErrors = 0; // 重置错误计数
            return result;
        }
        catch (const std::exception& e) {
            attempt++;
            consecutiveErrors++;
            
            logError(errorType, "尝试 " + std::to_string(attempt) + " 失败: " + e.what());
            
            if (attempt >= config.maxRetries) {
                // 执行错误处理器
                auto it = errorHandlers.find(errorType);
                if (it != errorHandlers.end()) {
                    it->second();
                }
                throw; // 重新抛出异常
            }
            
            // 等待后重试
            std::this_thread::sleep_for(delay);
            
            if (config.exponentialBackoff) {
                delay = std::chrono::milliseconds(
                    static_cast<long long>(delay.count() * config.backoffMultiplier)
                );
            }
        }
    }
    
    throw std::runtime_error("所有重试都失败了");
}

} // namespace core
