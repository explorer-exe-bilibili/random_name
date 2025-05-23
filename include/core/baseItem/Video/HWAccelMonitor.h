#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>

namespace core {

/**
 * @brief 硬件加速监控类，用于跟踪和报告视频硬件加速的使用情况和性能
 * 
 * 此类使用单例模式实现，可在应用程序的不同部分共享硬件加速状态信息
 */
class HWAccelMonitor {
public:
    /**
     * @brief 获取HWAccelMonitor单例实例
     * @return HWAccelMonitor& 单例实例的引用
     */
    static HWAccelMonitor& getInstance();

    /**
     * @brief 设置硬件加速是否启用
     * @param enabled 启用状态
     */
    void setHardwareAccelEnabled(bool enabled);

    /**
     * @brief 获取硬件加速启用状态
     * @return bool 是否启用硬件加速
     */
    bool isHardwareAccelEnabled() const;

    /**
     * @brief 设置硬件加速类型
     * @param type 硬件加速类型的字符串表示（如"cuda"、"d3d11va"等）
     */
    void setHWAccelType(const std::string& type);

    /**
     * @brief 获取硬件加速类型
     * @return std::string 硬件加速类型
     */
    std::string getHWAccelType() const;

    /**
     * @brief 更新硬件加速帧统计
     */
    void updateHWFrameCount();

    /**
     * @brief 获取硬件加速每秒帧数
     * @return double 当前每秒帧数
     */
    double getHWFramesPerSecond() const;

    /**
     * @brief 记录硬件加速错误
     * @param errorMessage 错误信息
     */
    void logHWError(const std::string& errorMessage);

    /**
     * @brief 获取最近的硬件加速错误
     * @param count 要获取的错误数量
     * @return std::vector<std::string> 错误信息列表
     */
    std::vector<std::string> getRecentHWErrors(int count = 5) const;

    /**
     * @brief 重置所有统计数据
     */
    void resetStats();

private:
    // 私有构造函数，防止外部实例化
    HWAccelMonitor();
    
    // 禁用拷贝和移动
    HWAccelMonitor(const HWAccelMonitor&) = delete;
    HWAccelMonitor(HWAccelMonitor&&) = delete;
    HWAccelMonitor& operator=(const HWAccelMonitor&) = delete;
    HWAccelMonitor& operator=(HWAccelMonitor&&) = delete;

    std::atomic<bool> hwAccelEnabled{false};
    std::string hwAccelType;
    mutable std::mutex typeMutex;

    // 帧率计算
    std::atomic<uint64_t> totalFramesDecoded{0};
    std::chrono::steady_clock::time_point lastFpsUpdateTime;
    std::atomic<double> currentFps{0.0};
    mutable std::mutex fpsMutex;
    
    // 定期更新FPS的方法
    void updateFps();

    // 错误日志
    std::queue<std::string> errorLog;
    mutable std::mutex errorMutex;
    static const size_t MAX_ERROR_LOG_SIZE = 20;
};

} // namespace core