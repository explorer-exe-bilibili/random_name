#pragma once

#include<Windows.h>
#include "Gp.h"
#include "Timer.h"

class LoadWindowPainter;
class LoadWindowExplorer;

class LoadWindow
{
	HWND hwnd;
	Timer refresh;

	void init();

public:
	LoadWindow();
	~LoadWindow();
	void create();
	void destroy();
};

class LoadWindowExplorer
{
	static LoadWindowExplorer* instance;
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> GBitmaps;
	std::vector<std::shared_ptr<HBITMAP>> HBitmaps;
	std::vector<std::shared_ptr<BITMAP>> Bitmaps;
	void Load();
public:
	LoadWindowExplorer();
	~LoadWindowExplorer();
	static LoadWindowExplorer* getInstance();
	HBITMAP getHBitmap(const int number)const;
	const BITMAP* getBitmap(const int number)const;
	const Gdiplus::Image* getGdiImage(const int number)const;
};

class LoadWindowPainter
{
	static LoadWindowPainter* instance;
	HWND hwnd;
	HDC hdc;
	bool cachedHDC = 0;
	std::shared_ptr<Gdiplus::Graphics> graphic;
	std::shared_ptr<Gdiplus::Bitmap> buffer;
	LoadWindowExplorer* ptr;
public:
	LoadWindowPainter(HWND hwnd);
	~LoadWindowPainter();
	static LoadWindowPainter* getInstance(HWND hwnd);
	static LoadWindowPainter* getInstance();
	void Flush();
	void Paint(int xDest, int yDest, int wDest, int hDest, int number);
	void Paint(int xDest, int yDest, int number);
	void DrawStringBetween(std::wstring str, HFONT font, int x, int y, int xend, int yend,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawStringBetween(std::string str, HFONT font, int x, int y, int xend, int yend,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawSqare(int xDest, int yDest, int xEnd, int yEnd, int R, int G, int B,bool filled);
	HDC GetDC();
	void ReleaseDC(HDC hdc);
};