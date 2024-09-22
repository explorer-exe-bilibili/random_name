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
	~MenuItem();

	void enterScreen()const;
	void deshow()const;

	void setStar(int star);
	void setName(std::wstring name);
	void setGp(Gp* p);
	void setNumber(int number);

	void prepare();
private:
	void setButton();
};

