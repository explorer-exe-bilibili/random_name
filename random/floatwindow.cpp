#include "floatwindow.h"
#include<thread>
#include <chrono>

#include "bitmaps.h"
#include "ConfigItem.h"
#include "Gp.h"
#include "mywindows.h"

bool ShowFloatWindow = true,ShowQuitWindow=true;
WNDPROC floatwindow::g_OrigFloatWndProc = nullptr;
bool floatwindow::is_mouse_dragging;
POINT floatwindow::p, floatwindow::p1;
POINT floatwindow::last_mouse_pos;
bool showing = false,buttonDown=false;
HBITMAP floatwindow::bitmap;
int floatwindow::icon_w, floatwindow::icon_h;
POINT lastxy, nowxy;
UINT_PTR TimerID;
double speedx, speedy;
std::chrono::time_point start = std::chrono::high_resolution_clock::now();

void floatwindow::open()
{
	if (ShowFloatWindow) {
		ShowFloatWindow = false;
		createWindow();
	}
}
void floatwindow::stop()
{
	ShowWindow(mywindows::float_hWnd, SW_HIDE);
}
void floatwindow::paint()
{
	PAINTSTRUCT ps;
	const HDC hdc= BeginPaint(mywindows::float_hWnd, &ps);
		static Gp p(mywindows::float_hWnd);
		RECT rect;
		GetWindowRect(mywindows::float_hWnd, &rect);
		const int w = rect.right - rect.left, h = rect.bottom - rect.top;
		p.Paint(0, 0, w, h, floating);
		p.Flush();
		ReleaseDC(mywindows::float_hWnd, hdc);
	EndPaint(mywindows::float_hWnd, &ps);
}
void floatwindow::createWindow() {
	// 计算悬浮窗口的位置和大小
	const int floatWndWidth = config::getScreen(FLOATW);
	const int floatWndHeight = config::getScreen(FLOATH);
	const int floatWndX = config::getScreen(FLOATX);
	const int floatWndY = config::getScreen(FLOATY);

	// 创建悬浮窗口
	mywindows::float_hWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST| WS_EX_TOOLWINDOW,
		L"FLOAT",L"Float Window", WS_POPUPWINDOW,
		floatWndX, floatWndY, floatWndWidth, floatWndHeight,
		nullptr, nullptr, mywindows::hinstance, nullptr);

	// 设置悬浮窗口的初始透明度
	SetLayeredWindowAttributes(mywindows::float_hWnd, 0, 128, LWA_ALPHA);
	// 显示悬浮窗口
	ShowWindow(mywindows::float_hWnd, SW_SHOWNOACTIVATE);
	UpdateWindow(mywindows::float_hWnd);
	constexpr int x = 0;
	constexpr int y = 0;
	const int w = mywindows::screenWidth;
	const int h = mywindows::screenHeight * 0.3;
	mywindows::Quit_hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
		L"QUIT", L"", WS_VISIBLE | WS_POPUP, x, y, w, h, nullptr, nullptr, mywindows::hinstance, nullptr);
	SetLayeredWindowAttributes(mywindows::Quit_hwnd, 0, 200, LWA_ALPHA);
	ShowWindow(mywindows::Quit_hwnd, SW_HIDE);
}
void floatwindow::showQuitWindow()
{
	if (!showing) {
		showing = true;
		ShowWindow(mywindows::Quit_hwnd, SW_SHOWNOACTIVATE);
		InvalidateRect(mywindows::Quit_hwnd, nullptr, FALSE);
	}
}
void floatwindow::create()
{
}
void floatwindow::hideQuitWindow()
{
	if (showing) {
		ShowWindow(mywindows::Quit_hwnd, SW_HIDE);
		showing = false;
	}
}
LRESULT floatwindow::nchittest(const WPARAM wParam)
{
	if (wParam == HTLEFT)
	{
		SetCapture(mywindows::float_hWnd);
		return HTCAPTION;
	}
	return HTCLIENT;
}

POINT prevPos;

