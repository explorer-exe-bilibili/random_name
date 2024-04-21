#include "sth2sth.h"
#include <vector>

LPCWSTR sth2sth::UTF8To16(const char* utf8String)
{
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, nullptr, 0);
    wchar_t* wideString = new wchar_t[bufferSize];

    MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, wideString, bufferSize);

    return wideString;
}
std::string sth2sth::LWStostr(LPCWSTR lpcwszStr)
{
    std::string str;
    DWORD dwMinSize = 0;
    LPSTR lpszStr = NULL;
    dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
    if (0 == dwMinSize)
    {
        return FALSE;
    }
    lpszStr = new char[dwMinSize];
    WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
    str = lpszStr;
    delete[] lpszStr;
    return str;
}

std::wstring sth2sth::Utf82Unicode(const std::string& utf8string)
{
    int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
    if (widesize == ERROR_NO_UNICODE_TRANSLATION)
    {
        throw std::exception("Invalid UTF-8 sequence.");
    }
    if (widesize == 0)
    {
        throw std::exception("Error in conversion.");
    }

    std::vector<wchar_t> resultstring(widesize);

    int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

    if (convresult != widesize)
    {
        throw std::exception("La falla!");
    }

    return std::wstring(&resultstring[0]);
}

//unicode 转为 ascii  
std::string sth2sth::WideByte2Acsi(std::wstring& wstrcode)
{
    int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
    if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
    {
        throw std::exception("Invalid UTF-8 sequence.");
    }
    if (asciisize == 0)
    {
        throw std::exception("Error in conversion.");
    }
    std::vector<char> resultstring(asciisize);
    int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);

    if (convresult != asciisize)
    {
        throw std::exception("La falla!");
    }

    return std::string(&resultstring[0]);
}
//utf-8 转 ascii  
std::string sth2sth::UTF_82ASCII(std::string& strUtf8Code)
{
    std::string strRet("");
    //先把 utf8 转为 unicode  
    std::wstring wstr = Utf82Unicode(strUtf8Code);
    //最后把 unicode 转为 ascii  
    strRet = WideByte2Acsi(wstr);
    return strRet;
}

char* sth2sth::const_char_ptr_to_char_ptr(const char* const_char_ptr) {
    return const_cast<char*>(const_char_ptr);
}

std::string sth2sth::TCHAR2STRING(TCHAR* STR)
{
    int iLen = WideCharToMultiByte(CP_UTF8, 0, STR, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_UTF8, 0, STR, -1, chRtn, iLen, NULL, NULL);
    std::string str(chRtn);
    delete chRtn;
    return str;
}
char* sth2sth::TCHAR2CHAR(TCHAR* STR)
{
    int iLen = WideCharToMultiByte(CP_UTF8, 0, STR, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_UTF8, 0, STR, -1, chRtn, iLen, NULL, NULL);
    return chRtn;
}