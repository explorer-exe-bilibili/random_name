#pragma once
#include<Windows.h>

#include "Gp.h"

class MSGback
{
public:
	static void create();
	static void size();
	static void paint();
	static void click(LPARAM lParam);
	static void keyDown(WPARAM wParam);
	static void command(LPARAM lParam, WPARAM wParam);
	static void destroy();
	static void active();
	static void close();
	static void showWindow(WPARAM wParam);
private:
	static Gp* Pptr;
	static LARGE_INTEGER frequency;
	static LARGE_INTEGER lastTime;
	static int frameCount;
	static float fps;

	static void updateFrameRate();
};
