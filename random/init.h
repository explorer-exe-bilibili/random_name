#pragma once
#include<Windows.h>
#include<string>

class init
{
public:
	static void font();
	static void resetxy();
	static void main(WNDPROC w1, WNDPROC w2, WNDPROC w3);
	static void MainWindow();
	static void regwindow(WNDPROC w1, WNDPROC w2, WNDPROC w3);
	static DWORD WINAPI Regwindows(); // �޸�����
	static DWORD WINAPI Upgrade();    // �޸�����
	static bool cheakIsFontInstalled(const std::wstring fontName);
	static void music();
	static void config();
};
