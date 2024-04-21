#pragma once
#include<windows.h>
#define TEXTBOXHWNDNUMBER 10
#define BOTTONNUMBER 10
#define FIRST_MENU 1
#define SETTING 2
#define SHOW_NAMES_ING 3

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
};

class setting
{
public:
	static int exitx, exitxend, exity, exityend, settingpage,screenmode;
	static bool musicplayed, offvideo, reran, initing;
	static BITMAP exitbm, setbm_, setbu;
	static void paintsettingpage(HDC hdc, HDC hdcMem, HFONT text_mid);
	static void seteditbox(LPARAM lParam, WPARAM wParam);
	static void settingkicked(int x, int y);
	static void init();
	static void quit();
private:
	static sNode* shead;
	static HWND textboxhwnd[TEXTBOXHWNDNUMBER];
	static int textboxnumber;
	static bool isused[TEXTBOXHWNDNUMBER + BOTTONNUMBER];
	static HWND bottomhwnd[BOTTONNUMBER];
	static settingxy sxy[21];
	static void OnButtonClick(HWND hwnd, LPCWSTR title, LPCWSTR filter, int number, const char* configname);
	static void switchbm(bool OnOrOff, const wchar_t* name, int number, HDC hdc, HDC hdcMem, HFONT text_mid);
	static void textbox(int ebnumber, int number, const char* configname, LPCWSTR name, HDC hdc, HDC hdcMem, HFONT text_mid, bool getfile);
};

