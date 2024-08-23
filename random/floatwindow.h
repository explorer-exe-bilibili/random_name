#pragma once
#include<Windows.h>
class floatwindow
{
public:
	// 全局变量,保存原始窗口过程指针
	static WNDPROC g_OrigFloatWndProc;
	static void open();
	static void stop();
	static LRESULT nchittest(WPARAM wParam);
	static void lbuttondown(WPARAM wParam);
	static void mousemove(LPARAM lParam);
	static void lbuttonup();
	static void lbuttonclick(WPARAM wParam);
	static void hideQuitWindow();
	static void showQuitWindow();
	static void create();
	static void paint();
private:
	static void createWindow();
	static bool is_mouse_dragging;
	static POINT last_mouse_pos;
	static POINT p, p1;
	static HBITMAP bitmap;
	static int icon_w, icon_h;
};