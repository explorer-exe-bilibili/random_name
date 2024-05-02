#pragma once
#include<Windows.h>
#include"Gp.h"
class paintfirstscreen
{
public:
	static bool firsttime;
	static void paintoverlay(HDC hdc, HDC hdcMem);
	static void printfirstmenu(HDC hdc, HDC hdcMem,Gp p);
	static void repaint();
private:
	static bool firstpaint;
};

