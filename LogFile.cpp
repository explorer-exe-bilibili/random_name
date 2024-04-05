#include "LogFile.h"

LogFile::LogFile(std::string filename, bool isate)
{
    if (isate)
        m_file.open(filename.c_str(), std::ios::ate);
    else
        m_file.open(filename.c_str(), std::ios::app);
    return;
}

inline LogFile::~LogFile()
{
    close();
}

inline void LogFile::close()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

inline void LogFile::flush()
{
    m_file.flush();
}

inline LogFile& LogFile::operator<<(const std::string& log)
{
    m_file << log;
    return *this;
}


inline LogFile& LogFile::operator<<(double log)
{
    m_file << log;
    return *this;
}

inline LogFile& LogFile::operator<<(const wchar_t* log)
{
    m_file << LWStostr(log);
    return *this;
}

inline std::string LogFile::LWStostr(const wchar_t* lpcwszStr)
{
    std::string str;
    DWORD dwMinSize = 0;
    LPSTR lpszStr = NULL;
    dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
    if (0 == dwMinSize)
    {
        return "";
    }
    lpszStr = new char[dwMinSize];
    WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
    str = lpszStr;
    delete[] lpszStr;
    return str;
}