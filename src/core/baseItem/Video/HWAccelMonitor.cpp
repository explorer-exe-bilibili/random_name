#include "core/baseItem/Video/HWAccelMonitor.h"

#include <vector>
#include <algorithm>

namespace core {

HWAccelMonitor& HWAccelMonitor::getInstance() {
    static HWAccelMonitor instance;
    return instance;
}

HWAccelMonitor::HWAccelMonitor() 
    : lastFpsUpdateTime(std::chrono::steady_clock::now()) {
}

void HWAccelMonitor::setHardwareAccelEnabled(bool enabled) {
    hwAccelEnabled = enabled;
    
    if (enabled) {
        // 重置计数器
        totalFramesDecoded = 0;
        lastFpsUpdateTime = std::chrono::steady_clock::now();
        currentFps = 0.0;
    }
}

bool HWAccelMonitor::isHardwareAccelEnabled() const {
    return hwAccelEnabled;
}

void HWAccelMonitor::setHWAccelType(const std::string& type) {
    std::lock_guard<std::mutex> lock(typeMutex);
    hwAccelType = type;
}

std::string HWAccelMonitor::getHWAccelType() const {
    std::lock_guard<std::mutex> lock(typeMutex);
    return hwAccelType;
}

void HWAccelMonitor::updateHWFrameCount() {
    if (!hwAccelEnabled) {
        return;
    }
    
    totalFramesDecoded++;
    
    // 每秒更新一次FPS
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastFpsUpdateTime).count();
    
    if (elapsed >= 1000) {  // 1秒
        updateFps();
    }
}

void HWAccelMonitor::updateFps() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastFpsUpdateTime).count();
    
    if (elapsed > 0) {
        std::lock_guard<std::mutex> lock(fpsMutex);
        double fps = (totalFramesDecoded * 1000.0) / elapsed;
        currentFps = fps;
        
        // 重置计数和时间
        totalFramesDecoded = 0;
        lastFpsUpdateTime = now;
    }
}

double HWAccelMonitor::getHWFramesPerSecond() const {
    std::lock_guard<std::mutex> lock(fpsMutex);
    return currentFps;
}

void HWAccelMonitor::logHWError(const std::string& errorMessage) {
    std::lock_guard<std::mutex> lock(errorMutex);
    
    // 添加时间戳
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    char timeBuffer[26];
    struct tm timeInfo;
    localtime_s(&timeInfo, &now_time_t);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    
    std::string timestampedError = std::string(timeBuffer) + " - " + errorMessage;
    
    // 添加到队列
    errorLog.push(timestampedError);
    
    // 保持队列大小限制
    if (errorLog.size() > MAX_ERROR_LOG_SIZE) {
        errorLog.pop();
    }
}

std::vector<std::string> HWAccelMonitor::getRecentHWErrors(int count) const {
    std::lock_guard<std::mutex> lock(errorMutex);
    
    std::vector<std::string> errors;
    std::queue<std::string> tempQueue = errorLog;  // 创建一个临时副本
    
    // 限制返回数量
    count = std::min(count, static_cast<int>(tempQueue.size()));
    
    // 我们需要从最近的开始，但队列是先进先出的
    // 所以我们先获取所有元素，然后反转
    while (!tempQueue.empty() && errors.size() < count) {
        errors.push_back(tempQueue.front());
        tempQueue.pop();
    }
    
    // 反转顺序，使最新的错误在前面
    std::reverse(errors.begin(), errors.end());
    
    return errors;
}

void HWAccelMonitor::resetStats() {
    // 重置帧计数和FPS
    {
        std::lock_guard<std::mutex> lock(fpsMutex);
        totalFramesDecoded = 0;
        lastFpsUpdateTime = std::chrono::steady_clock::now();
        currentFps = 0.0;
    }
    
    // 清除错误日志
    {
        std::lock_guard<std::mutex> lock(errorMutex);
        std::queue<std::string> empty;
        std::swap(errorLog, empty);
    }
}

} // namespace core