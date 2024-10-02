#include "MenuItem.h"

#include "mywindows.h"
#include "ui.h"

MenuItem::MenuItem()
{
	number = 0;
	star = 0;
	p = nullptr;
}

MenuItem::MenuItem(int number, int star, std::wstring name, Gp* p)
{
	this->number = number;
	this->star = star;
	this->name = name;
	this->p = p;
}

MenuItem::~MenuItem()
{
	p = nullptr;
	number = 0;
	star = 0;

}

void MenuItem::enterScreen()
{
	int x = mywindows::WW * ((number * 0.078) + 0.107);
	int y = 0;
	int xend = x + 0.078 * mywindows::WW;
	int yend = mywindows::WH;
	button.MoveTo(x, y, xend, yend, 1, 30, 10);
}

void MenuItem::deshow()
{
	button.setDisable(1);
	button.setPoint(mywindows::WW, 0, mywindows::WW * 1.078, mywindows::WH);
}

void MenuItem::paint()
{
	button.paint();
}

bool MenuItem::click(int x, int y)
{
	int i=button.click(CLICK, x, y);
	if (i == Clicked)return 1;
	else return 0;
}

void MenuItem::setStar(int star)
{
	this->star = star;
}

void MenuItem::setName(std::wstring name)
{
	this->name = name;
}

void MenuItem::setGp(Gp* p)
{
	this->p = p;
}

void MenuItem::setNumber(int number)
{
	this->number = number;
}

void MenuItem::prepare()
{
	setButton();
	button.setDisable(0);
}

void MenuItem::setButton()
{
	button.setGp(p);
	button.setText(name);
	button.setVertical(1);
	button.setTextColor(255, 255, 255);
	button.setFont(&ui::text_list);
	button.setBmapC(star + 11);
	button.setPoint(mywindows::WW, 0, mywindows::WW * 1.078, mywindows::WH);
}
