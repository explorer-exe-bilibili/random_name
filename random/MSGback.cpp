#include "MSGback.h"
#include"init.h"
#include<Windows.h>
#include"set-json.h"
#include"mywindows.h"
#include "paintfirstscreen.h"
#include "ui.h"
#include "paintname.h"
#include "click.h"
#include "directshow.h"
#include"floatwindow.h"
#include"Gp.h"

extern set2 setscreen;

void MSGback::create()
{
	setscreen.reinit();
	HDC hdc = GetDC(NULL);
	init::music();
	if (mywindows::float_hWnd != 0)
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	SetWindowPos(mywindows::main_hwnd, HWND_TOP, 0, 0, mywindows::windowWidth, mywindows::windowHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
	ReleaseDC(0,hdc);
}
bool _____ = 1;
void MSGback::paint()
{
	static Gp p(mywindows::main_hwnd);
	if (!setscreen.fullscreen&&_____) {
		SetWindowPos(mywindows::main_hwnd, NULL, 0, 0, mywindows::screenWidth * 0.6, mywindows::screenHeight * 0.6, SWP_NOMOVE | SWP_NOZORDER);
	}
	_____ = 0;
	if (mywindows::load_hwnd != 0) {
		DestroyWindow(mywindows::load_hwnd);
		mywindows::load_hwnd = 0;
	}
	init::resetxy();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);
	SetStretchBltMode(hdc, HALFTONE);
	switch (ui::screenmode)
	{
	case FIRST_MENU:paintfirstscreen::printfirstmenu(&p); directshow::startbgm(); break;
	case SHOW_NAMES_ING:paintname::paint(p);break;
	case SETTING: setscreen.paint(&p); break;
	}
	mywindows::log("paint successfully");
	DeleteDC(hdcMem);
	DeleteDC(hdc);
	EndPaint(mywindows::main_hwnd, &ps);
}

void MSGback::click(LPARAM lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	click::doclick(x, y);
}

void MSGback::keyDown(WPARAM wParam)
{
	switch (wParam) {
	case VK_ESCAPE:
		if (setscreen.FloatWindow) {
			ShowWindow(mywindows::main_hwnd, SW_HIDE);
			floatwindow::open();
		}
		else DestroyWindow(mywindows::main_hwnd); break;
	}
}

void MSGback::commond(LPARAM lParam, WPARAM wParam)
{
	switch (HIWORD(wParam)) {
	case EN_CHANGE: {
		if (ui::screenmode == SETTING)
			setscreen.seteditbox(lParam, wParam);
	}break;
	}
}
void MSGback::destroy()
{
	destroyall();
	PostQuitMessage(0);
}

void MSGback::active()
{
	switch (ui::screenmode) {
	case FIRST_MENU:
		paintfirstscreen::repaint();
		break;
	case SETTING:
		setscreen.repaint();
		break;
	}
}

void MSGback::close()
{
	if (setscreen.FloatWindow) {
		ShowWindow(mywindows::main_hwnd, SW_HIDE);
		floatwindow::open();
	}
	else DestroyWindow(mywindows::main_hwnd);
}
void MSGback::showwindow(WPARAM wParam)
{
	if (wParam) // 主窗口显示
	{
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
		if (!setscreen.offmusic)
			directshow::startbgm();

	}
	else // 主窗口隐藏
	{
		ShowWindow(mywindows::float_hWnd, SW_SHOWNOACTIVATE);
		if (!setscreen.offmusic)
			directshow::stopmusic();
	}
}
void MSGback::destroyall()
{
	setscreen.release();
	mciSendString(L"close bgm", 0, 0, 0);
}
