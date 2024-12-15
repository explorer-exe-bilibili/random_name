#pragma once
#include "EasyGL/My_Font.h"

class MenuScreen;
class set2;
class NameScreen;
class HistoryScreen;
class FirstScreen;

enum ScreenMode
{
	FIRST_SCREEN = 1,
	SETTING,
	namescreen,
	HISTORY,
	MENU
};

class ui
{
public:
	enum FontSize
	{
		star = 3,
		normal = 20,
		mid = 30,
		big = 40
	};

	static int mode,ScreenMode;
	static Font* text, *icon;
	static bool ScreenModeChanged;
	static HistoryScreen* HS;
	static FirstScreen* FS;
	static set2* SS;
	static NameScreen* NS;
	static MenuScreen* MS;
};
