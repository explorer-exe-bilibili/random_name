#ifndef LOG_H
#define LOG_H
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#define INSIDEINT 1143221
// 定义日志级别枚举

class Log {
private:
    std::ofstream logFile;
    static bool CreatedMultipleDirectory(const std::string& direct);
public:
    Log(const std::string& fileName, bool add);
    ~Log();

    Log& operator<<(const std::string& str);
    Log& operator<<(long double value);
    // 可以根据需要重载更多其他类型的<<运算符函数
    int nl();
    int pt();
private:
    void writeToLog(const std::string& message);
};
#endif // LOG_H