#pragma once
#include<Windows.h>
#include<string>

namespace init
{
	void font();
	void resetPoint();
	void main(WNDPROC w1, WNDPROC w2, WNDPROC w3);
	void MainWindow();
	void regWindow(WNDPROC w1, WNDPROC w2, WNDPROC w3);
	DWORD WINAPI RegWindows(); // �޸�����
	DWORD WINAPI Upgrade();    // �޸�����
	bool checkIsFontInstalled(const std::wstring& fontName);
	void music();
	void config();
}