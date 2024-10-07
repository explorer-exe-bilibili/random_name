#pragma once
#include<Windows.h>
#include<gdiplus.h>
#include <memory>
#include<string>
#include <mutex>

#include "explorer.h"
#pragma comment(lib,"gdiplus.lib")

class Gp
{
public:
	struct StaticPaintInfo
	{
		int xDest, yDest;
		unsigned char r, g, b;
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
		unsigned char R = 255, G = 255, B = 255;
		HFONT font = 0;
		bool operator==(const FontInfo&) const;
		bool operator!=(const FontInfo&) const;
		operator bool() const;
	}NowFontInfo, LastFontInfo;
	HWND hwnd;
	HDC hdc;
	bool cachedHDC = 0, cachedGraphics = 0, cachedFont = 0, cachedBrush = 0;
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
	void DrawString(const std::wstring& str, HFONT font, int x, int y,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawString(const std::string& str, HFONT font, int x, int y,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawStringBetween(const std::wstring& str, HFONT font, int x, int y, int xend, int yend,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawstringBetween(const std::string& str, HFONT font, int x, int y, int xend, int yend,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawVerticalString(const std::wstring& str, HFONT font, int x, int y,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawVerticalString(const std::string& str, HFONT font, int x, int y,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawVerticalStringBetween(const std::wstring& str, HFONT font, int x, int y, int xend, int yend,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawVerticalStringBetween(const std::string& str, HFONT font, int x, int y, int xend, int yend,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawChar(wchar_t ch, HFONT font, int x, int y,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void DrawChar(char ch, HFONT font, int x, int y,
		unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
	void AddStaticPaintItem(const StaticPaintInfo& Item);
	void ClearStaticPaintItem();
	HDC GetDC();
	void ReleaseDC(HDC hdc);
	void DrawSquare(int xDest, int yDest, int xEnd, int yEnd, int R, int G, int B, bool filled);
	void LockWindowSize() const;
	void UnlockWindowSize() const;
};