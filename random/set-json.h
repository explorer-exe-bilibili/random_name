#pragma once
#include<windows.h>
#include<string>
#include<vector>
#include"log.h"
#include"bitmaps.h"

#define LASTBM L"b"
#define NEXTBM L"c"

class set2
{
private:
	typedef struct sitem {
		bool IsEditBox = 0;
		bool IsFile = 0;
		bool IsSwitch = 0;
		int Number = 0;
		int max = 0;
		int min = 0;
		int Limit = 0;
		int BitmapNumber = 0;
		std::wstring Name;
		std::wstring ConfigName;
		std::wstring FileChooseWindowName;
		std::string FileType;
		std::string OutOfLimitOutPut;
	}sitem;

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
	void paint(HDC hdc, HDC hdcMem);
	void quit();
	void reinit();
	void release();
	void repaint();
	void rereadconfig();
	void resetplace();
	void seteditbox(LPARAM lParam, WPARAM wParam);
private:
	BITMAP* bitmaps[BitmapCounts];
	bool isused[40] = { 0 };
	HWND CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const wchar_t* words);
	HWND textboxhwnd[20] = { 0 };
	int lastbmx=0, lastbmy=0, lastxend=0, lastyend=0;
	int nextbmx=0, nextbmy=0, nextxend=0, nextyend=0;
	int textboxnumber = 0;
	settingxy sxy[20] = {};
	sNode* shead = nullptr;
	std::string G2U(const std::string& gbk);
	std::string U2G(const std::string& utf8);
	std::vector<spage> pages;
	void changepage();
	void ChooseFile(sitem item);
	void EditBoxEditor(sitem item,std::wstring tmp);
	void init();
	void Load(std::string jsonpath);
	void OpenFile(sitem item);
	void reloadbmp(sitem item);
	void rollback(std::string jsonpath);
	void showitem(sitem item, HDC hdc, HDC hdcMem);
	void switchbm(sitem item, HDC hdc, HDC hdcMem);
	void textbox(sitem item, HDC hdc, HDC hdcMem);
};
