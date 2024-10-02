#include "Screen.h"

#include "Button.h"

Screen::Screen() = default;
Screen::~Screen() = default;

void Screen::paint() const
{
	for (const auto& button : buttons)
	{
		button.paint();
	}
}

void Screen::click(int x, int y) const
{
	for (const auto& button : buttons)
	{
		button.click(CLICK, x, y);
	}
}

void Screen::resetPoint()
{
	for (auto& button : buttons)
	{
		button.refresh();
	}
}

void Screen::setGp(Gp* p)
{
	this->p = p;
	for (auto& button : buttons)
	{
		button.setGp(p);
	}
}
