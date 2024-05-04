#include "ui.h"
#include "mywindows.h"
#include "bitmaps.h"


int ui::listx[11]={0}, ui::listy=0, ui::listxend=0, ui::listyend=0;
bool ui::ing=0, ui::printing=0, ui::isball10=0, ui::isball1=0, ui::ball10ing=0, ui::clicked=0, ui::ft=0;
int ui::mode=1, ui::bottom1x=0, ui::bottom1y=0, ui::screenmode=FIRST_MENU;
HFONT ui::text=0, ui::icon=0, ui::text_big=0, ui::text_mid=0, ui::text_list=0, ui::icon_mid=0;
int ui::overlay1X = 0, ui::overlay2X = 0, ui::overlay3X = 0, ui::overlay4X = 0;
int ui::overlay1Y = 0, ui::overlayW = 0, ui::overlayH = 0, ui::button1x = 0, ui::button2x = 0;
int ui::button3x = 0, ui::button4x = 0, ui::buttony = 0, ui::overX = 0, ui::overlayY = 0;
int ui::ball10x = 0, ui::bally = 0, ui::ball1x = 0, ui::ball1end = 0, ui::ball10end = 0, ui::ballyend = 0;
int ui::settingx = 0, ui::settingy = 0, ui::settingxend = 0, ui::settingyend = 0;
int ui::ballW = 0, ui::ballH = 0, ui::addnamex = 0, ui::addnamey = 0, ui::addnameendx = 0;
int ui::addnameendy=0, ui::addnameW=0, ui::addnameH = 0;
int ui::exitx = 0, ui::exitxend = 0, ui::exity = 0, ui::exityend = 0;
bool ui::ScreenModeChanged = 1;

void ui::creatbuttom(Gp *p, int x, int y, LPCWSTR text_out)
{
	p->Paint(x, y, mywindows::windowWidth * 0.073, mywindows::windowHeight * 0.039, Buttom);
	HDC hdc = p->GetDC();
	SelectObject(hdc, ui::text);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkColor(hdc, RGB(225, 222, 213));
	TextOut_(hdc, x + mywindows::windowWidth * 0.0073, y + mywindows::windowHeight * 0.0065, text_out);
	p->ReleaseDC(hdc);
}
