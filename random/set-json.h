#pragma once
#include<windows.h>
#include<string>
#include<vector>
#include"bitmaps.h"
#include"Gp.h"
#include"button.h"
#include<nlohmann/json.hpp>
#include"item.h"
#include "SetButton.h"

#define LASTBM L"b"
#define NEXTBM L"c"

class set2
{
private:
public:
	typedef struct spage {
		int itemcount = 0;
		std::wstring Title;
		std::vector<sitem> items;
		std::vector<std::shared_ptr<SetButton>> buttons;
	}spage;
	~set2();
	BITMAP setbm, setbu;
	static bool offvideo, reran, offmusic, fullscreen, FloatWindow;
	int settingpage;
	set2();
	set2(std::string& jsonfile);
	void clicked(int x, int y);
	void paint();
	void quit();
	void reinit();
	void repaint();
	void resetplace();
	void enter();
	void seteditbox(WPARAM wParam);
	void setGp(Gp* p);
	static void rereadconfig();
private:
	Gp* p;
	int textboxnumber = 0;
	std::string G2U(const std::string& gbk);
	std::string U2G(const std::string& utf8);
	std::vector<spage> pages;
	std::vector<Button> buttons;
	void changepage();
	void init();
	void Load(std::string jsonpath);
	nlohmann::json rollback(std::string jsonpath);
	void showitem(sitem item);
	void regButton();
};
