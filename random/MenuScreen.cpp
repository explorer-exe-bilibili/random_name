#include "MenuScreen.h"
#include "NameScreen.h"
#include "FirstScreen.h"

#include <memory>

#include "getname.h"
#include "Gp.h"
#include "mywindows.h"
#include "ui.h"


MenuScreen::MenuScreen() = default;

MenuScreen::~MenuScreen()
{
	p = nullptr;
}

void MenuScreen::paint() const
{
	p->Paint(0, 0, mywindows::WW, mywindows::WH, listbg);
	for (auto& item : items)
	{
		item->paint();
	}
}

void MenuScreen::enter()
{
	ui::ScreenMode = MENU;
	setItems();
	explorer::getInstance()->PlayMusic("list");
	std::thread ([this]
		{
			for (const auto& item : items)
			{
				item->enterScreen();
				std::this_thread::sleep_for(std::chrono::milliseconds(150));
			}
		}).detach();
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
	Button::needFresh=true;
}

void MenuScreen::click(int x, int y) const
{
	bool ClickedItem = false;
	for (auto& item : items)
	{
		if (item->click(x, y))ClickedItem = true;
	}
	if (ClickedItem)return;
	else ui::FS->enter();
}

void MenuScreen::setGp(Gp* p)
{
	this->p = p;
	for (const auto& item : items)
	{
		item->setGp(p);
	}
}

void MenuScreen::setItems()
{
	items.clear();
	const std::vector<NameItem> showedItems = ui::NS->getShowedItems();
	for (int i = 0; i < 10; i++)
	{
		items.push_back(std::make_shared<MenuItem>(MenuItem(i, showedItems[i].star, showedItems[i].name, p)));
	}
	for(const auto& i:items)
	{
		i->prepare();
	}
}
