#include "Button.h"

#include "mywindows.h"
#include "ui.h"

Button::Button(const int x, const int y, const int xE, const int yE, const int BitmapC,std::wstring text_)
{
	this->x = x;
	this->y = y;
	this->xE = xE;
	this->yE = yE;
	this->BmapC = BitmapC;
	text= text_;
}

Button::Button()
{
}

Button::~Button()
{
	x = 0; y = 0; xE = 0; yE = 0;
	xP = nullptr; yP=nullptr; xEP=nullptr; yEP=nullptr;
	BmapC = 0;
	FuncCounts = 0;
	functions.clear();
}

void Button::click(const int condition, const int x, const int y)
{
	if (x != -1 && y != -1) {
		if (x >= *xP AND x <= *xEP AND y >= *yP AND y <= *yEP)
		{
			if (functions.size()>condition)
			{
				functions[condition]();
			}
		}
	}
	else
		functions[condition]();
}

void Button::paint() const
{
	if(!p)return;
	if(!DisableBmap)
		p->Paint(x, y, xE-x, yE-y,BmapC);
	if(!DisableStr){
		if(TextW!=-1 AND TextH!=-1)
		{
			if(font!=nullptr)
				p->DrawString(text, *font, x + (xE - x - TextW) / 2, y + (yE - y - TextH) / 2, R, G, B);
			else
				p->DrawString(text, ui::icon_mid, x + (xE - x - TextW) / 2, y + (yE - y - TextH) / 2, R, G, B);
		}
		if(font)
		{
			if (*font == ui::text_mid || *font == ui::icon)
			{
				int w = xE - x - (text.size() * mywindows::WW * 0.17 * 0.17 * 0.77) / 2;
				int h = yE - y - mywindows::WH * 0.17 * 0.17 / 2;
				p->DrawString(text, *font, x + w, y + h, R, G, B);
			}
			else if (*font == ui::text || *font == ui::icon_mid)
			{
				int w = (xE - x - (text.size() - 1) * mywindows::WW * 0.17 * 0.11 * 0.77) / 2;
				int h = (yE - y - mywindows::WH * 0.17 * 0.12) / 2;
				p->DrawString(text, *font, x + w, y + h, R, G, B);
			}
			else
				p->DrawString(text, *font, x + (xE - x - (text.size() * mywindows::WW * 0.17 * 0.17)) / 2,
					y + (yE - y - mywindows::WH * 0.17*0.17) / 2, R, G, B);
		}
		else
		{
			p->DrawString(text, ui::icon_mid, x + (xE - x - (text.size() * mywindows::WW * 0.17 * 0.17)) / 2,
				y + (yE - y - mywindows::WH * 0.17*0.17) / 2, R, G, B);
		}
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

void Button::setGp(Gp* p)
{
	this->p = p;
}

void Button::refreash()
{
	x = x2WW * mywindows::WW;
	y = y2WH * mywindows::WH;
	xE = xE2WW * mywindows::WW;
	yE = yE2WH * mywindows::WH;
}

void Button::changeStr(std::wstring NewStr)
{
	text=NewStr;
	paint();
}

void Button::changeBmap(const int NewBmapC)
{
	BmapC=NewBmapC;
	paint();
}

Button::Breturn Button::Binded()
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

int Button::bind(std::function<void()> func, const int condition)
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

int Button::bindX(Button* b, const int add)
{
	this->x = -1;
	this->xAdd=add;
	this->xP= b->xP;
	return *xP;
}

int Button::bindXE(Button* b, const int add)
{
	this->xE = -1;
	this->xEAdd=add;
	this->xEP = b->xEP;
	return *xEP;
}

int Button::bindY(Button* b, const int add)
{
	this->y = -1;
	this->yAdd=add;
	this->yP = b->yP;
	return *yP;
}

int Button::bindYE(Button* b, const int add)
{
	this->yE = -1;
	this->yEAdd=add;
	this->yEP = b->yEP;
	return *yEP;
}

int Button::bindXXE(Button* b, const int add)
{
	this->xE = -1;
	this->xP= b->xP;
	this->xAdd=add;
	this->xEAdd = add;
	this->xEP = b->xEP;
	return *xEP+*xP;
}

int Button::bindYYE(Button* b, const int add)
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
