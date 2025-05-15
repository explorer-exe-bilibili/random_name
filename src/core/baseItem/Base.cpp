#include "core/baseItem/base.h"
#include <locale>
#include <cstring>
#include <utf8cpp/utf8.h>


using namespace core;

ScreenInfo core::screenInfo;
namespace core {
    namespace color {
        Color white(255, 255, 255, 255);
        Color black(0, 0, 0, 255);
        Color red(255, 0, 0, 255);
        Color green(0, 255, 0, 255);
        Color blue(0, 0, 255, 255);
    }
}

std::string core::wstring2string(const std::wstring& wstr) {
    std::string str;
    utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(str));
    return str;
}

std::wstring core::string2wstring(const std::string& str) {
    std::wstring wstr;
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(wstr));
    return wstr;
}