void floatwindow::lbuttondown(const WPARAM wParam)
{
	if (wParam == HTCLIENT)
	{
		showQuitWindow();
		GetCursorPos(&p); //获取鼠标位置
		is_mouse_dragging = true;
		GetCursorPos(&last_mouse_pos);
		start = std::chrono::high_resolution_clock::now();
		SetCapture(mywindows::float_hWnd);
		prevPos = last_mouse_pos;

	}
	GetCursorPos(&lastxy);
	TimerID = 1;
	buttonDown = true;
}
void floatwindow::mousemove(LPARAM lParam)
{
	if (is_mouse_dragging)
	{
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);

		RECT rect;
		GetWindowRect(mywindows::float_hWnd, &rect);
		const int dx = currentMousePos.x - last_mouse_pos.x;
		const int dy = currentMousePos.y - last_mouse_pos.y;

		MoveWindow(mywindows::float_hWnd, rect.left + dx, rect.top + dy, rect.right - rect.left, rect.bottom - rect.top, TRUE);

		last_mouse_pos = currentMousePos;
	}
}
bool uplbutton = false;
void movewindow() {
	static double fps_time = 1000/config::getint(FPS);
	static double speedx_;
	static double speedy_;
	if (uplbutton) {
		speedx_ = speedx*fps_time;
		speedy_ = speedy*fps_time;
		uplbutton = false;
	}while (speedx_ > 1 || speedy_ > 1||speedx_<-1||speedy_<-1) {
		RECT WindowRect;
		GetWindowRect(mywindows::float_hWnd, &WindowRect);
		const int x = WindowRect.left + int(speedx_);
		const int y = WindowRect.top + int(speedy_);
		const int w = WindowRect.right - WindowRect.left;
		const int h = WindowRect.bottom - WindowRect.top;
		MoveWindow(mywindows::float_hWnd, x, y, w, h, 0);
		GetWindowRect(mywindows::float_hWnd, &WindowRect);
		if (WindowRect.left + speedx_ < 0) {
			speedx_ = -speedx_;
			MoveWindow(mywindows::float_hWnd, 0, WindowRect.top, w, h, 0);
		}
		else if (WindowRect.right > mywindows::screenWidth) {
			speedx_ = -speedx_;
			MoveWindow(mywindows::float_hWnd,mywindows::screenWidth-w,WindowRect.top, w, h, 0);
		}
		if (WindowRect.top + speedy_ < 0) {
			speedy_ = -speedy_;
			MoveWindow(mywindows::float_hWnd, WindowRect.left, 0, w, h, 0);
		}
		else if (WindowRect.bottom > mywindows::screenHeight) {
			speedy_ = -speedy_;
			MoveWindow(mywindows::float_hWnd, WindowRect.left, mywindows::screenHeight - h, w, h, 0);
		}
		Sleep(fps_time);
		const double Mu = config::getd(MU);
		speedx_ = speedx_ * Mu;
		speedy_ = speedy_ * Mu;
		if (TimerID) {
			break;
		}
	}
}
void floatwindow::lbuttonup()
{
	buttonDown = false;
	if (is_mouse_dragging)
	{
		hideQuitWindow();
		GetCursorPos(&p1); //获取鼠标位置
		is_mouse_dragging = false;
		ReleaseCapture();
		if (-3 <= (p.x - p1.x)AND(p.x - p1.x) <= 3 AND - 3 <= (p.y - p1.y)AND(p.y - p1.y) <= 3)
		{
			ShowWindow(mywindows::main_hwnd, SW_SHOW);
			ShowWindow(mywindows::float_hWnd, SW_HIDE);
		}
	}
	POINT currentMousePos;
	GetCursorPos(&currentMousePos);
	const std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double, std::milli> elapsed = end - start;
	speedx =(currentMousePos.x - prevPos.x)/elapsed.count();
	speedy = (currentMousePos.y - prevPos.y) / elapsed.count();
	if (currentMousePos.y <= mywindows::screenHeight * 0.3) {
		ShowWindow(mywindows::float_hWnd,SW_HIDE);
		ShowWindow(mywindows::Quit_hwnd,SW_HIDE);
		PostQuitMessage(0);
	}
	KillTimer(nullptr, TimerID);
	TimerID = NULL;
	uplbutton = true;
	std::thread (movewindow).detach();
}

void floatwindow::lbuttonclick(const WPARAM wParam)
{
	if (wParam == HTCLIENT)
	{
		// 双击窗口内部区域时的操作，例如显示/隐藏主窗口
		ShowWindow(mywindows::main_hwnd, SW_SHOW);
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	}
}