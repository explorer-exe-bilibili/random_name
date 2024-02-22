#pragma once
#include<Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib") // 链接到 Winmm.lib

#define Itoa(number,goal) _itoa(number,(char*)goal,10)
#define OR ||
#define AND &&

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
HWND hWnd;

HWND CreateButton(const char* classname, const char* title, int x, int y, int width, int height, HWND hWnd, int numberofbutton, const char* path,bool mode);
LPCWSTR UTF8To16(const char* utf8String);
void PrintfDebugString(const wchar_t* format, ...);
void IntMessageBox(int intValue);

void IntMessageBox(int intValue) {
    char tmp[3];
    Itoa(intValue,tmp);
    MessageBox(NULL, UTF8To16(tmp), L"提示", MB_OK);
}

HWND CreateButton(const char* classname,const char* title,int x,int y,int width,int height,HWND hWnd,int numberofbutton, const char* path,bool mode){
    HWND hwndbutton = CreateWindow(UTF8To16(classname), UTF8To16(title), WS_VISIBLE | WS_CHILD | BS_BITMAP | BS_PUSHBUTTON, x, y, width, height, hWnd, (HMENU)numberofbutton, GetModuleHandle(NULL), NULL);
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL,UTF8To16(path), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    SendMessage(hwndbutton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    return hwndbutton;
}
LPCWSTR UTF8To16(const char* utf8String)
{
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, nullptr, 0);
    wchar_t* wideString = new wchar_t[bufferSize];

    MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, wideString, bufferSize);
    
    return wideString;
}
void PrintfDebugString(const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);

    int length = _vscwprintf(format, args) + 1; // 获取格式化字符串的长度
    wchar_t* buffer = new wchar_t[length];

    vswprintf_s(buffer, length, format, args);

    OutputDebugStringW(buffer);

    delete[] buffer;

    va_end(args);
}