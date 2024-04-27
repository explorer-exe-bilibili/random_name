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
		std::wstring FileType;
		std::wstring OutOfLimitOutPut;
	}sitem;
	typedef struct spage {
		int itemcount = 0;
		std::wstring Title;
		std::vector<sitem> items;
	}spage;
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

	int settingpage;
	bool offvideo, reran, offmusic, fullscreen, firstpaint;
	BITMAP setbm, setbu;
	set2(std::string& jsonfile);
	set2();
	~set2();
	void paint(HDC hdc, HDC hdcMem);
	void seteditbox(LPARAM lParam, WPARAM wParam);
	void repaint();
	void clicked(int x, int y);
	void resetplace();
	void quit();
	void rereadconfig();
	bool changebitmap[4] = { 0 };
private:
	BITMAP* bitmaps[BitmapCounts];
	int nextbmx=0, nextbmy=0, nextxend=0, nextyend=0;
	int lastbmx=0, lastbmy=0, lastxend=0, lastyend=0;
	sNode* shead = nullptr;
	HWND textboxhwnd[20] = { 0 };
	int textboxnumber = 0;
	bool isused[40] = { 0 };
	std::vector<spage> pages;
	void editboxeditor(sitem item,std::wstring tmp);
	void ChooseFile(HWND hwnd, sitem item);
	void switchbm(sitem item, HDC hdc, HDC hdcMem);
	void textbox(sitem item, HDC hdc, HDC hdcMem);
	HWND CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const wchar_t* words);
	settingxy sxy[20] = {};
	void rollback(std::string jsonpath);
	void showitem(sitem item, HDC hdc, HDC hdcMem);
	void changepage();
	void reloadbmp(sitem item);
};