#pragma once
#include<Windows.h>
#include<gdiplus.h>
#include <memory>
#include<string>
#include <mutex>

#include "explorer.h"
#pragma comment(lib,"gdiplus.lib")

#define ARGB(a,r,g,b)	((uint32_t)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)|(((DWORD)(BYTE)(a))<<24)))


class Gp
{
public:
	struct StaticPaintInfo
	{
		int xDest, yDest;
		uint32_t argb;
		std::wstring str;
		HFONT font;
	};
private:
	struct TextNeeds
	{
		std::shared_ptr<Gdiplus::Graphics> hdc_graphics;
		std::shared_ptr<Gdiplus::Font> font;
		std::shared_ptr<Gdiplus::SolidBrush> brush;
	}TextNeeds;

	struct FontInfo
	{
		uint32_t argb = 0xFFFFFFFF;
		HFONT font = nullptr;
		bool operator==(const FontInfo&) const;
		bool operator!=(const FontInfo&) const;
		operator bool() const;
	}NowFontInfo, LastFontInfo;
	HWND hwnd;
	HDC hdc;
	bool cachedHDC = false, cachedGraphics = false, cachedFont = false, cachedBrush = false;
	std::shared_ptr<Gdiplus::Graphics> graphic;
	std::shared_ptr<Gdiplus::Bitmap> buffer;
	explorer* ptr;
	std::mutex SizeChangeMutex;
	std::vector<StaticPaintInfo> StaticPaintList;
	struct TextNeeds getTextNeeds();
	void releaseTextNeeds();
	void PaintStaticItems();
public:
	~Gp();
	Gp(HWND hwnd);
	void Flush();
	void SizeChanged();
	void Paint(int xDest, int yDest, Gdiplus::Bitmap* image);
	void Paint(int xDest, int yDest, Gdiplus::Bitmap* image, int wDest, int hDest);
	void Paint(int xDest, int yDest, int wDest, int hDest, int number, unsigned char alpha_count = 255);
	void Paint(int xDest, int yDest, int number, unsigned char alpha_count = 255);
	void Paint(int xDest, int yDest, int wDest, int hDest,
		HBITMAP hbitmap, unsigned char alpha_count = 255);
	void DrawString(const std::wstring& str, HFONT font, int x, int y,uint32_t argb=0xFFFFFFFF);
	void DrawString(const std::string& str, HFONT font, int x, int y,uint32_t argb=0xFFFFFFFF);
	void DrawStringBetween(const std::wstring& str, HFONT font, int x, int y, int xend, int yend,uint32_t argb=0xFFFFFFFF);
	void DrawstringBetween(const std::string& str, HFONT font, int x, int y, int xend, int yend,uint32_t argb=0xFFFFFFFF);
	void DrawVerticalString(const std::wstring& str, HFONT font, int x, int y,uint32_t argb=0xFFFFFFFF);
	void DrawVerticalString(const std::string& str, HFONT font, int x, int y,uint32_t argb=0xFFFFFFFF);
	void DrawVerticalStringBetween(const std::wstring& str, HFONT font, int x
		, int y, int xend, int yend,uint32_t argb=0xFFFFFFFF);
	void DrawVerticalStringBetween(const std::string& str, HFONT font, int x,
		int y, int xend, int yend,uint32_t argb=0xFFFFFFFF);
	void DrawChar(wchar_t ch, HFONT font, int x, int y,uint32_t argb=0xFFFFFFFF);
	void DrawChar(char ch, HFONT font, int x, int y,uint32_t argb=0xFFFFFFFF);
	void AddStaticPaintItem(const StaticPaintInfo& Item);
	void ClearStaticPaintItem();
	HDC GetDC();
	void ReleaseDC(HDC hdc);
	void DrawSquare(int xDest, int yDest, int xEnd, int yEnd, uint32_t color, bool filled);
	void LockWindowSize() const;
	void UnlockWindowSize() const;
};