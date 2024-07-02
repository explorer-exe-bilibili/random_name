#include "MSGback.h"
#include"init.h"
#include<Windows.h>
#include"set-json.h"
#include"mywindows.h"
#include "FirstScreen.h"
#include "ui.h"
#include "NameScreen.h"
#include "click.h"
#include "directshow.h"
#include"floatwindow.h"
#include"Gp.h"
#include "HistoryScreen.h"


void MSGback::create()
{
	ui::SS.reinit();
	HDC hdc = GetDC(nullptr);
	init::music();
	if (mywindows::float_hWnd != nullptr)
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	SetWindowPos(mywindows::main_hwnd, HWND_TOP, 0, 0, mywindows::WW, mywindows::WH, SWP_NOZORDER | SWP_FRAMECHANGED);
	ReleaseDC(nullptr,hdc);
	ui::HS.setFile(L"./name.txt");
}
bool _____ = 1;
void MSGback::paint()
{
	static Gp p(mywindows::main_hwnd);
	if (!ui::SS.fullscreen&&_____) {
		SetWindowPos(mywindows::main_hwnd, nullptr, 0, 0, mywindows::screenWidth * 0.6, mywindows::screenHeight * 0.6, SWP_NOMOVE | SWP_NOZORDER);
	}
	_____ = 0;
	init::resetxy();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
	ui::HS.changeGp(&p);
	ui::FS.setGp(&p);
	ui::NS.changeGp(&p);
	ui::SS.setGp(&p);
	switch (ui::screenmode)
	{
	case FIRST_SCREEN:ui::FS.paint(); directshow::startbgm(); break;
	case SHOW_NAMES_ING:ui::NS.paint();break;
	case SETTING: ui::SS.paint(); break;
	case HISTORY: ui::HS.paint(); break;
	default:ui::FS.paint(); directshow::startbgm(); break;
	}
	if(ui::screenmode!=SHOW_NAMES_ING)
	{
		ui::ExitB.paint();
	}
	mywindows::log("paint successfully");
	DeleteDC(hdc);
	EndPaint(mywindows::main_hwnd, &ps);
}

void MSGback::click(const LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	click::doclick(x, y);
}

void MSGback::keyDown(const WPARAM wParam)
{
	switch (wParam) {
	case VK_ESCAPE:
		if (ui::SS.FloatWindow) {
			ShowWindow(mywindows::main_hwnd, SW_HIDE);
			floatwindow::open();
		}
		else DestroyWindow(mywindows::main_hwnd); break;
	}
}

void MSGback::commond(const LPARAM lParam, const WPARAM wParam)
{
	switch (HIWORD(wParam)) {
	case EN_CHANGE: {
		if (ui::screenmode == SETTING)
			ui::SS.seteditbox(lParam, wParam);
	}break;
	}
}
void MSGback::destroy()
{
	PostQuitMessage(0);
}

void MSGback::active()
{
	switch (ui::screenmode) {
	case FIRST_SCREEN:
		ui::FS.repaint();
		break;
	case SETTING:
		ui::SS.repaint();
		break;
	}
}

void MSGback::close()
{
	if (ui::SS.FloatWindow) {
		ShowWindow(mywindows::main_hwnd, SW_HIDE);
		floatwindow::open();
	}
	else DestroyWindow(mywindows::main_hwnd);
}
void MSGback::showwindow(const WPARAM wParam)
{
	if (wParam) // 主窗口显示
	{
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
		if (!ui::SS.offmusic)
			directshow::startbgm();

	}
	else // 主窗口隐藏
	{
		ShowWindow(mywindows::float_hWnd, SW_SHOWNOACTIVATE);
		if (!ui::SS.offmusic)
			directshow::stopmusic();
	}
}
void MSGback::destroyall()
{
	mciSendString(L"close bgm", nullptr, 0, nullptr);
}
