#include "FirstScreen.h"

#include "bitmaps.h"
#include "config.h"
#include "ConfigItem.h"
#include "directshow.h"
#include "ui.h"
#include "mywindows.h"
#include "set-json.h"
#include"Gp.h"

#define QI_YUAN L"r"
#define SETICON L"'"

#define O1 0
#define O2 1
#define O3 2
#define O4 3
#define SET 4
#define OFFVIDEO 5
#define HIS 6
#define X1 7
#define X10 8
#define EXITBUTTON 9


FirstScreen::FirstScreen(Gp* p_)
{
	p=p_;
	initButtons();
}

FirstScreen::FirstScreen()
{
	p=nullptr;
	initButtons();
}

FirstScreen::~FirstScreen() = default;

void FirstScreen::resetSize()
{
	int mode = ui::mode - 1;
	int tmp = mode;
	while(mode>0)
	{
		mode--;
		overlay[mode].setPoint(-mywindows::WW * (tmp - mode), mywindows::WH * 0.2,
			-mywindows::WW * 0.4 * (tmp - mode), mywindows::WH * 0.8);
	}
	mode = tmp;
	while(mode<config::getint(POOL_COUNT)-1)
	{
		mode++;
		overlay[mode].setPoint(mywindows::WW * (mode - tmp), mywindows::WH * 0.2,
			mywindows::WW * 1.6 * (mode - tmp), mywindows::WH * 0.8);
	}
	mode = tmp;
	overlay[mode].setPoint(mywindows::WW * 0.2, mywindows::WH * 0.2, mywindows::WW * 0.8, mywindows::WH * 0.8);
	changeMode(ui::mode);
	resetPoint();
}

void FirstScreen::click(const int x, const int y)
{
	for(auto& t:b)
	{
		t.click(CLICK,x,y);
	}
}

void FirstScreen::changeMode(int Mode)
{
	ui::mode = 1;
	while (ui::mode < Mode)
	{
		int x = mywindows::WW * (ui::mode - Mode);
		int y = mywindows::WH * 0.2;
		int xend = mywindows::WW * (ui::mode - Mode + 0.6);
		int yend = mywindows::WH * 0.8;
		overlay[ui::mode - 1].MoveTo(x, y, xend, yend, 1, 30, 40);
		ui::mode++;
	}
	ui::mode = 4;
	while (ui::mode > Mode)
	{
		int x = mywindows::WW * (0.2 + ui::mode - Mode);
		int y = mywindows::WH * 0.2;
		int xend = mywindows::WW * (0.8 + ui::mode - Mode);
		int yend = mywindows::WH * 0.8;
		overlay[ui::mode - 1].MoveTo(x, y, xend, yend, 1, 30, 40);
		ui::mode--;
	}
	overlay[ui::mode - 1].MoveTo(mywindows::WW * 0.2, mywindows::WH * 0.2, mywindows::WW * 0.8, mywindows::WH * 0.8, 1, 30, 30);
	InvalidateRect(mywindows::main_hwnd, NULL, false);
}

void FirstScreen::resetPoint()
{
	for (auto& t : b)
	{
		t.refresh();
	}
}

