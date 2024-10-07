#pragma once
#include <vector>
#include <Windows.h>

#include "item.h"

class Gp;
class Button;

class SetButton
{
	struct point
	{
		double x = 0;
		double y = 0;
		double xend = 0;
		double yend;
	};

	point TitleRect = {}, TextBoxRect = {};
	sitem item;
	std::vector<point>  ButtonRect;
	std::vector<Button> buttons;
	Gp * p=nullptr;
	HWND textboxHwnd = nullptr;
	void OpenFile() const;
	void ChooseFile() const;
	void ChooseDir() const;
	static void FullScreen();
	void load();
	static HWND CreateEditBox(HWND hWndParent, int number, const point& rect, const wchar_t* words);
	void EditBoxEditor(const std::wstring& tmp) const;
public:
	explicit SetButton(const sitem& Item);
	SetButton();
	~SetButton();
	void setItem(const sitem& Item);
	void show();
	void unshow();
	void setGp(Gp* p);
	void setPoint();
	void refresh();
	void reConnect();
	void click(int x, int y) const;
	void EditBoxUpgrade(int number) const;
	static bool needReran, needReboot;
};

