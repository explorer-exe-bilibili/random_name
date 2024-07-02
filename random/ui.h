#pragma once
#include<Windows.h>
#include "Gp.h"
#include "HistoryScreen.h"
#include "FirstScreen.h"
#include "NameScreen.h"
#include "set-json.h"
#define FIRST_SCREEN 1
#define SETTING 2
#define SHOW_NAMES_ING 3
#define HISTORY 4

class ui
{
public:
	struct buttom_point
	{
		int x, y, xE, yE;
	};
	static int listx[11], listy, listxend, listyend;
	static bool ing, printing, isball10, isball1, ball10ing, clicked, ft;
	static int mode, bottom1x, bottom1y,screenmode;
	static HFONT text, icon, text_big, text_mid, text_list, icon_mid;
	static int overlay1X, overlay2X, overlay3X, overlay4X, overlay1Y, overlayW, overlayH, button1x, button2x, button3x, button4x, buttony, overX, overlayY;
	static int ball10x, bally, ball1x, ball1end, ball10end, ballyend, settingx, settingy, settingxend, settingyend;
	static int ballW, ballH, addnamex, addnamey, addnameendx, addnameendy, addnameW, addnameH;
	static int exitx, exitxend, exity, exityend;
	static bool ScreenModeChanged;
	static buttom_point his;
	static HistoryScreen HS;
	static FirstScreen FS;
	static set2 SS;
	static NameScreen NS;
	static Button ExitB;
};
