#pragma once
#include <vector>

#include "Button.h"


class Gp;

class Screen
{
protected:
	std::vector<Button> buttons;
	Gp* p = nullptr;
public:
	Screen();
	virtual ~Screen();

	virtual void paint()const;
	void click(int x, int y) const;
	void resetPoint();

	virtual void enter() const{}
	virtual void quit() const{}

	void setGp(Gp* p);

protected:
	virtual void regButtons(){}
};

