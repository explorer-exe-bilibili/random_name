#include "click.h"
#include"ui.h"
#include"NameScreen.h"
#include "FirstScreen.h"
#include "set-json.h"


void click::firstscreen(int x,int y)
{
	ui::FS->click(x, y);
}

void click::setting(const int x, const int y)
{
	ui::SS->clicked(x, y);
}
void click::showname(const int x, const int y)
{
	ui::NS->click(x, y);
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
		case SETTING: ui::SS->clicked(x, y);
			break;
		case HISTORY: ui::HS->click(x, y);
			break;
		}
	}
}
