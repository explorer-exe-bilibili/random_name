#pragma once

#include <atomic>
#include <mutex>
#include <memory>

namespace core {

class VideoPlayer;

class VideoPlayerErrorRecovery {
public:
    VideoPlayerErrorRecovery();
    ~VideoPlayerErrorRecovery() = default;

    bool recoverFromDecodeError(VideoPlayer* player);
    bool recoverFromMemoryError(VideoPlayer* player);
    void reset();
    bool isRecovering() const;

private:
    std::atomic<bool> isRecoveringFlag{false};
    std::mutex recoveryMutex;
    std::atomic<int> recoveryAttempts{0};
    
    static constexpr int MAX_RECOVERY_ATTEMPTS = 3;
    
    void cleanupCorruptedResources(VideoPlayer* player);
    bool reinitializeDecoder(VideoPlayer* player);
    void restartDecodingThreads(VideoPlayer* player);
    void safeStopThreads(VideoPlayer* player);
};

} // namespace core
