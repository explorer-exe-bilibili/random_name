#pragma once
#include<Windows.h>
#define STAR L"E"

class paintname
{
public:
	static void showname1();
	static void showname10();
	static bool is5star, is4star, skipped ;
	static int  skipbmx, skipbmy, skipbmxend, skipbmyend;
	static HANDLE random_handle;
	static HFONT icon_star;
	static void addaname();
	static void out10name(HDC hdc, HDC hdcMem);
	static void out1name(HDC hdc, HDC hdcMem);
	static void rerandom();
	static DWORD WINAPI RandomNumberGenerator();
	static void KillAllTimer();
	static void paint(HDC hdc, HDC hdcMem);
private:
	static UINT_PTR g_StarTimerID; // 定时器ID
	static UINT_PTR g_Case6TimerID[3], g_InitTimerID; // case 6 定时器 ID
	static HWND InitTimerHwnd, Case6TimerHwnd, StarTimerHwnd;
	static bool firsttimeCase6;
	static int number;
	static LPCWSTR names[4][256];
	static wchar_t * tmp;
	static int paintstarcount, g_Case6Count, g_StarCount, star_[10], step;
	static LPCWSTR  temp[10];
	static void menu(HDC hdc, HDC hdcMem);
	static void printstars(HDC hdc, int number);
	static void listpainter(LPCWSTR tmp_, int i, HDC hdc);
	static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static void CALLBACK Case6TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static void CALLBACK InitTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static void printnames(HDC hdc, HDC hdcMem);
};

