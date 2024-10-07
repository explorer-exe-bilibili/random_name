#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <mutex>
#include "log.h"
#include<Windows.h>

#define TextOut_(hdc,x,y,string) TextOutW(hdc,static_cast<int>(x),static_cast<int>(y),string,static_cast<int>(wcslen(string)))
#define Itoa(number,goal) _itoa(number,(char*)goal,10)
#define OR ||
#define AND &&
#define LOGPATH ".\\files\\log\\main.log"
#define ERR_LOGPATH ".\\files\\log\\err.log"
#define （ (
#define ） )
class mywindows
{
public:
	static Log logf, errlogf;
	static std::mutex logMutex, randomlock;
	static int screenHeight;
	static int screenWidth;
	static int WW;//WW
	static int WH;//WH
	static int windowTop;
	static int windowLeft;
	static int indices[10];
	static HWND main_hwnd,load_hwnd;
	static HWND float_hWnd,Quit_hwnd;
	static void removeFileNameFromPath(char* path);
	static void removeFileNameFromPath(wchar_t* path);
	static void removeFileNameFromPath(std::wstring& path);
	static void log(const char* format, ...);
	static void log(const wchar_t* format, ...);
	static void errlog(const char* format, ...);
	static void errlog(const wchar_t* format, ...);
	static int* find(int* array, int size, int valueToFind, int* count);
	static bool CreatedMultipleDirectory(const std::string& direct);
	static int GetStringWidth(HDC hdc, const std::wstring& str, int height);
	static HINSTANCE hinstance;
	static void reboot();
	static bool debug;
};

