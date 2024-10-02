#include "Button.h"

#include "configitem.h"
#include "VideoPlayer.h"
#include "explorer.h"
#include "Gp.h"
#include "mywindows.h"
#include "ui.h"

bool Button::needFresh = false;

Button::Button(const int x, const int y, const int xE, const int yE,
	const int BitmapC, const std::wstring& text)
{
	this->x = x;
	this->y = y;
	this->xE = xE;
	this->yE = yE;
	this->BmapC = BitmapC;
	this->text = text;
	xP = &(this->x);
	yP = &(this->y);
	xEP = &(this->xE);
	yEP = &(this->yE);
}

Button::Button()
{
	xP = &(this->x);
	yP = &(this->y);
	xEP = &(this->xE);
	yEP = &(this->yE);
}

Button::~Button()
{
	x = 0; y = 0; xE = 0; yE = 0;
	xP = nullptr; yP=nullptr; xEP=nullptr; yEP=nullptr;
	BmapC = 0;
	FuncCounts = 0;
	functions.clear();
}

int Button::click(const int condition, const int x, const int y) const
{
	if (Disable)return Disabled;
	if (x != -1 && y != -1) {
		if (x >= *xP AND x <= *xEP AND y >= *yP AND y <= *yEP)
		{
			if (!music_string.empty())
				explorer::getInstance()->PlayMusic(music_string);
			if (functions.size()>condition)
			{
				functions[condition]();
			}
			return Clicked;
		}
	}
	else if(functions.size()>condition)
	{
		functions[condition]();
	}
	return outOfRange;
}

void Button::paint() const
{
	static bool debug=config::getint(DEBUG);
	if (Disable)return;
	if ((xE < 0 AND x < 0)
		OR(yE < 0 AND y < 0)
		OR(x > mywindows::WW AND xE > mywindows::WW)
		OR(y > mywindows::WH AND yE > mywindows::WH))
		return;
	if(!p)return;
	if(!DisableBmap)
		p->Paint(x, y, xE-x, yE-y,BmapC);
	if (!DisableStr) {
		// 绘制文本
		if (IsVertical)
		{
			if (font != nullptr)
				p->DrawVerticalStringBetween(text, *font, x, y, xE, yE, R, G, B);
			else
				p->DrawVerticalStringBetween(text, ui::text_mid, x, y, xE, yE, R, G, B);
		}
		else {
			if (font != nullptr)
				p->DrawStringBetween(text, *font, x, y, xE, yE, R, G, B);
			else
				p->DrawStringBetween(text, ui::text_mid, x, y, xE, yE, R, G, B);
		}
	}
	if(debug)
	{
		p->DrawSqare(x, y, xE, yE, 255, 0, 0, false);
	}
}

void Button::setText(const std::wstring& NewText, const int DisableBmap)
{
	text = NewText;
	if(DisableBmap!=-1)
		this->DisableBmap = DisableBmap;
}

void Button::setTextColor(const int R, const int G, const int B, const int DisableBmap)
{
	this->R = R;
	this->G = G;
	this->B = B;
	if(DisableBmap!=-1)
		this->DisableBmap = DisableBmap;
}

void Button::setPoint(const int x, const int y, const int xE, const int yE)
{
	this->x = x;
	this->y = y;
	this->xE = xE;
	this->yE = yE;
}

void Button::setxy2WWWH(const double x2WW, const double y2WH, const double xE2WW, const double yE2WH)
{
	this->xP = &x;
	this->xEP = &xE;
	this->yP = &y;
	this->yEP = &yE;
	this->x2WW = x2WW;
	this->y2WH = y2WH;
	this->xE2WW = xE2WW;
	this->yE2WH = yE2WH;
}

void Button::setBmapC(const int Bmapc, const int DisableStr)
{
	BmapC = Bmapc;
	if(DisableStr!=-1)
		this->DisableStr = DisableStr;
}

void Button::setFont(HFONT* font, const int DisableBmap)
{
	this->font = font;
	if(DisableBmap!=-1)
		this->DisableBmap = DisableBmap;
}

void Button::setDisableStr(const bool newValue)
{
	DisableStr = newValue;
}

