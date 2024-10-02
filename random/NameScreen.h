#pragma once
#include <memory>
#include <thread>
#include <vector>
#include<Windows.h>

#include "Button.h"
#include "getname.h"
#include "Screen.h"

#define STAR L"E"

class NameButton;
class Gp;

class NameScreen :Screen
{
	bool isBall10=false;
	bool is5star = false, is4star = false;
	int number = 0;
	int step = 0;
	NameItem items[10];
	std::thread* NameBitmapThread = nullptr;
	std::vector<HBITMAP> nameBitmaps;
	std::shared_ptr<NameButton> nameButton;
public:
	HFONT icon_star = nullptr;

	explicit NameScreen(Gp* p);
	NameScreen();
	~NameScreen() override;

	void ShowName1();
	void ShowName10();

	void paint() const override;
	void resetPoint();
	void click(int x, int y);

	void setGp(Gp* p);
private:
	void changedStep() const;
	void enter();
	void menu();
	void printStars(int star_number) const;
	void regButtons() override;
	void AddName() const;
	void quit() const override;
public:
	std::vector<NameItem> getShowedItems() const;
};
