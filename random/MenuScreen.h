#pragma once
#include"menuItem.h"

class MenuScreen
{
	Gp* p;
	std::vector<std::shared_ptr<MenuItem>> items;
public:
	MenuScreen();
	~MenuScreen();

	void paint() const;
	void enter();
	void click(int x, int y) const;
	void setGp(Gp* p);
private:
	void setItems();
};