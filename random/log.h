#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#define INSIDEINT 1143221
// 定义日志级别枚举

class Log {
private:
    std::ofstream logFile;
    static bool CreatedMultipleDirectory(const std::string& direct);
public:
    static std::wstring wrunpath;
    static std::string runpath;
    Log(const std::string& fileName, bool add);
    ~Log();

    Log& operator<<(const std::string& str);
    Log& operator<<(const std::wstring& str);
    Log& operator<<(long double value);
    // 可以根据需要重载更多其他类型的<<运算符函数
    int nl();
    int pt();
private:
    void writeToLog(const std::string& message);
};