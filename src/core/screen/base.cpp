#include "core/baseItem/Base.h"
#include <locale>
#include <cstring>
#include <utf8cpp/utf8.h>

using namespace core;

ScreenInfo core::screenInfo;

std::string wstring2string(const std::wstring& wstr) {
    std::string str;
    utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(str));
    return str;
}

std::wstring string2wstring(const std::string& str) {
    std::wstring wstr;
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(wstr));
    return wstr;
}