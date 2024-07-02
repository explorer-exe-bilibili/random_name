#pragma once
#include<Windows.h>
#include<gdiplus.h>
#include <vector>
#include <memory>
#include<string>
#pragma comment(lib,"gdiplus.lib")

class Gp
{
	std::shared_ptr<Gdiplus::Graphics> graphic;
	void Load_();
	void Load();
public:
	~Gp();
	Gp(HWND hwnd);
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> bitmaps;
	void Paint(int xDest, int yDest, Gdiplus::Bitmap* image);
	void Paint(int xDest, int yDest, Gdiplus::Bitmap* image, int wDest, int hDest);
	void Paint(int xDest, int yDest, int wDest, int hDest,int number);
	void Paint(int xDest, int yDest,int number);
	void DrawString(std::wstring str, HFONT font, int x, int y);
	void DrawString(std::string str, HFONT font, int x, int y);
	HDC GetDC();
	void ReleaseDC(HDC hdc);
};