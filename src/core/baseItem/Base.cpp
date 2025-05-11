#include "core/baseItem/Base.h"
#include <locale>
#include <cstring>
#include <utf8cpp/utf8.h>
#pragma execution_character_set("utf-8")

using namespace core;

ScreenInfo core::screenInfo;

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

Character& Character::operator=(const Character& character)
{
    if (this != &character)
    {
        texture = character.texture;
        size = character.size;
        bearing = character.bearing;
        advance = character.advance;
    }
    return *this;
}