void Button::setDisableBmap(const bool newValue)
{
	DisableBmap = newValue;
}

void Button::setMusic(const std::string& music_string)
{
	this->music_string = music_string;
}

void Button::setGp(Gp* p)
{
	this->p = p;
}

void Button::setDisable(bool newValue)
{
	Disable= newValue;
}

void Button::setVertical(bool newValue)
{
	IsVertical = newValue;
}

void Button::MoveTo(int x, int y, int xend, int yend, bool smoothly,double xVelocity,double yVelocity)
{
	Moving = true;
	if(smoothly)
	{
		//move_timer.Describe = "Button move timer";
		move_timer.setDelay(1000 / 60);
		move_timer.setPool(true);
		move_timer.setCallBack([this, x, y, xend, yend, xVelocity, yVelocity]() {
			
			needFresh = true;
			if (this->x < x ) {
				this->x += xVelocity;
				if (x < this->x)
					this->x = x;
				if (xend == -1) {
					this->xE += xVelocity;
					if (x < this->x)
						this->xE = xend;
				}
			}
			else if (this->x > x) {
				this->x -= xVelocity;
				if (x > this->x)
					this->x = x;
				if (xend == -1) {
					this->xE -= xVelocity;
					if (x > this->x)
						this->xE = xend;
				}
			}
			if (this->y < y) {
				this->y += yVelocity;
				if (y < this->y)
					this->y = y;
				if (yend == -1) {
					this->yE += yVelocity;
					if (y < this->y)
						this->yE = yend;
				}
			}
			else if (this->y > y) {
				this->y -= yVelocity;
				if (y > this->y)
					this->y = y;
				if (yend == -1) {
					this->yE += yVelocity;
					if (y > this->y)
						this->y = y;
				}
			}
			if (this->xE < xend AND xend != -1) {
				this->xE += xVelocity;
				if (xend < this->xE)
					this->xE = xend;
			}
			else if (this->xE > xend AND xend != -1) {
				this->xE -= xVelocity;
				if (xend > this->xE)
					this->xE = xend;
			}
			if (this->yE < yend AND yend != -1) {
				this->yE += yVelocity;
				if (yend < this->yE)
					this->yE = yend;
			}
			else if (this->yE > yend AND yend != -1) {
				this->yE -= yVelocity;
				if (yend > this->yE)
					this->yE = yend;
			}
			//检测是否在屏幕外
			if ((this->x > mywindows::WW OR this->xE < 0)AND(this->y > mywindows::WH OR this->yE < 0))
			{
				this->x = x;
				this->y = y;
				this->xE = xend;
				this->yE = yend;
			}
			if (this->x == x AND this->y == y AND(this->xE == xend OR xend == -1) AND(this->yE == yend OR yend == -1)) {
				this->move_timer.stop();
				Moving = false;
			}
		});
		move_timer.init();
		move_timer.start();
	}
	else
	{
		this->x = x;
		this->y = y;
		this->xE = xend;
		this->yE = yend;
	}
}

void Button::refresh()
{
	if (Moving)return;
	x = x2WW * mywindows::WW;
	y = y2WH * mywindows::WH;
	xE = xE2WW * mywindows::WW;
	yE = yE2WH * mywindows::WH;
}

void Button::changeStr(const std::wstring& NewStr)
{
	text=NewStr;
	paint();
}

void Button::changeBmap(const int NewBmapC)
{
	BmapC=NewBmapC;
	paint();
}

void Button::reConnect()
{
	xP = &(this->x);
	yP = &(this->y);
	xEP = &(this->xE);
	yEP = &(this->yE);
}

void Button::disConnect()
{
	xP = nullptr;
	yP = nullptr;
	xEP = nullptr;
	yEP = nullptr;
}

