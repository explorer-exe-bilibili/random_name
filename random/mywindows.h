#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <mutex>
#include "log.h"
#include <wchar.h>
#include <stdio.h>
#include<Windows.h>

#define TextOut_(hdc,x,y,string) TextOutW(hdc,x,y,string,wcslen(string)) 
#define Itoa(number,goal) _itoa(number,(char*)goal,10)
#define OR ||
#define AND &&
#define LOGPATH ".\\files\\log\\main.log"
#define ERR_LOGPATH ".\\files\\log\\err.log"
class mywindows
{
public:
	static std::mutex logMutex, randomlock;
	static int screenHeight;
	static int screenWidth;
	static int windowWidth;
	static int windowHeight;
	static int windowTop;
	static int windowLeft;
	static int indices[10];
	static bool fullscreen, offmusic;
	static HWND hWnd;
	static FILE* logfile_main, * logfile_err;
	static void removeFileNameFromPath(char* path);
	static void IntMessageBox(int intValue);
	static HWND CreateButton(const char* classname, const char* title, int x, int y, int width, int height, HWND hWnd, int numberofbutton, const char* path, bool mode);
	static void PrintfDebugString(const wchar_t* format, ...);
	static void log(const char* format, ...);
	static void errlog(const char* format, ...);
	static int* find(int* array, int size, int valueToFind, int* count);
	static void errend();
	static HWND CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const char* words);
};

