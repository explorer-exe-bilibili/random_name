#include "floatwindow.h"
#include<Windows.h>
#include "mywindows.h"
#include "resource.h"
#include "set-json.h"

extern set2 setscreen;
bool showfloatwindow = 1,showQuitwindow=1;
WNDPROC floatwindow::g_OrigFloatWndProc = NULL;
bool floatwindow::is_mouse_dragging;
POINT floatwindow::p, floatwindow::p1;
POINT floatwindow::last_mouse_pos;
bool showing = 0;
HBITMAP floatwindow::bitmap;
int floatwindow::icon_w, floatwindow::icon_h;
HBITMAP black;

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
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::float_hWnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);
	RECT rect;
	GetWindowRect(mywindows::float_hWnd, &rect);
	int w = rect.right - rect.left, h = rect.bottom - rect.top;
	SelectObject(hdcMem, black);
	StretchBlt(hdc, 0, 0, w, h, hdcMem, 0, 0, icon_w, icon_h, SRCCOPY);
	SelectObject(hdcMem, bitmap);
	SetBkColor(hdc, RGB(0, 0, 0));
	StretchBlt(hdc, 0, 0, w, h, hdcMem, 0, 0,icon_w , icon_h, SRCCOPY);
	DeleteDC(hdcMem);
	DeleteDC(hdc);
	EndPaint(mywindows::float_hWnd, &ps);
}

void floatwindow::createWindow() {
	// 计算悬浮窗口的位置和大小
	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	constexpr int floatWndWidth = 50;
	constexpr int floatWndHeight = 50;
	const int floatWndX = screenWidth - floatWndWidth - 10;
	const int floatWndY = screenHeight - floatWndHeight - 10;

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
	BITMAP bmp;
	bitmap= (HBITMAP)LoadImage(NULL, L"files\\imgs\\OIP-C.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(bitmap, sizeof(BITMAP), &bmp);
	icon_w = bmp.bmWidth; icon_h = bmp.bmHeight;
}

void floatwindow::hideQuitWindow()
{
	if (showing) {
		ShowWindow(mywindows::Quit_hwnd, SW_HIDE);
		showing = 0;
	}
}

BOOL floatwindow::IsEntirelyCovered(HWND hWnd)
{
	RECT rectWnd, rectDesktop;
	HRGN hRgnWindow, hRgnDesktop;

	// 获取窗口矩形区域
	GetWindowRect(hWnd, &rectWnd);

	// 获取桌面窗口句柄
	HWND hDesktopWnd = GetDesktopWindow();

	// 获取桌面窗口矩形区域
	GetWindowRect(hDesktopWnd, &rectDesktop);

	// 创建窗口区域
	hRgnWindow = CreateRectRgnIndirect(&rectWnd);

	// 创建桌面区域
	hRgnDesktop = CreateRectRgnIndirect(&rectDesktop);

	// 将窗口区域与桌面区域相减
	CombineRgn(hRgnDesktop, hRgnDesktop, hRgnWindow, RGN_DIFF);

	// 遍历剩余区域中的窗口
	HWND hWndCover = NULL;
	while ((hWndCover = GetWindow(mywindows::main_hwnd, GW_HWNDNEXT)) != NULL)
	{
		if (IsWindowVisible(hWndCover) && hWndCover != hWnd && hWndCover != hDesktopWnd)
		{
			HRGN hRgnCover{};
			GetWindowRgn(hWndCover, hRgnCover);

			// 将覆盖窗口区域从桌面区域中减去
			CombineRgn(hRgnDesktop, hRgnDesktop, hRgnCover, RGN_DIFF);
			DeleteObject(hRgnCover);
		}
	}

	// 如果剩余区域为空,说明窗口被完全遮挡
	const BOOL bCovered = (GetRgnBox(hRgnDesktop, &rectDesktop) == NULLREGION);

	DeleteObject(hRgnWindow);
	DeleteObject(hRgnDesktop);

	return bCovered;
}

BOOL floatwindow::IsFullScreen(HWND hWnd)
{
	RECT rectWnd, rectDesktop;

	// 获取窗口矩形区域
	GetWindowRect(hWnd, &rectWnd);

	// 获取桌面窗口矩形区域
	HWND hDesktopWnd = GetDesktopWindow();
	GetWindowRect(hDesktopWnd, &rectDesktop);

	// 如果窗口区域与桌面区域完全重合,则认为是全屏
	return (EqualRect(&rectWnd, &rectDesktop));
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