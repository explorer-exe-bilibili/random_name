#include"core/log.h"
#include "core/baseItem/Base.h"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif



Log_ Log;
Logerr logErr;
std::string LogBuffer; // 改名为LogBuffer，为主日志使用
std::string ErrorBuffer; // 为错误日志使用
Level currentLevel=Level::none;
std::mutex LogMutex, ErrorMutex;

bool Log_::setFile(const std::string& filename) {
    std::string actualFilename = filename;
    
    try {
        std::filesystem::path logPath;
        
        if (filename.find(':') == std::string::npos && filename.substr(0, 2) != "\\\\") {
            // 相对路径
#ifdef _WIN32
            if (core::g_programDirWritable) {
                // 程序目录可写，使用程序目录
                logPath = core::g_executableDir / std::filesystem::u8path(filename);
            } else {
                // 程序目录不可写，使用用户数据目录
                logPath = core::g_userDataDir / std::filesystem::u8path(filename);
            }
#else
            logPath = std::filesystem::current_path() / filename;
#endif
        } else {
            // 绝对路径
            logPath = std::filesystem::u8path(filename);
        }
        
        actualFilename = logPath.string();
        
    } catch (const std::exception& e) {
        std::cout << "日志路径处理异常: " << e.what() << ", 使用原始路径" << std::endl;
        actualFilename = filename;
    }

    // 如果文件已存在，删除它
    if(std::filesystem::exists(actualFilename)) {
        std::filesystem::remove(actualFilename);
    }
    
    //关闭之前的文件
    if (logFile.is_open()) {
        logFile.close();
    }
    logFile.open(actualFilename);
    if (logFile.is_open()) {
        std::cout << "日志文件位置: " << actualFilename << std::endl;
    }
    return logFile.is_open();
}

void Log_::refresh() {
    std::lock_guard<std::mutex> lock(LogMutex);
    if (logFile.is_open()) {
        logFile << LogBuffer;
        logFile.flush();
        LogBuffer.clear(); // 清空缓冲区
    }
}

void Log_::Init() {
    LogBuffer.clear();
    setFile("files/log/full.log");
    // 确保线程启动前标志位已设置为true
    autoFlushEnabled = true;
    // 创建新线程前先检查并停止已有线程
    if (AutoFlushThread && AutoFlushThread->joinable()) {
        AutoFlushThread->join();
    }
    // 创建新线程
    AutoFlushThread = std::make_unique<std::thread>(
        [this]() {
            while (autoFlushEnabled) {
                refresh();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    );
    logErr.Init();
}

void Logerr::Init() {
    ErrorBuffer.clear();
    setFile("files/log/error.log");
    // 确保线程启动前标志位已设置为true
    autoFlushEnabled = true;
    // 创建新线程前先检查并停止已有线程
    if (AutoFlushThread && AutoFlushThread->joinable()) {
        AutoFlushThread->join();
    }
    // 创建新线程
    AutoFlushThread = std::make_unique<std::thread>(
        [this]() {
            while (1) {
                if(autoFlushEnabled) 
                    refresh();
                if(quiting) 
                    break; // 退出循环
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    );
}

void Log_::LogMessage(const std::string& message) {
    std::cout << message; // 输出到控制台
    LogBuffer += message;
    // 只有当日志级别是错误时，才将消息写入错误日志
    if(currentLevel==Level::Error) {
        logErr.LogMessage(message);
    }
}

// 添加Logerr::LogMessage实现
void Logerr::LogMessage(const std::string& message) {
    try{
        ErrorBuffer += message;
    }
    catch(...){
        std::cerr << "Error writing to error log: " << message << std::endl;
    }

}

Log_& Log_::operator<<(operation op) {
    switch (op) {
        case operation::time:
        {
            auto now = std::chrono::system_clock::now();
            std::time_t time_now = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;

            char buffer[32];
            #ifdef _WIN32
            std::tm timeinfo;
            localtime_s(&timeinfo, &time_now);
            std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
            #else
            std::tm* timeinfo = std::localtime(&time_now);
            std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
            #endif

            char time_with_ms[40];
            std::sprintf(time_with_ms, "%s.%03lld", buffer, static_cast<long long>(ms.count()));
            LogMessage("[" + std::string(time_with_ms) + "] ");
        }break;
        case operation::endl:
            LogMessage("\n");
            break;
        case operation::flush:
            refresh();
            logErr.refresh();
            break;
    }
    return *this;
}

Log_& Log_::operator<<(Level level) {
    *this << op::time;
    currentLevel = level;
    LogMessage("[" + LevelToString(level) + "] ");
    return *this;
}

Logerr& Logerr::operator<<(Level level) {
    // 只处理Error级别的日志
    if(level==Level::Error) {
        currentLevel = level; // 确保设置当前级别
        LogMessage("[ERROR]");
    }
    return *this;
}

// 添加Logerr::refresh实现
void Logerr::refresh() {
    std::lock_guard<std::mutex> lock(ErrorMutex);
    if (logFile.is_open()) {
        logFile << ErrorBuffer;
        logFile.flush();
        ErrorBuffer.clear(); // 清空缓冲区
    }
}

// 添加Stop方法实现，安全地停止线程
void Log_::Stop() {
    // 设置标志位，通知线程停止循环
    autoFlushEnabled = false;
    quiting = true; // 设置退出标志
    
    // 最后一次刷新缓冲区
    refresh();
    
    // 如果线程存在并且可以被join，则等待它结束
    if (AutoFlushThread && AutoFlushThread->joinable()) {
        AutoFlushThread->join();
    }
    
    // 关闭日志文件
    if (logFile.is_open()) {
        logFile.close();
    }
}