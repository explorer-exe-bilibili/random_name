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
	typedef struct sNode {
		int number;
		int x;
		int y;
		int xend;
		int yend;
		struct sNode* next;
	} sNode;
	typedef struct settingxy {
		int x, y, bmy, bmx, bmyend, bmxend, bmw, bmh;
	}settingxy;
public:
	typedef struct spage {
		int itemcount = 0;
		std::wstring Title;
		std::vector<sitem> items;
		std::vector<std::shared_ptr<SetButton>> buttons;
	}spage;
	~set2();
	BITMAP setbm, setbu;
	bool changebitmap[4] = { 0 };
	bool offvideo, reran, offmusic, fullscreen, FloatWindow, firstpaint = 1;
	int settingpage;
	set2();
	set2(std::string& jsonfile);
	spage getpage(int settingpage) { return pages[settingpage]; }
	void clicked(int x, int y);
	void paint();
	void quit();
	void reinit();
	void repaint();
	void rereadconfig();
	void resetplace();
	void enter();
	void seteditbox(LPARAM lParam, WPARAM wParam);
	void setGp(Gp* p);
private:
	Gp* p;
	BITMAP* bitmaps[BitmapCounts];
	bool isused[40] = { 0 },needReboot=0;
	HWND CreateEditBox(HWND hWndParent, int number, int x, int y, int w, int h, const wchar_t* words); 
	HWND textboxhwnd[20] = { 0 };
	int lastbmx = 0, lastbmy = 0, lastxend = 0, lastyend = 0;
	int nextbmx = 0, nextbmy = 0, nextxend = 0, nextyend = 0;
	int applybmx = 0, applybmy = 0, applyxend = 0, applyyend = 0;
	int textboxnumber = 0;
	sitem PictureNeedReload;
	settingxy sxy[20] = {};
	sNode* shead = nullptr;
	std::string G2U(const std::string& gbk);
	std::string U2G(const std::string& utf8);
	std::vector<spage> pages;
	std::vector<std::vector<Button>> buttons;
	void changepage();
	void ChooseFile(sitem item);
	void EditBoxEditor(sitem item,std::wstring tmp);
	void init();
	void Load(std::string jsonpath);
	void OpenFile(sitem item);
	void reloadbmp(sitem item);
	nlohmann::json rollback(std::string jsonpath);
	void showitem(sitem item);
	void switchbm(sitem item);
	void textbox(sitem item);
	void cheakLimit(sitem item);
	void regButton();
};
