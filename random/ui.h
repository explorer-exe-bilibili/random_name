#pragma once
#include<Windows.h>

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
	static int mode,ScreenMode;
	static HFONT text, icon, text_big, text_mid, text_list, icon_mid;
	static bool ScreenModeChanged;
	static HistoryScreen* HS;
	static FirstScreen* FS;
	static set2* SS;
	static NameScreen* NS;
	static MenuScreen* MS;
};
