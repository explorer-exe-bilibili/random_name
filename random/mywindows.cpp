#include "mywindows.h"
#include <mmsystem.h>
#include<time.h>
#include <cstdarg>
#include <filesystem>
#include<commctrl.h>
#include"sth2sth.h"

#pragma comment(lib, "winmm.lib") // 链接到 Winmm.lib

std::mutex mywindows::logMutex, mywindows::randomlock;
int mywindows::screenHeight = GetSystemMetrics(SM_CYSCREEN);
int mywindows::screenWidth = GetSystemMetrics(SM_CXSCREEN);
int mywindows::mywindows::windowWidth = screenWidth;
int mywindows::mywindows::windowHeight = screenHeight;
int mywindows::windowTop;
int mywindows::windowLeft;
int mywindows::indices[10];
bool mywindows::fullscreen, mywindows::offmusic = 0;
HWND mywindows::hWnd;
FILE* mywindows::logfile_main, * mywindows::logfile_err;
Log infolog(LOGPATH, 0), errlogf(ERR_LOGPATH, 0);


void mywindows::removeFileNameFromPath(char* path) {
    // 获取字符串长度
    size_t len = strlen(path);
    // 找到最后一个'\'的位置
    char* lastSlash = strrchr(path, '\\');
    // 如果找到了最后一个'\'，并且它后面还有字符
    if (lastSlash != NULL && lastSlash < path + len - 1) {
        // 将最后一个'\'后面的所有字符（包括它自己）替换为字符串结束符'\0'
        *lastSlash = '\0';
    }
}

void mywindows::IntMessageBox(int intValue) {
    char tmp[3];
    Itoa(intValue, tmp);
    MessageBox(NULL, sth2sth::UTF8To16(tmp), L"提示", MB_OK);
}
HWND mywindows::CreateButton(const char* classname, const char* title, int x, int y, int width, int height, HWND hWnd, int numberofbutton, const char* path, bool mode) {
    HWND hwndbutton = CreateWindow(sth2sth::UTF8To16(classname), sth2sth::UTF8To16(title), WS_VISIBLE | WS_CHILD | BS_BITMAP | BS_PUSHBUTTON, x, y, width, height, hWnd, (HMENU)numberofbutton, GetModuleHandle(NULL), NULL);
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, sth2sth::UTF8To16(path), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    SendMessage(hwndbutton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    return hwndbutton;
}
void mywindows::PrintfDebugString(const wchar_t* format, ...)
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
void mywindows::log(const char* format, ...) {
    if (format == NULL)errlog("meet a void string");
    va_list args;
    va_start(args, format);
    int length = vsnprintf(0, 0, format, args) + 1;
    char* buffer = new char[length];
    if (buffer == NULL) {
        // 如果内存分配失败，输出错误信息
        errlog("memory error(log)");
        va_end(args);
        return;
    }
    vsnprintf_s(buffer, length, _TRUNCATE, format, args);
    infolog << infolog.pt() << "[INFO]" << buffer << infolog.nl();
    delete[] buffer;
    va_end(args);
}
void mywindows::errlog(const char* format, ...) {
    if (format == NULL)errlog("meet a void string");
    if (format == NULL)return;
    va_list args;
    va_start(args, format);
    int length = _vscprintf(format, args) + 1;
    char* buffer = new char[length];
    vsnprintf_s(buffer, length, _TRUNCATE, format, args);
    infolog << infolog.pt() << "[ERROR]" << buffer << infolog.nl();
    errlogf << errlogf.pt() << "[ERROR]" << buffer << errlogf.nl();
    delete[] buffer;
    va_end(args);
}

// 函数用于查找数组中特定值的所有位置
int* mywindows::find(int* array, int size, int valueToFind, int* count) {
    // 分配一个动态数组来存储找到的索引
    if (indices == NULL) {
        // 如果内存分配失败，返回 NULL
        return NULL;
    }
    int indexCount = 0; // 用于记录找到的索引数量

    // 遍历数组，检查每个元素是否与 valueToFind 相等
    for (int i = 0; i < size; ++i) {
        if (array[i] == valueToFind) {
            indices[indexCount++] = i; // 如果找到匹配的值，将其索引添加到数组中
        }
    }

    // 更新 count 参数，表示找到的索引数量
    *count = indexCount;

    // 如果没有找到任何匹配的值，释放分配的内存并返回 NULL
    if (indexCount == 0) {
        return NULL;
    }

    return indices; // 返回包含所有匹配索引的数组
}

void mywindows::errend() {
    MessageBox(NULL, L"遇到错误，请查看log或提交issue", L"错误", MB_ICONERROR);
    PostQuitMessage(-1);
}


HWND mywindows::CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const char* words) {
    // 创建EDIT控件的样式
    DWORD editStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL;
    // 创建EDIT控件的附加样式（可选）
    DWORD editExStyle = WS_EX_CLIENTEDGE;

    // 创建文本框
    HWND hEdit = CreateWindowExA(editExStyle, "EDIT", words, editStyle, x, y, w, h, hWndParent, (HMENU)NUMBER, NULL, NULL);

    // 返回文本框句柄
    return hEdit;
}
