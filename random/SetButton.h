#pragma once
#include "Button.h"
#include "item.h"
class SetButton
{
private:
	struct point
	{
		double x = 0;
		double y = 0;
		double xend = 0;
		double yend;
	};

	point TitleRect = {}, TextBoxRext = {};
	sitem item;
	std::vector<point>  ButtonRect;
	std::vector<Button> buttons;
	Gp * p=nullptr;
	HWND textboxhwnd = NULL;
	static bool reran;
	void OpenFile(sitem item);
	void ChooseFile(sitem item);
	void load();
	HWND CreateEditBox(HWND hWndParent, int number, point rect, const wchar_t* words);
	void EditBoxEditor(sitem item, std::wstring tmp);
public:
	SetButton(sitem Item);
	SetButton();
	~SetButton();
	void setItem(sitem Item);
	void show();
	void unshow();
	void setGp(Gp* p);
	void setPoint();
	void refresh();
	void reConnect();
	void click(int x, int y);
};

