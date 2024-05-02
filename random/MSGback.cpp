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
extern HBITMAP hbitmaps[BitmapCounts];
extern BITMAP overlay1Bm, bm, ball, overlay2Bm, overlay3Bm, overlay4Bm, cardbg, exitinfo, goldenbg, listbm, liststar, buttom;

void MSGback::create()
{
	setscreen.reinit();
	HDC hdc = GetDC(NULL);
	init::picture();
	init::music();
	if (mywindows::float_hWnd != 0)
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	SetWindowPos(mywindows::main_hwnd, HWND_TOP, 0, 0, mywindows::windowWidth, mywindows::windowHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
	ReleaseDC(0,hdc);
}

void MSGback::paint()
{
	static Gp p;
	if (mywindows::load_hwnd != 0) {
		DestroyWindow(mywindows::load_hwnd);
		mywindows::load_hwnd = 0;
	}
	init::resetxy();
	init::resetpicture();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);
	p.Prepar(hdc);
	SetStretchBltMode(hdc, HALFTONE);
	switch (ui::screenmode)
	{
	case FIRST_MENU:paintfirstscreen::printfirstmenu(hdc, hdcMem,p); directshow::startbgm(); break;
	case SHOW_NAMES_ING:paintname::paint(hdc, hdcMem);break;
	case SETTING: setscreen.paint(hdc, hdcMem); break;
	}
	HDC hdct = CreateCompatibleDC(hdc);
	p.pPaint(hdct, 0, 0, 2);
	BitBlt(hdc, 0, 0, mywindows::windowWidth, mywindows::windowHeight, hdct, 0, 0, SRCCOPY);
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
		PostQuitMessage(0); break;
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
	for (auto& hbitmap : hbitmaps)
	{
		if (hbitmap != NULL)
		{
			DeleteObject(hbitmap);
			hbitmap = NULL; // 将已释放的句柄置为NULL,避免重复释放
			
		}
	}
	mciSendString(L"close bgm", 0, 0, 0);
}
