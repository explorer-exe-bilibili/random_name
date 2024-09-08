#include "MSGback.h"
#include"init.h"
#include<Windows.h>
#include"set-json.h"
#include"mywindows.h"
#include "FirstScreen.h"
#include "ui.h"
#include "NameScreen.h"
#include "click.h"
#include "config.h"
#include "Timer.h"
#include "directshow.h"
#include"floatwindow.h"
#include"Gp.h"
#include "HistoryScreen.h"

Gp* Pptr = nullptr;
Timer* MSGback::refreash_Timer=nullptr;


void MSGback::create()
{
	ui::SS->reinit();
	HDC hdc = GetDC(nullptr);
	init::music();
	if (mywindows::float_hWnd != nullptr)
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	SetWindowPos(mywindows::main_hwnd, HWND_TOP, 0, 0, mywindows::WW, mywindows::WH, SWP_NOZORDER | SWP_FRAMECHANGED);
	ReleaseDC(nullptr,hdc);
	ui::HS->setFile(L"./name.txt");
	refreash_Timer = new Timer;
	refreash_Timer->setCallBack([] {InvalidateRect(mywindows::main_hwnd, NULL, FALSE); });
	refreash_Timer->setPool(1);
	refreash_Timer->setDelay(1000 / config::getint(FPS));
	ShowWindow(mywindows::main_hwnd, SW_SHOWNORMAL);//把窗体显示出来
	refreash_Timer->start();
}
bool _____ = 1;


void MSGback::size()
{
	ui::ScreenModeChanged = 1;
	if (Pptr != nullptr) {
		init::resetxy();
		Pptr->SizeChanged();
	}
}


void MSGback::paint()
{
	//static int ms_between=1000/config::getint(FPS);
	//static long last_ms = 0;
	//auto now=std::chrono::system_clock::now();
	//auto now_ = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	//long now_ms= now_.time_since_epoch().count() % 1000;

	//if(ms_between>=now_ms&&now_ms-last_ms<ms_between)
	//{
	//	return;
	//}
	static Gp p(mywindows::main_hwnd);
	Pptr= &p;
	if (!ui::SS->fullscreen&&_____) {
		SetWindowPos(mywindows::main_hwnd, nullptr, 0, 0, mywindows::screenWidth * 0.6, mywindows::screenHeight * 0.6, SWP_NOMOVE | SWP_NOZORDER);
	}
	if(_____==1)
	{
		init::resetxy();

	}
	_____ = 0;
	//init::resetxy();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
	ui::HS->changeGp(&p);
	ui::FS->setGp(&p);
	ui::NS->setGp(&p);
	ui::SS->setGp(&p);
	switch (ui::screenmode)
	{
	case FIRST_SCREEN:ui::FS->paint(); directshow::startbgm(); break;
	case SHOW_NAMES_ING:ui::NS->paint();break;
	case SETTING: ui::SS->paint(); break;
	case HISTORY: ui::HS->paint(); break;
	default:ui::FS->paint(); directshow::startbgm(); break;
	}
	if(ui::screenmode!=SHOW_NAMES_ING)
	{
		ui::ExitB.paint();
	}
	p.Flush();
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
		if (ui::SS->FloatWindow) {
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
			ui::SS->seteditbox(wParam);
	}break;
	}
}
void MSGback::destroy()
{
	refreash_Timer->stop();
	PostQuitMessage(0);
}

void MSGback::active()
{
	switch (ui::screenmode) {
	case FIRST_SCREEN:
		ui::FS->repaint();
		break;
	case SETTING:
		ui::SS->repaint();
		break;
	}
}

void MSGback::close()
{
	if (ui::SS->FloatWindow) {
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
		if (!ui::SS->offmusic)
			directshow::startbgm();
		refreash_Timer->start();
	}
	else // 主窗口隐藏
	{
		ShowWindow(mywindows::float_hWnd, SW_SHOWNOACTIVATE);
		if (!ui::SS->offmusic)
			directshow::stopmusic();
		refreash_Timer->pause();
	}
}
void MSGback::destroyall()
{
	mciSendString(L"close bgm", nullptr, 0, nullptr);
}
