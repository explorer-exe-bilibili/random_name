#include "MSGback.h"
#include"init.h"
#include"mywindows.h"
#include "ui.h"
#include "click.h"
#include "ConfigItem.h"
#include"floatwindow.h"
#include"Gp.h"
#include "LoadWindow.h"

Gp* MSGback::Pptr = nullptr;
LARGE_INTEGER MSGback::frequency;
LARGE_INTEGER MSGback::lastTime;
int MSGback::frameCount = 0;
float MSGback::fps = 0.0f;

void MSGback::create()
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);
	ui::SS->reinit();
	HDC hdc = GetDC(nullptr);
	init::music();
	if (mywindows::float_hWnd != nullptr)
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	SetWindowPos(mywindows::main_hwnd, HWND_TOP, 0, 0, mywindows::WW, mywindows::WH, SWP_NOZORDER | SWP_FRAMECHANGED);
	ShowWindow(mywindows::main_hwnd, SW_HIDE);
	ReleaseDC(nullptr,hdc);
	ui::HS->setFile(L"./name.txt");
	Pptr=new Gp(mywindows::main_hwnd);
	init::resetxy();
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
	static bool debug=config::getint(DEBUG);
	extern LoadWindow* loadWindow;
	if(loadWindow)
	{
		loadWindow->destroy();
		loadWindow = nullptr;
	}
	if (!ui::SS->fullscreen&&_____) {
		SetWindowPos(mywindows::main_hwnd, nullptr, 0, 0, mywindows::screenWidth * 0.6, mywindows::screenHeight * 0.6, SWP_NOMOVE | SWP_NOZORDER);
	}
	if(_____==1)
	{
		init::resetxy();
	}
	_____ = 0;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
	ui::HS->changeGp(Pptr);
	ui::FS->setGp(Pptr);
	ui::NS->setGp(Pptr);
	ui::SS->setGp(Pptr);
	switch (ui::screenmode)
	{
	case FIRST_SCREEN:ui::FS->paint(); break;
	case SHOW_NAMES_ING:ui::NS->paint();break;
	case SETTING: ui::SS->paint(); break;
	case HISTORY: ui::HS->paint(); break;
	default:ui::FS->paint(); break;
	}
	if(ui::screenmode!=SHOW_NAMES_ING)
	{
		ui::ExitB.paint();
	}
	updateFrameRate();
	if (debug) {
		Pptr->DrawString(("FPS:" + std::to_string(fps)), ui::text, 0, 0, 0, 0, 0);
	}
	Pptr->Flush();
	mywindows::log("paint successfully");
	DeleteDC(hdc);
	EndPaint(mywindows::main_hwnd, &ps);
	if(Button::needFresh)
	{
		InvalidateRect(mywindows::main_hwnd, nullptr, 0);
		Button::needFresh = 0;
	}
}

void MSGback::click(const LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	click::doclick(x, y);
	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
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
			mciSendString(L"play bgm repeat from 0", 0, 0, 0);
	}
	else // 主窗口隐藏
	{
		ShowWindow(mywindows::float_hWnd, SW_SHOWNOACTIVATE);
		if (!ui::SS->offmusic)
			explorer::getInstance()->stopmusic();
	}
}

void MSGback::updateFrameRate()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	frameCount++;
	if (currentTime.QuadPart - lastTime.QuadPart >= frequency.QuadPart)
	{
		fps = frameCount / ((currentTime.QuadPart - lastTime.QuadPart) / (float)frequency.QuadPart);
		frameCount = 0;
		lastTime = currentTime;
	}
}

void MSGback::destroyall()
{
	mciSendString(L"close bgm", nullptr, 0, nullptr);
}
