#pragma once
#include<Windows.h>
#include<gdiplus.h>
#include"bitmaps.h"
#include <vector>
#include <memory>
#include<string>
#pragma comment(lib,"gdiplus.lib")

class Gp
{
public:
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> bitmaps;
	std::vector<std::shared_ptr<Gdiplus::CachedBitmap>> Cbitmaps;
	void Load();
	void Paint(HDC hdc, int xDest, int yDest, int wDest, int hDest,Gdiplus::Image* image);
	void Paint(HDC hdc, int xDest, int yDest,Gdiplus::Image* image);
	void Paint_trans(HDC hdc, int xDest, int yDest, int wDest, int hDest, Gdiplus::Image* image);
	void Paint_trans(HDC hdc, int xDest, int yDest, Gdiplus::Image* image);
	void Paint(HDC hdc, int xDest, int yDest, int wDest, int hDest,int number);
	void Paint(HDC hdc, int xDest, int yDest,int number);
	void pPaint(HDC hdc, int xDest, int yDest, int number);
	void pPaint(HDC hdc, int xDest, int yDest, int wDest, int hDest, int number);
	void Paint_trans(HDC hdc, int xDest, int yDest, int wDest, int hDest, int number);
	void Paint_trans(HDC hdc, int xDest, int yDest, int number);
	void Prepar(HDC hdc);
	~Gp();
	Gp();
};

