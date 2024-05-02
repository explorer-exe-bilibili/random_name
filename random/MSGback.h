#pragma once
#include<Windows.h>
#include"Gp.h"
class MSGback
{
public:
	static void create();
	static void paint();
	static void click(LPARAM lParam);
	static void keyDown(WPARAM wParam);
	static void commond(LPARAM lParam, WPARAM wParam);
	static void destroy();
	static void active();
	static void close();
	static void showwindow(WPARAM wParam);
private:
	static void destroyall();
};
