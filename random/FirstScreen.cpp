#include "FirstScreen.h"

#include "bitmaps.h"
#include "config.h"
#include "ConfigItem.h"
#include "VideoPlayer.h"
#include "ui.h"
#include "mywindows.h"
#include "set-json.h"
#include"Gp.h"
#include "HistoryScreen.h"
#include "NameScreen.h"

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

FirstScreen::FirstScreen()
{
	p=nullptr;
	FirstScreen::regButtons();
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

void FirstScreen::click(const int x, const int y) const
{
	for(auto& t:buttons)
	{
		t.click(CLICK,x,y);
	}
}

void FirstScreen::changeMode(int Mode)
{
	int v = config::getint(SPEED_FIRST_SCREEN);
	ui::mode = 1;
	while (ui::mode < Mode)
	{
		int x = mywindows::WW * (ui::mode - Mode);
		int y = mywindows::WH * 0.2;
		int xend = mywindows::WW * (ui::mode - Mode + 0.6);
		int yend = mywindows::WH * 0.8;
		overlay[ui::mode - 1].MoveTo(x, y, xend, yend, true, v, v);
		ui::mode++;
	}
	ui::mode = 4;
	while (ui::mode > Mode)
	{
		int x = mywindows::WW * (0.2 + ui::mode - Mode);
		int y = mywindows::WH * 0.2;
		int xend = mywindows::WW * (0.8 + ui::mode - Mode);
		int yend = mywindows::WH * 0.8;
		overlay[ui::mode - 1].MoveTo(x, y, xend, yend, true, v, v);
		ui::mode--;
	}
	overlay[ui::mode - 1].MoveTo(mywindows::WW * 0.2, mywindows::WH * 0.2, mywindows::WW * 0.8, mywindows::WH * 0.8, true, v, v);
	InvalidateRect(mywindows::main_hwnd, nullptr, false);
}

void FirstScreen::resetPoint()
{
	for (auto& t : buttons)
	{
		t.refresh();
	}
}

void FirstScreen::regButtons()
{
	for (int a = 0; a < 10; a++)
		buttons.emplace_back();
	buttons[O1].bind([this] {changeMode(1); });
	buttons[O1].setBmapC(over1, 1);
	buttons[O1].setMusic(CLICK_MUSIC);
	buttons[O1].setxy2WWWH(0.27, 0.075, 0.345, 0.15);
	buttons[O2].bind([this] {changeMode(2); });
	buttons[O2].setBmapC(over2, 1);
	buttons[O2].setMusic(CLICK_MUSIC);
	buttons[O2].setxy2WWWH(0.38, 0.075, 0.459, 0.15);
	buttons[O3].bind([this] {changeMode(3); });
	buttons[O3].setBmapC(over3, 1);
	buttons[O3].setMusic(CLICK_MUSIC);
	buttons[O3].setxy2WWWH(0.50, 0.075, 0.571, 0.15);
	buttons[O4].bind([this] {changeMode(4); });
	buttons[O4].setBmapC(over4, 1);
	buttons[O4].setMusic(CLICK_MUSIC);
	buttons[O4].setxy2WWWH(0.61, 0.075, 0.682, 0.15);
	buttons[EXITBUTTON].bind(([]{PostMessage(mywindows::main_hwnd, WM_CLOSE, 0, 0);}));
	buttons[EXITBUTTON].setBmapC(exitBu, 1);
	buttons[EXITBUTTON].setMusic(ENTER);
	buttons[EXITBUTTON].setxy2WWWH(0.9, 0.045, 0.93, 0.105);
	buttons[HIS].bind([] { HistoryScreen::enter(); });
	buttons[HIS].setDisable(true);
	buttons[HIS].setFont(&ui::text);
	buttons[HIS].setMusic(ENTER);
	buttons[HIS].setText(L"历史记录");
	buttons[HIS].setTextColor(ARGB(255,0,0,0));
	buttons[HIS].setxy2WWWH(0.18, 0.85, 0.253, 0.886);
	buttons[OFFVIDEO].bind([this]
	{
		set2::offVideo = !set2::offVideo;
		InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
	});
	buttons[OFFVIDEO].setFont(&ui::text);
	buttons[OFFVIDEO].setMusic(CLICK_MUSIC);
	buttons[OFFVIDEO].setTextColor(ARGB(255,0,0,0));
	buttons[OFFVIDEO].setxy2WWWH(0.1, 0.85, 0.173, 0.886);
	buttons[SET].bind([] { set2::enter(); });
	buttons[SET].setFont(&ui::icon_mid, 1);
	buttons[SET].setMusic(ENTER);
	buttons[SET].setText(SETICON);
	buttons[SET].setTextColor(ARGB(255,211, 188, 142));
	buttons[SET].setxy2WWWH(0.05, 0.85, 0.073, 0.886);
	buttons[X1].bind([] { ui::NS->ShowName1(); });
	buttons[X1].setBmapC(pink1, 1);
	buttons[X1].setMusic(CLICK_MUSIC);
	buttons[X1].setxy2WWWH(0.568, 0.85, 0.748, 0.925);
	buttons[X10].bind([] {ui::NS->ShowName10(); });
	buttons[X10].setBmapC(pink10, 1);
	buttons[X10].setMusic(CLICK_MUSIC);
	buttons[X10].setxy2WWWH(0.766, 0.85, 0.946, 0.925);
	for (int a = 0; a < config::getint(POOL_COUNT);a++) {
		Button b;
		if (a == config::getint(MODE) - 1)b.setPoint(mywindows::WW * 0.2, mywindows::WH * 0.2, mywindows::WW * 0.8, mywindows::WH * 0.8);
		else if (a > config::getint(MODE) - 1)b.setPoint(mywindows::WW * 1.2, mywindows::WH * 0.2, mywindows::WW * 2, mywindows::WH * 0.8);
		else b.setPoint(-mywindows::WW, mywindows::WH * 0.2, -mywindows::WW * 0.2, mywindows::WH * 0.8);
		b.setDisableStr(true);
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
	ui::ScreenMode = FIRST_SCREEN;
	if (!set2::offMusic)
		mciSendString(L"play bgm repeat", nullptr, 0, nullptr);
}

void FirstScreen::paint()
{
	p->Paint(0, 0, mywindows::WW, mywindows::WH, BackGround);
	if(set2::offVideo)
	{
		buttons[OFFVIDEO].setText(L"跳过视频:开");
	}
	else
	{
		buttons[OFFVIDEO].setText(L"跳过视频:关");
	}
	if(ui::mode<=3)
	{
		buttons[X1].setBmapC(pink1,1);
		buttons[X10].setBmapC(pink10,1);
	}
	else
	{
		buttons[X1].setBmapC(blue1,1);
		buttons[X10].setBmapC(blue10,1);
	}
	for(auto & t :buttons)
	{
		t.paint();
	}
	for(auto&b:overlay)
	{
		b.paint();
	}
}

void FirstScreen::setGp(Gp* p)
{
	Screen::setGp(p);
	for(auto&b:overlay)
	{
		b.setGp(p);
	}
}

