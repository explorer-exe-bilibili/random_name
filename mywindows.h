#pragma once
#include <mmsystem.h>
#include<time.h>
#include <wchar.h>
#include <stdio.h>
#include <cstdarg>
#include <filesystem>
#include"bitmaps.h"

#pragma comment(lib, "winmm.lib") // 链接到 Winmm.lib

#define TextOut_(hdc,x,y,string) TextOutW(hdc,x,y,string,wcslen(string)) 
#define Itoa(number,goal) _itoa(number,(char*)goal,10)
#define OR ||
#define AND &&
#define LOGPATH ".\\files\\log\\main.log"
#define ERR_LOGPATH ".\\files\\log\\err.log"
#define NAMESED L".\\history.txt"

std::mutex logMutex,randomlock;
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int windowWidth = screenWidth;
int windowHeight = screenHeight;
int windowTop, windowLeft;
int indices[10];
bool fullscreen;
HWND hWnd;
FILE *logfile_main,*logfile_err,*namesedfile;

HWND CreateButton(const char* classname, const char* title, int x, int y, int width, int height, HWND hWnd, int numberofbutton, const char* path,bool mode);
LPCWSTR UTF8To16(const char* utf8String);
void PrintfDebugString(const wchar_t* format, ...);
void IntMessageBox(int intValue);
void log(const char* logstring,...);
void errlog(const char* format, ...);
void initlogs();
HWND CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const char* words);


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
void log(const char* format,...) {
    std::lock_guard<std::mutex> guard(logMutex); // 锁定互斥锁
    if (format == NULL)errlog("meet a void string");
    if (format == NULL)return;
    va_list args;
    va_start(args, format);
    int length = vsnprintf(0,0,format, args) + 1;
    char* buffer = new char[length];
    if (buffer == NULL) {
        // 如果内存分配失败，输出错误信息
        errlog("memory error(log)");
        va_end(args);
        return;
    }
    vsnprintf_s(buffer, length, _TRUNCATE, format, args);
    fopen_s(&logfile_main,LOGPATH, "a");
    time_t now = time(NULL);
    struct tm* localTime = localtime(&now);
    wchar_t timeStr[32];
    swprintf(timeStr, sizeof(timeStr) / sizeof(wchar_t), L"%04d-%02d-%02d %02d:%02d:%02d",
        localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
        localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    fwprintf(logfile_main,L"[%s]",timeStr);
    fprintf(logfile_main,"[INFO]%s\n", buffer);
    fclose(logfile_main);
    delete[] buffer;
    va_end(args);
}
void errlog(const char* format, ...) {
    std::lock_guard<std::mutex> guard(logMutex); // 锁定互斥锁
    if (format == NULL)errlog("meet a void string");
    if (format == NULL)return;
    va_list args;
    va_start(args, format);
    int length = _vscprintf(format, args) + 1;
    char* buffer = new char[length];
    vsnprintf_s(buffer, length, _TRUNCATE, format, args);
    logfile_main = fopen(LOGPATH, "a");
    logfile_err = fopen(ERR_LOGPATH, "a");
    time_t now = time(NULL);
    struct tm* localTime = localtime(&now);
    wchar_t timeStr[32];
    swprintf(timeStr, sizeof(timeStr) / sizeof(wchar_t), L"%04d-%02d-%02d %02d:%02d:%02d",
        localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
        localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    fwprintf(logfile_main, L"[%s]", timeStr);
    fprintf(logfile_main, "[ERROR]%s\n", buffer);
    fclose(logfile_main);   
    fwprintf(logfile_err, L"[%s]", timeStr);
    fprintf(logfile_err, "[ERROR]%s\n", buffer);
    fclose(logfile_err);
    delete[] buffer;
    va_end(args);
}
//void randomed(LPCWSTR name, int star) {
//    std::lock_guard<std::mutex> guard(randomlock); // 锁定互斥锁
//    namesedfile = _wfopen(NAMESED, L"a");
//    time_t now = time(NULL);
//    struct tm* localTime = localtime(&now);
//    wchar_t timeStr[32];
//    swprintf(timeStr, sizeof(timeStr) / sizeof(wchar_t), L"%04d-%02d-%02d %02d:%02d:%02d",
//        localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
//        localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
//    fwprintf(namesedfile, L"[%s]", timeStr);
//    fwprintf(namesedfile,L"[%s]", name);
//    fwprintf(namesedfile, L"[%d]", star);
//    fwprintf(namesedfile, L"\n");
//    fclose(namesedfile);
//    Sleep(10);
//}
void initlogs() {
    namespace fs = std::filesystem;
    // 指定log文件夹的路径
    std::string log_folder_path = ".\\files\\log";
    // 检查文件夹是否存在
    if (!fs::exists(log_folder_path)) {
        // 文件夹不存在，尝试创建
        try {
            fs::create_directory(log_folder_path);
        }
        catch (const fs::filesystem_error& e) {
            errlog("Error creating directory: %s", e.what());
        }
    }
    logfile_main = fopen(LOGPATH, "w");
    logfile_err = fopen(ERR_LOGPATH, "w");
    namesedfile = _wfopen(NAMESED, L"a");
    if (logfile_err == NULL) {
        MessageBox(NULL, L"创建日志文件失败，尝试检查文件权限，不要放在c盘根目录下", L"错误", MB_ICONERROR);
        PostQuitMessage(0);
    } 
    if (logfile_main == NULL) {
        MessageBox(NULL, L"创建日志文件失败，尝试检查文件权限，不要放在c盘根目录下", L"错误", MB_ICONERROR);
        PostQuitMessage(0);
    }
    if (namesedfile == NULL) {
        MessageBox(NULL, L"创建抽卡记录文件失败，尝试检查文件权限，不要放在c盘根目录下", L"错误", MB_ICONERROR);
        PostQuitMessage(0);
    }
    fclose(logfile_err);
    fclose(logfile_main); 
    fclose(namesedfile);

    log("logs init successfully");
}
// 函数用于查找数组中特定值的所有位置
int* find(int* array, int size, int valueToFind, int* count) {
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

void errend() {
    MessageBox(NULL, L"遇到错误，请查看log或提交issue", L"错误", MB_ICONERROR);
    PostQuitMessage(-1);
}


HWND CreateEditBox(HWND hWndParent,int NUMBER,int x,int y,int w,int h,const char *words) {
    // 创建EDIT控件的样式
    DWORD editStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL;
    // 创建EDIT控件的附加样式（可选）
    DWORD editExStyle = WS_EX_CLIENTEDGE;

    // 创建文本框
    HWND hEdit = CreateWindowExA(editExStyle, "EDIT", words, editStyle, x, y, w, h, hWndParent, (HMENU)NUMBER, NULL, NULL);

    // 返回文本框句柄
    return hEdit;
}
