#pragma once
#include<Windows.h>
#include<gdiplus.h>
#include <vector>
#include <memory>
#include<string>

#include "resources_manager.h"
#pragma comment(lib,"gdiplus.lib")

class Gp
{
	HWND hwnd;
	std::shared_ptr<Gdiplus::Graphics> graphic;
	std::shared_ptr<Gdiplus::Bitmap> buffer;
	std::vector<HBITMAP> HBitmaps;
	std::vector<BITMAP> BITMAPs;
	void Load_();
	void Load();
	resources_manager *ptr;
public:
	~Gp();
	Gp(HWND hwnd);
	void Flush();
	void SizeChanged();
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> bitmaps;
	void Paint(int xDest, int yDest, Gdiplus::Bitmap* image);
	void Paint(int xDest, int yDest, Gdiplus::Bitmap* image, int wDest, int hDest);
	void Paint(int xDest, int yDest, int wDest, int hDest,int number);
	void Paint(int xDest, int yDest,int number);
	void DrawString(std::wstring str, HFONT font, int x, int y, int R=255, int G = 255, int B = 255);
	void DrawString(std::string str, HFONT font, int x, int y, int R = 255, int G = 255, int B = 255);
	HDC GetDC();
	void ReleaseDC(HDC hdc);
};