void FirstScreen::initButtons()
{
	b[O1].setxy2WWWH(0.27, 0.075, 0.345, 0.15);
	b[O2].setxy2WWWH(0.38, 0.075, 0.459, 0.15);
	b[O3].setxy2WWWH(0.50, 0.075, 0.571, 0.15);
	b[O4].setxy2WWWH(0.61, 0.075, 0.682, 0.15);
	b[SET].setxy2WWWH(0.05, 0.85, 0.073, 0.886);
	b[OFFVIDEO].setxy2WWWH(0.1, 0.85, 0.173, 0.886);
	b[HIS].setxy2WWWH(0.18, 0.85, 0.253, 0.886);
	b[X1].setxy2WWWH(0.568, 0.85, 0.748, 0.925);
	b[X10].setxy2WWWH(0.766, 0.85, 0.946, 0.925);
	b[EXITBUTTON].setxy2WWWH(0.9, 0.045, 0.93, 0.105);
	b[O1].setMusic(CLICK_MUSIC);
	b[O2].setMusic(CLICK_MUSIC);
	b[O3].setMusic(CLICK_MUSIC);
	b[O4].setMusic(CLICK_MUSIC);
	b[X10].setMusic(CLICK_MUSIC);
	b[X1].setMusic(CLICK_MUSIC);
	b[OFFVIDEO].setMusic(CLICK_MUSIC);
	b[SET].setMusic(ENTER);
	b[HIS].setMusic(ENTER);
	b[EXITBUTTON].setMusic(ENTER);
	b[O1].bind([this] {changeMode(1);});
	b[O2].bind([this] {changeMode(2);});
	b[O3].bind([this] {changeMode(3);});
	b[O4].bind([this] {changeMode(4);});
	b[SET].bind([] { ui::SS->enter(); });
	b[OFFVIDEO].bind([this] {
		ui::SS->offvideo = !ui::SS->offvideo;
		InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
	});
	b[HIS].bind([] {ui::HS->enter(); });
	b[X1].bind([] { ui::NS->showname1(); });
	b[X10].bind([] {ui::NS->showname10(); });
	b[EXITBUTTON].bind(([]
		{
			PostMessage(mywindows::main_hwnd, WM_CLOSE, 0, 0);
		}));
	b[O1].setBmapC(over1, 1);
	b[O2].setBmapC(over2, 1);
	b[O3].setBmapC(over3, 1);
	b[O4].setBmapC(over4, 1);
	b[EXITBUTTON].setBmapC(exitBu, 1);
	b[SET].setFont(&ui::icon_mid, 1);
	b[SET].setText(SETICON);
	b[SET].setTextColor(211, 188, 142);
	b[OFFVIDEO].setFont(&ui::text);
	b[OFFVIDEO].setTextColor(0, 0, 0);
	b[HIS].setFont(&ui::text);
	b[HIS].setText(L"历史记录");
	b[HIS].setTextColor(0, 0, 0);
	b[X1].setBmapC(pink1, 1);
	b[X10].setBmapC(pink10, 1);
	b[HIS].setDisable(1);
	for (int a = 0; a < config::getint(POOL_COUNT);a++) {
		Button b;
		if (a == config::getint(MODE) - 1)b.setPoint(mywindows::WW * 0.2, mywindows::WH * 0.2, mywindows::WW * 0.8, mywindows::WH * 0.8);
		else if (a > config::getint(MODE) - 1)b.setPoint(mywindows::WW * 1.2, mywindows::WH * 0.2, mywindows::WW * 2, mywindows::WH * 0.8);
		else b.setPoint(-mywindows::WW, mywindows::WH * 0.2, -mywindows::WW * 0.2, mywindows::WH * 0.8);
		b.setDisableStr(1);
		b.setBmapC(a);
		overlay.push_back(b);
	}
	for(auto& b:overlay)
	{
		b.reConnect();
	}
	resetSize();
}

void FirstScreen::enter()
{
	ui::screenmode = FIRST_SCREEN;
	explorer::getInstance()->PlayMusic(ENTER);
}

void FirstScreen::paint()
{
	p->Paint(0, 0, mywindows::WW, mywindows::WH, BackGround);
	if(ui::SS->offvideo)
	{
		b[OFFVIDEO].setText(L"跳过视频:开");
	}
	else
	{
		b[OFFVIDEO].setText(L"跳过视频:关");
	}
	if(ui::mode<=3)
	{
		b[X1].setBmapC(pink1,1);
		b[X10].setBmapC(pink10,1);
	}
	else
	{
		b[X1].setBmapC(blue1,1);
		b[X10].setBmapC(blue10,1);
	}
	for(auto & t :b)
	{
		t.paint();
	}
	for(auto&b:overlay)
	{
		b.paint();
	}
}


void FirstScreen::repaint()
{
	firstpaint = 1;
	InvalidateRect(mywindows::main_hwnd, nullptr, 0);
}


void FirstScreen::setGp(Gp* p_)
{
	p=p_;
	for(auto & t :b)
	{
		t.setGp(p);
	}
	for(auto&b:overlay)
	{
		b.setGp(p);
	}
}

