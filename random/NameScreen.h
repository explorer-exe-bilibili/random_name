#pragma once
#include<Windows.h>

#include "Button.h"
#include"Gp.h"
#define STAR L"E"

class NameScreen
{
public:
	NameScreen(Gp* p);
	NameScreen();
	~NameScreen();
	void showname1();
	void showname10();
	bool is5star=0, is4star=0, skipped=0;
	int  skipbmx=0, skipbmy=0, skipbmxend=0, skipbmyend=0;
	HANDLE random_handle=nullptr;
	HFONT icon_star=nullptr;
	void addaname() const;
	void rerandom() = delete;
	void KillAllTimer();
	void paint();
	void resetPoint();
	void setGp(Gp* p);
	void click(int x, int y);
private:
	Gp *p=nullptr;
	UINT_PTR g_StarTimerID=0; // 定时器ID
	UINT_PTR g_Case6TimerID[3] = {0}, g_InitTimerID=0; // case 6 定时器 ID
	HWND InitTimerHwnd=nullptr, Case6TimerHwnd=nullptr, StarTimerHwnd=nullptr;
	bool firsttimeCase6=1;
	int number=0;
	wchar_t * tmp=nullptr;
	int paintstarcount = 0, g_Case6Count = 500, g_StarCount = 0, star_[10]{}, step=0;
	LPCWSTR  temp[10]{};
	int type[10]{};
	void menu();
	void printnames();
	void printstars(int star_number);
	void listpainter(LPCWSTR tmp_, int i, HDC hdc);
	void out10name();
	void out1name();
	void skip();
	void setButton();
	Button B[2];
public:
	void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	void CALLBACK Case6TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	void CALLBACK InitTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
};