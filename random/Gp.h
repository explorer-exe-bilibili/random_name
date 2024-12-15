#pragma once
#include<Windows.h>
#include<string>
#include <mutex>

#include "explorer.h"
#include "EasyGL/My_Font.h"

#define ARGB(a,r,g,b)	((uint32_t)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)|(((DWORD)(BYTE)(a))<<24)))


class My_Font;

class Gp
{
public:
	struct StaticPaintInfo
	{
		int xDest, yDest, size;
		uint32_t argb;
		std::wstring str;
		Font font;
	};
private:
	HWND hwnd;
	explorer* ptr;
	std::vector<StaticPaintInfo> StaticPaintList;
	void PaintStaticItems();
public:
	~Gp();
	Gp(HWND hwnd);
	void Flush();
	void SizeChanged();
	void Paint(int xDest, int yDest, Bitmap* image) const;
	void Paint(int xDest, int yDest, Bitmap* image, int wDest, int hDest) const;
	void Paint(int xDest, int yDest, int wDest, int hDest, int number, unsigned char alpha_count = 255) const;
	void Paint(int xDest, int yDest, int number, unsigned char alpha_count = 255) const;
	void DrawString(const std::wstring& str, Font& font,int size, int x, int y, uint32_t argb = 0xFFFFFFFF) const;
	void DrawString(const std::string& str, Font& font, int size, int x, int y, uint32_t argb = 0xFFFFFFFF) const;
	void DrawStringBetween(const std::wstring& str, Font& font, int size, int x, int y,
		int xend, int yend, uint32_t argb = 0xFFFFFFFF) const;
	void DrawstringBetween(const std::string& str, Font& font, int size, int x, int y, 
		int xend, int yend, uint32_t argb = 0xFFFFFFFF) const;
	void DrawVerticalString(const std::wstring& str, Font& font, int size, int x, int y, uint32_t argb = 0xFFFFFFFF) const;
	void DrawVerticalString(const std::string& str, Font& font, int size, int x, int y, uint32_t argb = 0xFFFFFFFF) const;
	void DrawVerticalStringBetween(const std::wstring& str, Font& font, int size, int x
	                               , int y, int xend, int yend, uint32_t argb = 0xFFFFFFFF) const;
	void DrawVerticalStringBetween(const std::string& str, Font& font, int size, int x,
	                               int y, int xend, int yend, uint32_t argb = 0xFFFFFFFF) const;
	static void DrawChar(wchar_t ch, Font& font, int size, int x, int y, uint32_t argb = 0xFFFFFFFF);
	static void DrawChar(char ch, Font& font, int size, int x, int y, uint32_t argb = 0xFFFFFFFF);
	void AddStaticPaintItem(const StaticPaintInfo& Item);
	void ClearStaticPaintItem();
	void DrawSquare(int xDest, int yDest, int xEnd, int yEnd, uint32_t color, bool filled) const;

	void BeginPaint() const;

	void initOpenGL(HWND hwnd);
	void CleanOpenGL();
	void Resize();

private:
	EasyGL easyGL;
	bool isInitOpenGL = false;

	void renderBitmapString(float x, float y, void* font, const char* string);

};
