#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include "baseItem/Base.h"


enum class operation {
    time,
    endl,
    flush
};
enum class Level {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    none
};
typedef operation op;

class Log_ {
public:
    Level currentLevel=Level::none;
    std::ofstream logFile;
    std::unique_ptr<std::thread> AutoFlushThread;
    std::atomic<bool> autoFlushEnabled = true; // 默认启用自动刷新
    std::atomic<bool> quiting = false; // 线程退出标志
    // 添加析构函数，安全地停止线程
    ~Log_() {
        Stop();
    }
    
    static std::string LevelToString(Level level) {
        switch (level) {
            case Level::Trace: return "TRACE";
            case Level::Debug: return "DEBUG";
            case Level::Info:  return "INFO";
            case Level::Warn:  return "WARN";
            case Level::Error: return "ERROR";
            case Level::none: return "NONE";
            default: return "UNKNOWN";
        }
    }
    bool setFile(const std::string& filename);
    void refresh();

    void Init();
    
    // 添加安全停止线程的方法
    void Stop();

    void setAutoFlush(bool on){autoFlushEnabled=on;}

    void LogMessage(const std::string& message);
    Log_& operator<< (const std::string& message) {
        LogMessage(message);
        return *this;
    }
    Log_& operator<<(const std::wstring& message) {
        LogMessage(core::wstring2string(message));
        return *this;
    }
    Log_& operator<<(Level level);
    Log_& operator<<(operation op);
    Log_& operator<<(const core::Point& point) {
        LogMessage("[" + std::to_string(point.getx()) + "," + std::to_string(point.gety()) + "]");
        return *this;
    }
    Log_& operator<<(const core::Size& size) {
        LogMessage("[" + std::to_string(size.getw()) + "," + std::to_string(size.geth()) + "]");
        return *this;
    }
    Log_& operator<<(const core::Region& region) {
        LogMessage("[" + std::to_string(region.getx()) + "," + std::to_string(region.gety()) + "|" +
                   std::to_string(region.getxend()) + "," + std::to_string(region.getyend()) + "]");
        return *this;
    }
    Log_& operator<<(const core::ScreenInfo& screenInfo) {
        LogMessage("[WindowWidth:" + std::to_string(screenInfo.width) + ",WindowHeight:" + std::to_string(screenInfo.height) +
                   ",AspectRatio:" + std::to_string(screenInfo.aspectRatio) + ",DPI:" + std::to_string(screenInfo.dpi) +
                   ",PixelRatio:" + std::to_string(screenInfo.pixelRatio) + "]");
        return *this;
    }
    Log_& operator<<(long double value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(double value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(float value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(long value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(long long value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(unsigned long value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(unsigned long long value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(unsigned int value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(int value) {
        LogMessage(std::to_string(value));
        return *this;
    }
    Log_& operator<<(char value) {
        LogMessage(std::string(1, value));
        return *this;
    }
    Log_& operator<<(wchar_t value) {
        LogMessage(std::string(1, value));
        return *this;
    }
};

class Logerr:public Log_ {
    Level currentLevel=Level::Error;
    public:
    void Init();
    void LogMessage(const std::string& message); // 添加LogMessage声明
    void refresh(); // 添加refresh声明
    Logerr& operator<<(Level level);
};

extern Log_ Log;