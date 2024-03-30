#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#define INSIDEINT 1143221
// 定义日志级别枚举
std::string TCHAR2STRING(TCHAR* STR);
CHAR runpath[MAX_PATH] = { NULL };

class Log {
private:
    std::ofstream logFile;

public:
    Log(const std::string& fileName,bool add) {
        std::string filepath;
        filepath = runpath;
        filepath += fileName;
        if(add)
            logFile.open(fileName, std::ios::app /*std::ios::app*/);
        else logFile.open(fileName,std::ios::ate /*std::ios::app*/);
    }

    ~Log() {
        logFile.close();
    }

    Log& operator<<(const std::string& str) {
        writeToLog(str);
        return *this;
    }

    Log& operator<<(int value) {
        if(value!=INSIDEINT)
        writeToLog(std::to_string(value));
        return *this;
    }

    Log& operator<<(double value) {
        writeToLog(std::to_string(value));
        return *this;
    }
    Log& operator<<(TCHAR *value) {
        writeToLog(TCHAR2STRING(value));
        return *this;
    }
    // 可以根据需要重载更多其他类型的<<运算符函数
    int nl() {
        logFile << std::endl;
        return INSIDEINT;
    }
    int pt() {
        time_t now = time(0);
        struct tm* localTime = localtime(&now);
        char timeStr[32];
        sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
        logFile <<"[" << timeStr << "]";
        return INSIDEINT;
    }
private:
    void writeToLog(const std::string& message) {
        logFile <<  message;
    }
};

std::string TCHAR2STRING(TCHAR* STR)
{
    int iLen = WideCharToMultiByte(CP_UTF8, 0, STR, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_UTF8, 0, STR, -1, chRtn, iLen, NULL, NULL);
    std::string str(chRtn);
    delete chRtn;
    return str;
}
char* TCHAR2CHAR(TCHAR* STR)
{
    int iLen = WideCharToMultiByte(CP_UTF8, 0, STR, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_UTF8, 0, STR, -1, chRtn, iLen, NULL, NULL);
    return chRtn;
}