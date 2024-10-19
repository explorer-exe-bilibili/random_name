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
namespace mywindows
{
	extern Log logf, errlogf;
	inline std::mutex logMutex, randomlock;
	inline int screenHeight=GetSystemMetrics(SM_CYSCREEN);
	inline int screenWidth=GetSystemMetrics(SM_CXSCREEN);
	inline int WW=screenWidth;//WW
	inline int WH=screenHeight;//WH
	inline int windowTop=0;
	inline int windowLeft=0;
	inline int indices[10]={};
	inline HWND main_hwnd=0,load_hwnd=0;
	inline HWND float_hWnd=0,Quit_hwnd=0;
	void removeFileNameFromPath(char* path);
	void removeFileNameFromPath(wchar_t* path);
	void removeFileNameFromPath(std::wstring& path);
	void log(const char* format, ...);
	void log(const wchar_t* format, ...);
	void errlog(const char* format, ...);
	void errlog(const wchar_t* format, ...);
	int* find(int* array, int size, int valueToFind, int* count);
	bool CreatedMultipleDirectory(const std::string& direct);
	int GetStringWidth(HDC hdc, const std::wstring& str, int height);
	inline HINSTANCE hinstance=nullptr;
	void reboot();
	inline bool debug=false;
};

