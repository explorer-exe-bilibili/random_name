#include "core/decrash/MemoryMonitor.h"
#include "core/log.h"
#include "core/explorer.h"

#ifdef __linux__
#include <malloc.h>
#endif

namespace core {

MemoryMonitor& MemoryMonitor::getInstance() {
    static MemoryMonitor instance;
    return instance;
}

void MemoryMonitor::trackAllocation(size_t size) {
    size_t newUsage = currentMemoryUsage.fetch_add(size) + size;
    
    // 更新峰值使用量
    size_t currentPeak = peakMemoryUsage.load();
    while (newUsage > currentPeak && 
           !peakMemoryUsage.compare_exchange_weak(currentPeak, newUsage)) {
        // 继续尝试更新峰值
    }
    
    checkMemoryThresholds(newUsage);
}

void MemoryMonitor::trackDeallocation(size_t size) {
    currentMemoryUsage.fetch_sub(size);
}

void MemoryMonitor::forceGarbageCollection() {
    auto now = std::chrono::steady_clock::now();
    auto lastGCTime = lastGC.load();
    
    // 避免过于频繁的垃圾回收
    if (now - lastGCTime < MIN_GC_INTERVAL) {
        return;
    }
    
    Log << Level::Info << "执行强制垃圾回收，当前内存使用: " 
        << (currentMemoryUsage.load() / 1024 / 1024) << " MB" << op::endl;
    
    try {
        // 清理未使用的资源
        cleanupUnusedResources();
        
        // 强制内存压缩 (Linux特定)
        #ifdef __linux__
        malloc_trim(0);
        #endif
        
        lastGC.store(now);
        
        Log << Level::Info << "垃圾回收完成，当前内存使用: " 
            << (currentMemoryUsage.load() / 1024 / 1024) << " MB" << op::endl;
            
    } catch (const std::exception& e) {
        Log << Level::Error << "垃圾回收过程中发生异常: " << e.what() << op::endl;
    }
}

void MemoryMonitor::checkMemoryPressure() {
    size_t usage = getCurrentUsage();
    checkMemoryThresholds(usage);
    
    if (isMemoryPressure()) {
        forceGarbageCollection();
    }
}

size_t MemoryMonitor::getCurrentUsage() const {
    return currentMemoryUsage.load();
}

size_t MemoryMonitor::getPeakUsage() const {
    return peakMemoryUsage.load();
}

bool MemoryMonitor::isMemoryPressure() const {
    return currentMemoryUsage.load() > MEMORY_WARNING_THRESHOLD;
}

void MemoryMonitor::checkMemoryThresholds(size_t usage) {
    if (usage > MEMORY_CRITICAL_THRESHOLD) {
        Log << Level::Error << "内存使用量达到临界值: " 
            << (usage / 1024 / 1024) << " MB，执行紧急垃圾回收" << op::endl;
        forceGarbageCollection();
    } else if (usage > MEMORY_WARNING_THRESHOLD) {
        Log << Level::Warn << "内存使用量较高: " 
            << (usage / 1024 / 1024) << " MB" << op::endl;
    }
}

void MemoryMonitor::cleanupUnusedResources() {
    Log << Level::Info << "清理未使用的资源" << op::endl;
    
    // 尝试清理Explorer中的未使用资源
    try {
        if (auto explorer = Explorer::getInstance2()) {
            // 这里可以调用Explorer的资源清理方法
            // explorer->cleanupUnusedResources();
        }
    } catch (const std::exception& e) {
        Log << Level::Warn << "清理Explorer资源时发生异常: " << e.what() << op::endl;
    }
}

} // namespace core
