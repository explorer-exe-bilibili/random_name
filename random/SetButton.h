#pragma once
#include <vector>
#include <Windows.h>

#include "item.h"

class Gp;
class Button;
//class MyEditBox;

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
	HWND textBoxhwnd=nullptr;
	//MyEditBox* editBox = nullptr;
	void OpenFile() const;
	void ChooseColor_();
	void ChooseFile() const;
	void ChooseDir() const;
	static void FullScreen();
	void load();
	//static MyEditBox* CreateEditBox(HWND hWndParent, int number, const point& rect, const wchar_t* words);
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

//class MyEditBox
//{
//public:
//	MyEditBox(HWND parent, const RECT& rect, int id);
//	HWND GetHwnd() const { return hwnd; }
//	void SetText(const std::wstring& text) const;
//	std::wstring GetText() const;
//	void SetBackgroundColor(COLORREF color);
//	void Show() const;
//	void Unshow() const;
//private:
//	HWND hwnd,parent_hwnd;
//	COLORREF bgColor;
//	HBRUSH hBrush;
//	RECT rect;
//	int id;
//	static LRESULT CALLBACK EditBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//	static WNDPROC oldProc;
//};