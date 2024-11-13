#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <string>
class task
{
public:
    static bool processExists(const std::wstring& processName);
};