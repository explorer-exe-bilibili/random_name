#include "paintfirstscreen.h"
#include"setting.h"
#include "bitmaps.h"
#include "ui.h"
#include "mywindows.h"
#include"sth2sth.h"
#include"config.h"

#define QI_YUAN L"r"
#define SETING L"'"

bool paintfirstscreen::firsttime = 1;
bool paintfirstscreen::firstpaint = 1;

extern HBITMAP hbitmaps[BitmapCounts];
extern BITMAP overlay1Bm, bm, ball, overlay2Bm, overlay3Bm, overlay4Bm, cardbg_, exitinfo_, goldenbg, listbm_, list4star_, list5star_, list6star_, list3star_, buttom_;

void paintfirstscreen::printfirstmenu(HDC hdc, HDC hdcMem) {
	if (setting::initing)firstpaint = 1;
	if (firstpaint) {
		firstpaint = 0;
		SelectObject(hdcMem, hbitmaps[background]);
		StretchBlt(hdc, 0, 0, mywindows::windowWidth, mywindows::windowHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		setting::initing = 0;
	}
	if (setting::offvideo)
		ui::creatbuttom(hdc, hdcMem, ui::bottom1x, ui::bottom1y, L"跳过视频:开");
	if (!setting::offvideo)
		ui::creatbuttom(hdc, hdcMem, ui::bottom1x, ui::bottom1y, L"跳过视频:关");
	SelectObject(hdcMem, hbitmaps[over1]);
	StretchBlt(hdc, ui::overlay1X, ui::overlay1Y, ui::overlayW, ui::overlayH, hdcMem, 0, 0, overlay1Bm.bmWidth, overlay1Bm.bmHeight, SRCCOPY);
	SelectObject(hdcMem, hbitmaps[over2]);
	StretchBlt(hdc, ui::overlay2X, ui::overlay1Y, ui::overlayW, ui::overlayH, hdcMem, 0, 0, overlay2Bm.bmWidth, overlay2Bm.bmHeight, SRCCOPY);
	SelectObject(hdcMem, hbitmaps[over3]);
	StretchBlt(hdc, ui::overlay3X, ui::overlay1Y, ui::overlayW, ui::overlayH, hdcMem, 0, 0, overlay3Bm.bmWidth, overlay3Bm.bmHeight, SRCCOPY);
	SelectObject(hdcMem, hbitmaps[over4]);
	StretchBlt(hdc, ui::overlay4X, ui::overlay1Y, ui::overlayW, ui::overlayH, hdcMem, 0, 0, overlay4Bm.bmWidth, overlay4Bm.bmHeight, SRCCOPY);
	SelectObject(hdcMem, hbitmaps[exitb]);
	StretchBlt(hdc, setting::exitx, setting::exity, setting::exitxend - setting::exitx, setting::exityend - setting::exity, hdcMem, 0, 0, setting::exitbm.bmWidth, setting::exitbm.bmHeight, SRCAND);
	SelectObject(hdcMem, hbitmaps[exiti]);
	StretchBlt(hdc, setting::exitx, setting::exity, setting::exitxend - setting::exitx, setting::exityend - setting::exity, hdcMem, 0, 0, setting::exitbm.bmWidth, setting::exitbm.bmHeight, SRCPAINT);
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
	paintoverlay(hdc, hdcMem);
	firsttime = !firsttime;
}
void paintfirstscreen::repaint()
{
	firstpaint = 1;
	InvalidateRect(mywindows::hWnd, 0, 0);
}
void paintfirstscreen::paintoverlay(HDC hdc, HDC hdcMem) {
	if (ui::mode == 1) {
		SelectObject(hdcMem, hbitmaps[over1]);
		StretchBlt(hdc, ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), hdcMem, 0, 0, overlay1Bm.bmWidth, overlay1Bm.bmHeight, SRCCOPY);
		SelectObject(hdcMem, hbitmaps[pink1b]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[pink1i]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		SelectObject(hdcMem, hbitmaps[pink10b]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[pink10i]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		mywindows::log("set mode1 successfully");
	}
	else if (ui::mode == 2) {
		SelectObject(hdcMem, hbitmaps[over2]);
		StretchBlt(hdc, ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), hdcMem, 0, 0, overlay2Bm.bmWidth, overlay2Bm.bmHeight, SRCCOPY);
		SelectObject(hdcMem, hbitmaps[pink1b]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[pink1i]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		SelectObject(hdcMem, hbitmaps[pink10b]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[pink10i]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		mywindows::log("set mode2 successfully");
	}
	else if (ui::mode == 3) {
		SelectObject(hdcMem, hbitmaps[over3]);
		StretchBlt(hdc, ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), hdcMem, 0, 0, overlay3Bm.bmWidth, overlay3Bm.bmHeight, SRCCOPY);
		SelectObject(hdcMem, hbitmaps[pink1b]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[pink1i]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		SelectObject(hdcMem, hbitmaps[pink10b]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[pink10i]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		mywindows::log("set mode3 successfully");
	}
	else if (ui::mode == 4) {
		SelectObject(hdcMem, hbitmaps[over4]);
		StretchBlt(hdc, ui::overX, ui::overlayY, (mywindows::windowWidth * 0.6), (mywindows::windowHeight * 0.6), hdcMem, 0, 0, overlay4Bm.bmWidth, overlay4Bm.bmHeight, SRCCOPY);
		SelectObject(hdcMem, hbitmaps[blue1b]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[blue1i]);
		StretchBlt(hdc, ui::ball1x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		SelectObject(hdcMem, hbitmaps[blue10b]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCAND);
		SelectObject(hdcMem, hbitmaps[blue10i]);
		StretchBlt(hdc, ui::ball10x, ui::bally, ui::ballW, ui::ballH, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCPAINT);
		mywindows::log("set mode4 successfully");
	}
	mywindows::log("paint overlay bitmap");
}
