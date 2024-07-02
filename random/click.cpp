#include "click.h"
#include"ui.h"
#include"directshow.h"
#include"mywindows.h"
#include"NameScreen.h"
#include "FirstScreen.h"
#include "config.h"
#include "set-json.h"


void click::firstscreen(int x,int y)
{
	ui::FS.click(x, y);
}

void click::setting(const int x, const int y)
{
	ui::SS.clicked(x, y);
}
void click::showname(const int x, const int y)
{
	if (ui::ball10ing)
	{
		if (x >= ui::NS.skipbmx AND x <= ui::NS.skipbmxend AND y >= ui::NS.skipbmy AND y <=
			ui::NS.skipbmyend)ui::NS.skipped = true;
		ui::clicked = true;
	}
	if (ui::mode == config::getint(SPECIAL)AND x >= ui::addnamex AND x <= ui::addnameendx AND y >= ui::addnamey
		AND y <= ui::addnameendy)ui::NS.addaname();
	if (!ui::ball10ing)
	{
		ui::screenmode = FIRST_SCREEN;
		ui::FS.firsttime = true;
		if (!ui::SS.offmusic)
			mciSendString(L"play bgm repeat", nullptr, 0, nullptr); // 使用别名 'bgm' 播放音乐，并设置为循环播放
	}
	directshow::stopmusic();
}
void click::doclick(const int x, const int y)
{
	if (!ui::ing)
	{
		switch (ui::screenmode)
		{
		case FIRST_SCREEN: firstscreen(x, y);
			break;
		case SHOW_NAMES_ING: showname(x, y);
			break;
		case SETTING: ui::SS.clicked(x, y);/*click::setting(x, y);*/
			break;
		case HISTORY: ui::HS.click(x, y);
			break;
		}
	}
	ui::NS.KillAllTimer();
}
