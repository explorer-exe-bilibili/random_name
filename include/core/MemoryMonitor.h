#pragma once

#include <atomic>
#include <chrono>
#include <memory>

namespace core {

class MemoryMonitor {
public:
    static MemoryMonitor& getInstance();
    
    void trackAllocation(size_t size);
    void trackDeallocation(size_t size);
    void forceGarbageCollection();
    void checkMemoryPressure();
    size_t getCurrentUsage() const;
    size_t getPeakUsage() const;
    bool isMemoryPressure() const;

private:
    MemoryMonitor() = default;
    ~MemoryMonitor() = default;
    MemoryMonitor(const MemoryMonitor&) = delete;
    MemoryMonitor& operator=(const MemoryMonitor&) = delete;
    
    void checkMemoryThresholds(size_t usage);
    void cleanupUnusedResources();
    
    std::atomic<size_t> currentMemoryUsage{0};
    std::atomic<size_t> peakMemoryUsage{0};
    std::atomic<std::chrono::steady_clock::time_point> lastGC{std::chrono::steady_clock::now()};
    
    static const size_t MEMORY_WARNING_THRESHOLD = 1024 * 1024 * 1024;  // 1GB
    static const size_t MEMORY_CRITICAL_THRESHOLD = 2ULL * 1024 * 1024 * 1024; // 2GB
    static constexpr std::chrono::minutes MIN_GC_INTERVAL{1}; // 最小垃圾回收间隔
};

} // namespace core
