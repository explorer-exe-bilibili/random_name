#include "Gp.h"

#include <filesystem>
#include "mywindows.h"
#include "explorer.h"
#include "EasyGL/EasyGL.h"
#include "EasyGL/My_Font.h"
//using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")

mutex StaticItemMutex;

void Gp::PaintStaticItems()
{
	lock_guard<mutex> lock(StaticItemMutex);
	for (auto& i : StaticPaintList)
	{
		if (IsBadReadPtr(i.str.c_str(), sizeof(wchar_t) * i.str.length()))continue;
		if (i.str.empty())continue;
		DrawString(i.str, i.font, i.size, i.xDest, i.yDest, i.argb);
	}
}

Gp::~Gp(){
	delete ptr;
}
Gp::Gp(HWND hwnd):hwnd(hwnd){
	SizeChanged();
	ptr = explorer::getInstance();
}

void Gp::Flush()
{
	PaintStaticItems();
	easyGL.EndRender();
}

void Gp::SizeChanged()
{
	// 获取窗口的客户区大小
	RECT rc;
	GetClientRect(mywindows::main_hwnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	easyGL.Resize(width, height);
}

void Gp::Paint(const int xDest, const int yDest, Bitmap* image, const int wDest = 0, const int hDest = 0) const
{
	easyGL.DrawBitmap(xDest, yDest, wDest, hDest, *image);
}
void Gp::Paint(const int xDest, const int yDest, Bitmap* image) const
{
	easyGL.DrawBitmap(xDest, yDest, *image);
}
void Gp::Paint(int xDest, int yDest, int wDest, int hDest, int number, unsigned char alpha_count) const
{
	easyGL.DrawBitmap(xDest, yDest, wDest, hDest, *ptr->getBitmap(number));
}
void Gp::Paint(int xDest, int yDest, int number, unsigned char alpha_count) const
{
	easyGL.DrawBitmap(xDest, yDest, *ptr->getBitmap(number));
}

void Gp::DrawString(const std::wstring& str, Font& font, int size, int x, int y, uint32_t argb) const
{
	easyGL.DrawString(str, font, x, y, size, color(argb));
}

void Gp::DrawString(const std::string& str, Font& font, int size, int x, int y, uint32_t argb) const
{
	easyGL.DrawString(str, font, x, y, size, color(argb));
}

void Gp::DrawStringBetween(const std::wstring& str, Font& font, int size, int x, int y, int xend, int yend, uint32_t argb) const
{
	easyGL.DrawStringBetween(str, font, x, y, xend, yend, size, color(argb));
}

void Gp::DrawstringBetween(const std::string& str, Font& font, int size, int x, int y, int xend, int yend, uint32_t argb) const
{
	easyGL.DrawStringBetween(str, font, x, y, xend, yend, size, color(argb));
}

void Gp::DrawVerticalString(const std::wstring& str, Font& font, int size, int x, int y, uint32_t argb) const
{
	easyGL.DrawVerticalString(str, font, x, y, size, color(argb));
}

void Gp::DrawVerticalString(const std::string& str, Font& font, int size, int x, int y, uint32_t argb) const
{
	easyGL.DrawVerticalString(str, font, x, y, size, color(argb));
}

void Gp::DrawVerticalStringBetween(const std::wstring& str, Font& font, int size, int x, int y, int xend, int yend, uint32_t argb) const
{
	easyGL.DrawVerticalStringBetween(str, font, x, y, xend, yend, size, color(argb));
}

void Gp::DrawVerticalStringBetween(const std::string& str, Font& font, int size, int x, int y, int xend, int yend, uint32_t argb) const
{
	easyGL.DrawVerticalStringBetween(str, font, x, y, xend, yend, size, color(argb));
}

void Gp::DrawChar(wchar_t ch, Font& font, int size, int x, int y, uint32_t argb)
{
	float scale=size/ font.GetFontSize();
	font.RendChar(ch, x, y, scale, color(argb));
}

void Gp::DrawChar(char ch, Font& font, int size, int x, int y, uint32_t argb)
{
	float scale = size / font.GetFontSize();
	font.RendChar(ch, x, y, scale, color(argb));
}

void Gp::AddStaticPaintItem(const StaticPaintInfo& Item)
{
	StaticPaintList.push_back(Item);
}

void Gp::ClearStaticPaintItem()
{
	StaticPaintList.clear();
}

void Gp::DrawSquare(int xDest, int yDest, int xEnd, int yEnd, uint32_t Color, bool filled) const
{
	easyGL.DrawRectangle(xDest, yDest, xEnd, yEnd, color(Color), filled);
}

void Gp::BeginPaint() const
{
	easyGL.BeginRender();
}

void Gp::initOpenGL(HWND hwnd)
{
	if (!isInitOpenGL)
	easyGL.Init(hwnd);
	else
	{
		easyGL.Cleanup();
		easyGL.Init(hwnd);
	}
}

void Gp::CleanOpenGL()
{
	easyGL.Cleanup();
}

void Gp::Resize()
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	easyGL.Resize(width, height);
}

