#pragma once
#include<windows.h>
#include<string>
#include<vector>
#include"Gp.h"
#include"button.h"
#include<nlohmann/json.hpp>
#include"item.h"
#include "SetButton.h"

class set2
{
	struct spage {
		int itemcount = 0;
		std::wstring Title;
		std::vector<sitem> items;
		std::vector<std::shared_ptr<SetButton>> buttons;
	};
	Gp* p;
	std::vector<spage> pages;
	std::vector<Button> buttons;
public:
	static bool offvideo, reran, offmusic, fullscreen, FloatWindow;
	int settingpage;
	set2();
	~set2();
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
	std::string G2U(const std::string& gbk);
	std::string U2G(const std::string& utf8);
	nlohmann::json rollback(std::string jsonpath);
	void changepage();
	void init();
	void Load(std::string jsonpath);
	void showitem(sitem item);
	void regButton();
};
