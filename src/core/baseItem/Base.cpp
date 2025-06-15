#include "core/baseItem/Base.h"
#include <locale>
#include <cstring>
#include "core/log.h"
#include <utf8cpp/utf8.h>


using namespace core;

ScreenInfo core::WindowInfo,core::screenInfo;
// 初始化静态成员变量
namespace core {
    namespace color {
        Color white(255, 255, 255, 255);
        Color black(0, 0, 0, 255);
        Color red(255, 0, 0, 255);
        Color green(0, 255, 0, 255);
        Color blue(0, 0, 255, 255);
    }
}

float core::Region::getyend() const { 
    if(yend<=0)
        return (gety()-getx()+getxend());
    else 
        return screenRatio ? yend * WindowInfo.height : yend; 
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

size_t core::utf8_length(const std::string& str) {
    size_t length = 0;
    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = str[i];
        if (c < 0x80) {
            i += 1; // ASCII字符
        } else if ((c >> 5) == 0x6) {
            i += 2; // 2字节UTF-8字符
        } else if ((c >> 4) == 0xe) {
            i += 3; // 3字节UTF-8字符（包括大部分中文）
        } else if ((c >> 3) == 0x1e) {
            i += 4; // 4字节UTF-8字符
        } else {
            i += 1; // 无效字符，跳过
        }
        length++;
    }
    return length;
}

void core::quit() {
    glfwSetWindowShouldClose(WindowInfo.window, true);
}