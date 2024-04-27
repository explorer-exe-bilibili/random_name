#pragma once
#include<windows.h>
#include<string>
#define TEXTBOXHWNDNUMBER 17
#define BOTTONNUMBER 17


class setting
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
	static bool offvideo, reran,offmusic, fullscreen;
	static BITMAP setbm, setbu;
	static void paintsettingpage(HDC hdc, HDC hdcMem);
	static void seteditbox(LPARAM lParam, WPARAM wParam);
	static void repaint();
	static void settingkicked(int x, int y);
	static void init();
	static void quit();
	static bool changebitmap[4];
private:
	static int settingpage;
	static sNode* shead;
	static HWND textboxhwnd[TEXTBOXHWNDNUMBER];
	static int textboxnumber;
	static bool isused[TEXTBOXHWNDNUMBER + BOTTONNUMBER];
	static settingxy sxy[20];
	static void OnButtonClick(HWND hwnd, LPCWSTR title, LPCWSTR filter, int number, const wchar_t* configname);
	static void switchbm(bool OnOrOff, const wchar_t* name, int number, HDC hdc, HDC hdcMem);
	static void switchbm(const wchar_t* configname, const wchar_t* name, int number, HDC hdc, HDC hdcMem);
	static void textbox(int ebnumber, int number, std::wstring configname, LPCWSTR name, HDC hdc, HDC hdcMem, bool getfile);
	static HWND CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const wchar_t* words);
	static void changepage();
};

