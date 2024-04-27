#pragma once
#include<Windows.h>

class init
{
public:
	static void picture();
	static void font();
	static void resetxy();
	static void resetpicture();
	static void main(WNDPROC w1, WNDPROC w2, WNDPROC w3);
	static void MainWindow();
	static void regwindow(WNDPROC w1, WNDPROC w2, WNDPROC w3);
	static DWORD Regwindows();
	static DWORD LoadIMG();
	static DWORD Upgrade();
	static void music();
};
