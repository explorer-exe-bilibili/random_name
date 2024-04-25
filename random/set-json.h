#pragma once
#include<windows.h>
#include<string>
#include<vector>
#include"log.h"

class set2
{
private:
	typedef struct sitem {
		bool IsEditBox, HaveChooseFileBM, IsSwitchBM;
		std::wstring Name, ConfigName, FileChooseWindowName, FileType, OutOfLimitOutPut;
		int number, max, min, limit;
	}sitem;
	typedef struct spage {
		int itemcount;
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
	bool musicplayed, offvideo, reran, initing, offmusic, fullscreen, firstpaint;
	BITMAP exitbm, setbm_, setbu;
	set2(std::string& jsonfile);
	set2();
	~set2();
	void paint(HDC hdc, HDC hdcMem);
	void seteditbox(LPARAM lParam, WPARAM wParam);
	void repaint();
	void settingkicked(int x, int y);
	void resetplace();
	void quit();
	void rereadconfig();
	bool changebitmap[4] = { 0 };
private:
	sNode* shead = nullptr;
	HWND textboxhwnd[20] = { 0 };
	int textboxnumber = 0;
	bool isused[40] = { 0 };
	std::vector<spage> pages;
	void editboxeditor(sitem item,std::wstring tmp);
	void OnButtonClick(HWND hwnd, sitem item);
	void switchbm(sitem item, HDC hdc, HDC hdcMem);
	void textbox(sitem item, HDC hdc, HDC hdcMem);
	HWND CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const wchar_t* words);
	settingxy sxy[20] = {};
	void rollback(std::string jsonpath);
};
