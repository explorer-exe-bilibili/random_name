#pragma once
#include "Button.h"

class MenuItem
{
	Button button;
	int number;
	int star;
	std::wstring name;
	Gp* p;

public:
	MenuItem();
	MenuItem(int number, int star, std::wstring name, Gp* p);
	~MenuItem();

	void enterScreen();
	void deshow();
	void paint();
	bool click(int x, int y);

	void setStar(int star);
	void setName(std::wstring name);
	void setGp(Gp* p);
	void setNumber(int number);

	void prepare();
private:
	void setButton();
};

