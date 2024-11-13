#include "MenuItem.h"

#include "explorer.h"
#include "Gp.h"
#include "mywindows.h"
#include "ui.h"

MenuItem::MenuItem()
{
	number = 0;
	star = 0;
	p = nullptr;
}

MenuItem::MenuItem(int number, int star, const std::wstring& name, Gp* p)
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
	button.MoveTo(x, y, xend, yend, true, 30, 10);
	std::thread ([x, y, xend, yend, this]
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(150));
			button.MoveTo(x, y, xend, yend, true, 30, 10);
		}).detach();
}

void MenuItem::deshow()
{
	button.setDisable(true);
	button.setPoint(mywindows::WW, 0, mywindows::WW * 1.078, mywindows::WH);
}

void MenuItem::paint() const
{
	button.paint();
}

bool MenuItem::click(int x, int y) const
{
	int i=button.click(CLICK, x, y);
	if (i == Clicked)return true;
	else return false;
}

void MenuItem::setStar(int star)
{
	this->star = star;
}

void MenuItem::setName(const std::wstring& name)
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
	button.setDisable(false);
}

void MenuItem::setButton()
{
	button.setGp(p);
	button.setText(name);
	button.setVertical(true);
	button.setTextColor(ARGB(255, 255, 255, 255));
	button.setFont(&ui::text_list);
	button.setBmapC(star + 11);
	button.setPoint(mywindows::WW, 0, mywindows::WW * 1.078, mywindows::WH);
}
