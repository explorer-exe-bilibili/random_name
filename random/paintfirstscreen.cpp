#include "paintfirstscreen.h"
#include "bitmaps.h"
#include "ui.h"
#include "mywindows.h"
#include"sth2sth.h"
#include"config.h"
#include "set-json.h"
#include"Gp.h"

#define QI_YUAN L"r"
#define SETING L"'"

bool paintfirstscreen::firsttime = 1;
bool paintfirstscreen::firstpaint = 1;

extern HBITMAP hbitmaps[BitmapCounts];
extern BITMAP overlay1Bm, bm, ball, overlay2Bm, overlay3Bm, overlay4Bm, cardbg, exitinfo, goldenbg, listbm;
extern set2 setscreen;

void paintfirstscreen::printfirstmenu(Gp *p) {
	if (ui::ScreenModeChanged)firstpaint = 1;
	if (firstpaint) {
		firstpaint = 0;
		p->Paint(0, 0, mywindows::windowWidth, mywindows::windowHeight, BackGround);
		ui::ScreenModeChanged = 0;
	}
	if (setscreen.offvideo)
		ui::creatbuttom(p, ui::bottom1x, ui::bottom1y, L"跳过视频:开");
	else
		ui::creatbuttom(p, ui::bottom1x, ui::bottom1y, L"跳过视频:关");
	p->Paint(ui::overlay1X, ui::overlay1Y, ui::overlayW, ui::overlayH, over1);
	p->Paint(ui::overlay2X, ui::overlay1Y, ui::overlayW, ui::overlayH, over2);
	p->Paint(ui::overlay3X, ui::overlay1Y, ui::overlayW, ui::overlayH, over3);
	p->Paint(ui::overlay4X, ui::overlay1Y, ui::overlayW, ui::overlayH, over4);
	p->Paint(ui::exitx, ui::exity, ui::exitxend - ui::exitx, ui::exityend - ui::exity, exiti);
	HDC hdc = p->GetDC();
	SelectObject(hdc, ui::icon);
	SetBkColor(hdc, RGB(255, 255, 255));
	SetTextColor(hdc, RGB(211, 188, 142));
	TextOut_(hdc, mywindows::windowWidth * 0.045, mywindows::windowHeight * 0.04, QI_YUAN);
	SelectObject(hdc, ui::text);
	SetTextColor(hdc, RGB(0, 0, 0));
	TextOut_(hdc, mywindows::windowWidth * 0.075, mywindows::windowHeight * 0.05, config::get(WINDOW_TITEL).c_str());
	SelectObject(hdc, ui::icon_mid);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkColor(hdc, RGB(225, 222, 213));
	TextOut_(hdc, ui::settingx, ui::settingy, SETING);
	firsttime = !firsttime;
	p->ReleaseDC(hdc);
	paintoverlay(p);

}
void paintfirstscreen::repaint()
{
	firstpaint = 1;
	InvalidateRect(mywindows::main_hwnd, 0, 0);
}
void paintfirstscreen::paintoverlay(Gp *p) {
	if (ui::mode == 1) {
		p->Paint(ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), over1);
		p->Paint(ui::ball1x, ui::bally, ui::ballW, ui::ballH, pink1b);
		p->Paint(ui::ball10x, ui::bally, ui::ballW, ui::ballH, pink10b);
		mywindows::log("set mode1 successfully");
	}
	else if (ui::mode == 2) {
		p->Paint(ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), over2);
		p->Paint(ui::ball1x, ui::bally, ui::ballW, ui::ballH, pink1b);
		p->Paint(ui::ball10x, ui::bally, ui::ballW, ui::ballH, pink10b);
		mywindows::log("set mode2 successfully");
	}
	else if (ui::mode == 3) {
		p->Paint(ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), over3);
		p->Paint(ui::ball1x, ui::bally, ui::ballW, ui::ballH, pink1b);
		p->Paint(ui::ball10x, ui::bally, ui::ballW, ui::ballH, pink10b);
		mywindows::log("set mode3 successfully");
	}
	else if (ui::mode == 4) {
		p->Paint(ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), over4);
		p->Paint(ui::ball1x, ui::bally, ui::ballW, ui::ballH, blue1b);
		p->Paint(ui::ball10x, ui::bally, ui::ballW, ui::ballH, blue10b);
		mywindows::log("set mode4 successfully");
	}
	mywindows::log("paint overlay bitmap");
}
