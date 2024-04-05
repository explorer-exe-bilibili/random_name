#ifndef LOGFILE_H
#define LOGFILE_H
#include <string>
#include <fstream>
#include <iostream>
#include<wchar.h>
#include <wtypes.h>
class LogFile
{
public:
    LogFile(std::string filename, bool isate);
    ~LogFile();
    void flush();
    LogFile& operator<<(const std::string& log);
    LogFile& operator<<(double log);
    LogFile& operator<<(const wchar_t* log);

private:
    std::ofstream m_file;
    std::string LWStostr(const wchar_t* lpcwszStr);
    void close();
};
#endif