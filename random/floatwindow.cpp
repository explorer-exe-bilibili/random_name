#include "floatwindow.h"
#include<Windows.h>
#include "mywindows.h"
#include "resource.h"
#include "set-json.h"
#include "config.h"
#include<thread>
#include <dshow.h>
#include <chrono>

extern set2 setscreen;
bool showfloatwindow = 1,showQuitwindow=1;
WNDPROC floatwindow::g_OrigFloatWndProc = NULL;
bool floatwindow::is_mouse_dragging;
POINT floatwindow::p, floatwindow::p1;
POINT floatwindow::last_mouse_pos;
bool showing = 0,buttomdown=0;
HBITMAP floatwindow::bitmap;
int floatwindow::icon_w, floatwindow::icon_h;
POINT lastxy, nowxy;
UINT_PTR TimerID;
double speedx, speedy;
std::chrono::time_point start = std::chrono::high_resolution_clock::now();

void floatwindow::open()
{
	if (showfloatwindow) {
		showfloatwindow = 0;
		createWindow();
	}

}
void floatwindow::stop()
{
	ShowWindow(mywindows::float_hWnd, SW_HIDE);
}
void floatwindow::paint()
{
	static Gp p(mywindows::float_hWnd);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::float_hWnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);
	RECT rect;
	GetWindowRect(mywindows::float_hWnd, &rect);
	int w = rect.right - rect.left, h = rect.bottom - rect.top;
	p.Paint(0, 0, w, h, 0);
	DeleteDC(hdcMem);
	DeleteDC(hdc);
	EndPaint(mywindows::float_hWnd, &ps);
}
void floatwindow::createWindow() {
	// 计算悬浮窗口的位置和大小
	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int floatWndWidth = config::getscreen(FLOATW);
	int floatWndHeight = config::getscreen(FLOATH);
	const int floatWndX = config::getscreen(FLOATX);
	const int floatWndY = config::getscreen(FLOATY);

	// 创建悬浮窗口
	mywindows::float_hWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST| WS_EX_TOOLWINDOW,
		L"FLOAT",L"Float Window", WS_POPUPWINDOW,
		floatWndX, floatWndY, floatWndWidth, floatWndHeight,
		NULL, NULL, mywindows::hinstance, NULL);

	// 设置悬浮窗口的初始透明度
	SetLayeredWindowAttributes(mywindows::float_hWnd, 0, 128, LWA_ALPHA);
	// 显示悬浮窗口
	ShowWindow(mywindows::float_hWnd, SW_SHOWNOACTIVATE);
	UpdateWindow(mywindows::float_hWnd);
	int x, y, w, h;
	x = 0; y = 0; w = mywindows::screenWidth; h = mywindows::screenHeight * 0.3;
	mywindows::Quit_hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
		L"QUIT", L"", WS_VISIBLE | WS_POPUP, x, y, w, h, NULL, NULL, mywindows::hinstance, 0);
	SetLayeredWindowAttributes(mywindows::Quit_hwnd, 0, 200, LWA_ALPHA);
	ShowWindow(mywindows::Quit_hwnd, SW_HIDE);
}
void floatwindow::showQuitWindow()
{
	if (!showing) {
		showing = 1;
		ShowWindow(mywindows::Quit_hwnd, SW_SHOWNOACTIVATE);
	}
}
void floatwindow::create()
{
}
void floatwindow::hideQuitWindow()
{
	if (showing) {
		ShowWindow(mywindows::Quit_hwnd, SW_HIDE);
		showing = 0;
	}
}
LRESULT floatwindow::nchittest(WPARAM wParam)
{
	if (wParam == HTLEFT)
	{
		SetCapture(mywindows::float_hWnd);
		return HTCAPTION;
	}
	return HTCLIENT;
}

POINT prevPos;

void floatwindow::lbuttondown(WPARAM wParam)
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
	buttomdown = 1;
}
void floatwindow::mousemove(LPARAM lParam)
{
	if (is_mouse_dragging)
	{
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);

		RECT rect;
		GetWindowRect(mywindows::float_hWnd, &rect);
		int dx = currentMousePos.x - last_mouse_pos.x;
		int dy = currentMousePos.y - last_mouse_pos.y;

		MoveWindow(mywindows::float_hWnd, rect.left + dx, rect.top + dy, rect.right - rect.left, rect.bottom - rect.top, TRUE);

		last_mouse_pos = currentMousePos;
	}
}
bool uplbutton = 0;
void movewindow() {
	int x, y, w, h;
	static double speedx_;
	static double speedy_;
	if (uplbutton) {
		speedx_ = speedx*16.6;
		speedy_ = speedy*16.6;
		uplbutton = 0;
	}while (speedx_ > 1 || speedy_ > 1||speedx_<-1||speedy_<-1) {
		RECT WindowRect;
		GetWindowRect(mywindows::float_hWnd, &WindowRect);
		x = WindowRect.left + (int)speedx_;
		y = WindowRect.top + (int)speedy_;
		w = WindowRect.right - WindowRect.left;
		h = WindowRect.bottom - WindowRect.top;
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
		Sleep(16.6);
		double Mu = config::getd(MU);
		speedx_ = speedx_ * Mu;
		speedy_ = speedy_ * Mu;
		if (TimerID) {
			break;
		}
	}
}
void floatwindow::lbuttonup()
{
	buttomdown = 0;
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
	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;
	speedx =(currentMousePos.x - prevPos.x)/elapsed.count();
	speedy = (currentMousePos.y - prevPos.y) / elapsed.count();
	if (currentMousePos.y <= mywindows::screenHeight * 0.3) {
		ShowWindow(mywindows::float_hWnd,SW_HIDE);
		ShowWindow(mywindows::Quit_hwnd,SW_HIDE);
		PostQuitMessage(0);
	}
	KillTimer(NULL, TimerID);
	TimerID = NULL;
	uplbutton = 1;
	std::thread m(movewindow);
	m.detach();
}

void floatwindow::lbuttonclick(WPARAM wParam)
{
	if (wParam == HTCLIENT)
	{
		// 双击窗口内部区域时的操作，例如显示/隐藏主窗口
		ShowWindow(mywindows::main_hwnd, SW_SHOW);
		ShowWindow(mywindows::float_hWnd, SW_HIDE);
	}
}