Button& Button::operator=(const Button& b)
{
	if (this == &b)return *this;
	x = b.x;
	y = b.y;
	xE = b.xE;
	yE = b.yE;
	xP = &x;
	yP = &y;
	xEP = &xE;
	yEP = &yE;
	x2WW = b.x2WW;
	y2WH = b.y2WH;
	xE2WW = b.xE2WW;
	yE2WH = b.yE2WH;
	xAdd = b.xAdd;
	yAdd = b.yAdd;
	xEAdd = b.xEAdd;
	yEAdd = b.yEAdd;
	R = b.R;
	G = b.G;
	B = b.B;
	font = b.font;
	BmapC = b.BmapC;
	FuncCounts = b.FuncCounts;
	DisableBmap = b.DisableBmap;
	DisableStr = b.DisableStr;
	Disable = b.Disable;
	Moving = b.Moving;
	IsVertical = b.IsVertical;
	text = b.text;
	TextW = b.TextW;
	TextH = b.TextH;
	functions = b.functions;
	music_string = b.music_string;
	move_timer = b.move_timer;
	return *this;
}

Button::Breturn Button::Binded() const
{
	if(binded!=nullptr){
		Breturn b;
		b.xAdd = xAdd;
		b.yAdd = yAdd;
		b.xEAdd = xEAdd;
		b.yEAdd = yEAdd;
		b.binded = binded;
		return b;
	}
	else
	{
		Breturn b;
		b.xAdd = 0;
		b.yAdd = 0;
		b.xEAdd = 0;
		b.yEAdd = 0;
		b.binded = nullptr;
		return b;
	}
}

Button::operator bool() const
{
	if(xP==nullptr OR yP==nullptr OR xEP==nullptr OR yEP==nullptr)
		return false;
	else
		return true;
}

bool Button::operator==(const Button& b) const
{
	if (x == b.x AND y == b.y AND xE == b.xE AND yE == b.yE)
		return true;
	else
		return false;
}

int Button::bind(const std::function<void()>& func, const int condition)
{
	if(functions.size()>condition)
	{
		functions[condition] = func;
	}
	else
	{
		functions.push_back(func);
		FuncCounts++;
	}
	return 0;
}

bool Button::bind(Button* b)
{
	binded = b;
	functions= b->functions;
	xP= b->xP;
	yP= b->yP;
	xEP= b->xEP;
	yEP= b->yEP;
	return true;
}

int Button::bindX(const Button* b, const int add)
{
	this->x = -1;
	this->xAdd=add;
	this->xP= b->xP;
	return *xP;
}

int Button::bindXE(const Button* b, const int add)
{
	this->xE = -1;
	this->xEAdd=add;
	this->xEP = b->xEP;
	return *xEP;
}

int Button::bindY(const Button* b, const int add)
{
	this->y = -1;
	this->yAdd=add;
	this->yP = b->yP;
	return *yP;
}

int Button::bindYE(const Button* b, const int add)
{
	this->yE = -1;
	this->yEAdd=add;
	this->yEP = b->yEP;
	return *yEP;
}

int Button::bindXXE(const Button* b, const int add)
{
	this->xE = -1;
	this->xP= b->xP;
	this->xAdd=add;
	this->xEAdd = add;
	this->xEP = b->xEP;
	return *xEP+*xP;
}

int Button::bindYYE(const Button* b, const int add)
{
	this->yE = -1;
	this->yP = b->yP;
	this->yAdd=add;
	this->yEAdd = add;
	this->yEP = b->yEP;
	return *yEP+*yP;
}

int Button::getx() const
{
	if(x2WW!=-1){
		if(x==-1&&binded)
			return binded->getx()+xAdd;
		else 
			return x;
	}
	else
		return mywindows::WW*x2WW;
}

int Button::gety() const
{
	if(y2WH != -1){
		if(y==-1&&binded)
			return binded->gety()+yAdd;
		else 
			return y;
	}
	else
		return mywindows::WH*y2WH;
}

int Button::getxE() const
{
	if(xE2WW!=-1){
		if(xE==-1&&binded)
			return binded->getxE()+xEAdd;
		else 
			return xE;
	}
	else
		return mywindows::WW*xE2WW;
}

int Button::getyE() const
{
	if(yE2WH!=-1){
		if(yE==-1&&binded)
			return binded->getyE()+yEAdd;
		else 
			return yE;
	}
	else
		return mywindows::WH*yE2WH;
}

bool Button::IsDisabled() const
{
	return Disable;
}

bool Button::IsMoving() const
{
	return Moving;
}
