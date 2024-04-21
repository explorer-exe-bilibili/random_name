#pragma once
#include<Windows.h>
class paintfirstscreen
{
public:
	static bool firsttime;
	static void paintoverlay(HDC hdc, HDC hdcMem);
	static void printfirstmenu(HDC hdc, HDC hdcMem);
	static void repaint();
private:
	static bool firstpaint;
};

