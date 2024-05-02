#include "click.h"
#include"ui.h"
#include"directshow.h"
#include"mywindows.h"
#include"paintname.h"
#include "paintfirstscreen.h"
#include "config.h"
#include "set-json.h"

extern set2 setscreen;

void click::firstscreen(int x, int y)
{
	if (x >= ui::overlay1X AND x <= ui::button1x AND y >= ui::overlay1Y AND y <= ui::buttony)
	{
		ui::mode = 1;
		directshow::music(CLICK);
	}
	else if (x >= ui::overlay2X AND x <= ui::button2x AND y >= ui::overlay1Y AND y <= ui::buttony)
	{
		ui::mode = 2;
		directshow::music(CLICK);
	}
	else if (x >= ui::overlay3X AND x <= ui::button3x AND y >= ui::overlay1Y AND y <= ui::buttony)
	{
		ui::mode = 3;
		directshow::music(CLICK);
	}
	else if (x >= ui::overlay4X AND x <= ui::button4x AND y >= ui::overlay1Y AND y <= ui::buttony)
	{
		ui::mode = 4;
		directshow::music(CLICK);
	}
	else if (x >= ui::bottom1x AND x <= ui::bottom1x + mywindows::windowWidth * 0.073 AND y >= ui::bottom1y AND y <=
		ui::bottom1y + mywindows::windowHeight * 0.039)
	{
		setscreen.offvideo = !setscreen.offvideo;
		directshow::music(CLICK);
	}
	else if (x >= ui::ball10x AND x <= ui::ball10end AND y >= ui::bally AND y <= ui::ballyend)
	{
		paintname::showname10();
	}
	else if (x >= ui::ball1x AND x <= ui::ball1end AND y >= ui::bally AND y <= ui::ballyend) { paintname::showname1(); }
	else if (x >= ui::exitx AND x <= ui::exitxend AND y >= ui::exity AND y <= ui::exityend)
	{
		PostMessage(mywindows::main_hwnd, WM_CLOSE, 0, 0);
	}
	else if (x >= ui::settingx AND x <= ui::settingxend AND y >= ui::settingy AND y <= ui::settingyend)
	{
		ui::screenmode = SETTING;
		directshow::music(ENTER);
	}
}

void click::setting(int x, int y)
{
	setscreen.clicked(x, y);
}

void click::showname(int x, int y)
{
	if (ui::ball10ing)
	{
		if (x >= paintname::skipbmx AND x <= paintname::skipbmxend AND y >= paintname::skipbmy AND y <=
			paintname::skipbmyend)paintname::skipped = true;
		ui::clicked = true;
	}
	if (ui::mode == config::getint(SPECIAL)AND x >= ui::addnamex AND x <= ui::addnameendx AND y >= ui::addnamey
		AND y <= ui::addnameendy)paintname::addaname();
	if (!ui::ball10ing)
	{
		ui::screenmode = FIRST_MENU;
		paintfirstscreen::firsttime = true;
		if (!setscreen.offmusic)
			mciSendString(L"play bgm repeat", nullptr, 0, nullptr); // 使用别名 'bgm' 播放音乐，并设置为循环播放
	}
	directshow::stopmusic();
}

void click::doclick(int x, int y)
{
	if (!ui::ing)
	{
		switch (ui::screenmode)
		{
		case FIRST_MENU: firstscreen(x, y);
			break;
		case SHOW_NAMES_ING: showname(x, y);
			break;
		case SETTING: setscreen.clicked(x, y);/*click::setting(x, y);*/
			break;
		}
	}
	paintname::KillAllTimer();
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}
