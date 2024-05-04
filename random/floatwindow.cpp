#include "floatwindow.h"
#include<Windows.h>
#include "mywindows.h"
#include "resource.h"
#include "set-json.h"
#include "config.h"

extern set2 setscreen;
bool showfloatwindow = 1,showQuitwindow=1;
WNDPROC floatwindow::g_OrigFloatWndProc = NULL;
bool floatwindow::is_mouse_dragging;
POINT floatwindow::p, floatwindow::p1;
POINT floatwindow::last_mouse_pos;
bool showing = 0;
HBITMAP floatwindow::bitmap;
int floatwindow::icon_w, floatwindow::icon_h;

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

void floatwindow::lbuttondown(WPARAM wParam)
{
	if (wParam == HTCLIENT)
	{
		showQuitWindow();
		GetCursorPos(&p); //获取鼠标位置
		is_mouse_dragging = true;
		GetCursorPos(&last_mouse_pos);
		SetCapture(mywindows::float_hWnd);
	}
}

void floatwindow::mousemove()
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

void floatwindow::lbuttonup()
{
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
	if (currentMousePos.y <= mywindows::screenHeight * 0.3) {
		ShowWindow(mywindows::float_hWnd,SW_HIDE);
		ShowWindow(mywindows::Quit_hwnd,SW_HIDE);
		PostQuitMessage(0);
	}
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