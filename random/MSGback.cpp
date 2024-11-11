#include "MSGback.h"

#include "ConfigItem.h"
#include "FirstScreen.h"
#include"floatwindow.h"
#include "HistoryScreen.h"
#include"init.h"
#include "LoadWindow.h"
#include "MenuScreen.h"
#include"mywindows.h"
#include "NameScreen.h"
#include "set-json.h"
#include "ui.h"

Gp* MSGback::Pptr = nullptr;
LARGE_INTEGER MSGback::frequency;
LARGE_INTEGER MSGback::lastTime;
int MSGback::frameCount = 0;
float MSGback::fps = 0.0f;

void MSGback::create()
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);
	set2::reinit();
	const HDC hdc = GetDC(nullptr);
	if (mywindows::float_hWnd != nullptr)
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	SetWindowPos(mywindows::main_hwnd, HWND_TOP, 0, 0, mywindows::WW, mywindows::WH, SWP_NOZORDER | SWP_FRAMECHANGED);
	ShowWindow(mywindows::main_hwnd, SW_HIDE);
	ReleaseDC(nullptr,hdc);
	ui::HS->setFile(L"./name.txt");
	Pptr=new Gp(mywindows::main_hwnd);
	init::resetPoint();
	std::thread([] {while (true) {
		if(ui::ScreenMode!=SETTING)
		InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
	std::this_thread::sleep_for(std::chrono::milliseconds(300)); }}).detach();
}
bool _ = true;


void MSGback::size()
{
	ui::ScreenModeChanged = true;
	if (Pptr != nullptr) {
		init::resetPoint();
		Pptr->SizeChanged();
	}
}


void MSGback::paint()
{
	static bool debug=config::getint(DEBUG);
	if(extern LoadWindow* loadWindow; loadWindow)
	{
		loadWindow->destroy();
		loadWindow = nullptr;
	}
	if (set2::fullscreen&&_) {
		SetWindowPos(mywindows::main_hwnd, nullptr, 0, 0, mywindows::screenWidth * 0.6
			, mywindows::screenHeight * 0.6, SWP_NOMOVE | SWP_NOZORDER);
	}
	if(_)
	{
		init::resetPoint();
	}
	_ = false;
	PAINTSTRUCT ps;
	const HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
	ui::HS->setGp(Pptr);
	ui::FS->setGp(Pptr);
	ui::NS->setGp(Pptr);
	ui::SS->setGp(Pptr);
	ui::MS->setGp(Pptr);
	switch (ui::ScreenMode)
	{
	case FIRST_SCREEN:ui::FS->paint(); break;
	case namescreen:ui::NS->paint();break;
	case SETTING: ui::SS->paint(); break;
	case HISTORY: ui::HS->paint(); break;
	case MENU:ui::MS->paint(); break;
	default: break;
	}
	updateFrameRate();
	if (debug) {
		Pptr->DrawString(("FPS:" + std::to_string(fps)), ui::text, 0, 0, ARGB(255 ,0, 0, 0));
	}
	Pptr->Flush();
	mywindows::log("paint successfully");
	DeleteDC(hdc);
	EndPaint(mywindows::main_hwnd, &ps);
	if(Button::needFresh)
	{
		InvalidateRect(mywindows::main_hwnd, nullptr, 0);
		Button::needFresh = false;
	}
}

void MSGback::click(const LPARAM lParam)
{
	const int x = GET_X_LPARAM(lParam);
	const int y = GET_Y_LPARAM(lParam);
	switch (ui::ScreenMode)
	{
	case FIRST_SCREEN: ui::FS->click(x, y);break;
	case namescreen: ui::NS->click(x, y);break;
	case SETTING: ui::SS->clicked(x, y);break;
	case HISTORY: ui::HS->click(x, y); break;
	case MENU: ui::MS->click(x, y); break;
	default: break;
	}
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}

void MSGback::keyDown(const WPARAM wParam)
{
	switch (wParam) {
	case VK_ESCAPE:
		if (set2::FloatWindow) {
			ShowWindow(mywindows::main_hwnd, SW_HIDE);
			floatwindow::open();
		}
		else DestroyWindow(mywindows::main_hwnd); break;
	default:break;
	}
}

void MSGback::command(const LPARAM lParam, const WPARAM wParam)
{
	switch (HIWORD(wParam)) {
	case EN_CHANGE: {
		if (ui::ScreenMode == SETTING)
			ui::SS->setEditBox(wParam);
	}break;
	default:break;
	}
}
void MSGback::destroy()
{
	PostQuitMessage(0);
}

void MSGback::active()
{
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}

void MSGback::close()
{
	if (set2::FloatWindow) {
		ShowWindow(mywindows::main_hwnd, SW_HIDE);
		floatwindow::open();
	}
	else DestroyWindow(mywindows::main_hwnd);
}
void MSGback::showWindow(const WPARAM wParam)
{
	if (wParam) // 主窗口显示
	{
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
		if (!set2::offMusic)
			mciSendString(L"play bgm repeat from 0", nullptr, 0, nullptr);
	}
	else // 主窗口隐藏
	{
		ShowWindow(mywindows::float_hWnd, SW_SHOWNOACTIVATE);
		if (!set2::offMusic)
			explorer::stopMusic();
	}
}

void MSGback::updateFrameRate()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	frameCount++;
	if (currentTime.QuadPart - lastTime.QuadPart >= frequency.QuadPart)
	{
		fps = float(frameCount) / ((currentTime.QuadPart - lastTime.QuadPart) / float(frequency.QuadPart));
		frameCount = 0;
		lastTime = currentTime;
	}
}