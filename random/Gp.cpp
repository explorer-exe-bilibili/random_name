#include "Gp.h"

#include <filesystem>
#include "mywindows.h"
#include "explorer.h"
using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")

bool Gp::FontInfo::operator==(const FontInfo& newFont) const
{
	if (R == newFont.R && G == newFont.G && B == newFont.B && font == newFont.font)
		return 1;
	else return 0;
}

bool Gp::FontInfo::operator!=(const FontInfo& newFont) const
{
	if (R != newFont.R || G != newFont.G || B != newFont.B || font != newFont.font)
		return 1;
	else return 0;
}

Gp::FontInfo::operator bool()
{
	if (font)
		return 1;
	else return 0;
}

struct Gp::TextNeeds Gp::getTextNeeds()
{
	hdc = GetDC();
	if (!cachedGraphics)
	{
		TextNeeds.hdc_graphics = make_shared<Graphics>(hdc);
		TextNeeds.hdc_graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
		cachedGraphics = 1;
	}
	if (!cachedFont)
	{
		TextNeeds.font = make_shared<Font>(hdc, NowFontInfo.font);
		cachedFont = 1;
	}
	if (!cachedBrush)
	{
		TextNeeds.brush = make_shared<SolidBrush>(Color(255, NowFontInfo.R, NowFontInfo.G, NowFontInfo.B));
		cachedBrush = 1;
	}
	if(NowFontInfo!=LastFontInfo)
	{
		TextNeeds.brush.reset();
		TextNeeds.font.reset();
		TextNeeds.font = make_shared<Font>(hdc, NowFontInfo.font);
		TextNeeds.brush = make_shared<SolidBrush>(Color(255, NowFontInfo.R, NowFontInfo.G, NowFontInfo.B));
		LastFontInfo = NowFontInfo;
	}
	return TextNeeds;
}

void Gp::releaseTextNeeds()
{
	TextNeeds.brush.reset();
	TextNeeds.font.reset();
	TextNeeds.hdc_graphics.reset();
	cachedBrush = 0;
	cachedFont = 0;
	cachedGraphics = 0;
}

void Gp::PaintStaticItems()
{
	for (auto& i : StaticPaintList)
	{
		if(IsBadReadPtr(i.str.c_str(), sizeof(wchar_t) * i.str.length()))continue;
		if(i.str.empty())continue;
		if(i.str.size()<=0)continue;
		DrawString(i.str, i.font, i.xDest, i.yDest, i.r, i.g, i.b);
	}
}

Gp::~Gp(){
	if (cachedHDC)
		ReleaseDC(hdc);
	delete ptr;
}
Gp::Gp(HWND hwnd):hwnd(hwnd)
{
	SizeChanged();
	ptr = explorer::getInstance();
}

void Gp::Flush() {
	PaintStaticItems();
	ReleaseDC(hdc);
	releaseTextNeeds();
    // 获取窗口的设备上下文
    HDC hdcWindow = ::GetDC(hwnd);

    // 创建一个离屏DC
    HDC hdcOffscreen = CreateCompatibleDC(hdcWindow);

    // 创建一个与窗口大小相同的位图
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, buffer->GetWidth(), buffer->GetHeight());

    // 将位图选入离屏DC
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcOffscreen, hBitmap);

    // 将离屏缓冲区的内容绘制到离屏DC上
    Graphics graphicsOffscreen(hdcOffscreen);
    graphicsOffscreen.DrawImage(buffer.get(), 0, 0);

    // 将离屏DC的内容绘制到窗口上
    BitBlt(hdcWindow, 0, 0, buffer->GetWidth(), buffer->GetHeight(), hdcOffscreen, 0, 0, SRCCOPY);

    // 释放资源
    SelectObject(hdcOffscreen, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcOffscreen);

    // 释放设备上下文
    ::ReleaseDC(hwnd, hdcWindow);
}

void Gp::SizeChanged()
{
	if(cachedHDC)
		ReleaseDC(hdc);
	// 获取窗口的客户区大小
	RECT rc;
	GetClientRect(mywindows::main_hwnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	// 重新创建一个与窗口客户区大小相同的Bitmap作为离屏缓冲区
	buffer = make_shared<Bitmap>(width, height, PixelFormat32bppARGB);
	// 重新创建Graphics对象用于绘制到离屏缓冲区
	graphic = make_shared<Graphics>(buffer.get());

}

void Gp::Paint(const int xDest, const int yDest, Bitmap* image, const int wDest = 0, const int hDest = 0)
{
	ReleaseDC(hdc);
	if (wDest != 0 && hDest != 0) {
		graphic->DrawImage(image, xDest, yDest, wDest, hDest);
	}
	else {
		graphic->DrawImage(image, xDest, yDest);
	}
}
void Gp::Paint(const int xDest, const int yDest, Bitmap* image)
{
	ReleaseDC(hdc);
	graphic->DrawImage(image, xDest, yDest);
}
void Gp::Paint(int xDest, int yDest, int wDest, int hDest, int number, unsigned char alpha_count)
{
	hdc = GetDC();
	HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP* t_bitmap = ptr->getBitmap(number);
	int w = t_bitmap->bmWidth;
	int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}
void Gp::Paint(int xDest, int yDest, int number, unsigned char alpha_count)
{
	hdc = GetDC();
	HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP *t_bitmap = ptr->getBitmap(number);
	int w = t_bitmap->bmWidth;
	int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void Gp::Paint(int xDest, int yDest, int wDest, int hDest, HBITMAP hbitmap, unsigned char alpha_count)
{
	hdc = GetDC();
	HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = hbitmap;
	BITMAP t_bitmap;
	GetObject(t_hbtiamp, sizeof(BITMAP), &t_bitmap);
	int w = t_bitmap.bmWidth;
	int h = t_bitmap.bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void Gp::DrawString(const std::wstring& str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(str.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::DrawString(const std::string& str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	std::wstring wstr(str.begin(), str.end());
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::DrawStringBetween(const std::wstring& str, HFONT font, int x, int y, int xend, int yend, unsigned char R,
	unsigned char G, unsigned char B){
	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(str.c_str(), -1, TextNeeds.font.get(), layoutRect, &format, TextNeeds.brush.get());
}

void Gp::DrawstringBetween(const std::string& str, HFONT font, int x, int y, int xend, int yend, unsigned char R, unsigned char G, unsigned char B)
{
	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 绘制文本
	std::wstring wstr(str.begin(), str.end());
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), layoutRect, &format, TextNeeds.brush.get());

}

void Gp::DrawVerticalString(const std::wstring& str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 绘制每个字符
	int yOffset = y;
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawVerticalString(const std::string& str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {

	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 绘制每个字符
	int yOffset = y;
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawVerticalStringBetween(const std::wstring& str, const HFONT font, const int x, const int y, const int xend, const int yend, unsigned const char R, unsigned const char G, unsigned const char B) {
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 计算字符串的总高度
	int totalHeight = 0;
	for (const wchar_t& ch : str) {
		totalHeight += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get());
	}

	// 计算起始y偏移量，使字符串在垂直方向居中
	int yOffset = y + (yend - y - totalHeight) / 2;

	// 计算起始x偏移量，使字符串在水平方向居中
	int xOffset = x + (xend - x - TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get())) / 2;

	// 绘制每个字符
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(xOffset, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawVerticalStringBetween(const std::string& str, const HFONT font, const int x, const int y, const int xend, const int yend, unsigned const char R, unsigned const char G, unsigned const char B) {
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 计算字符串的总高度
	int totalHeight = 0;
	for (const wchar_t& ch : str) {
		totalHeight += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get());
	}

	// 计算起始y偏移量，使字符串在垂直方向居中
	int yOffset = y + (yend - y - totalHeight) / 2;

	// 计算起始x偏移量，使字符串在水平方向居中
	int xOffset = x + (xend - x - TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get())) / 2;

	// 绘制每个字符
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(xOffset, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawChar(const wchar_t ch, HFONT font, int x, int y, unsigned char R, unsigned char G, unsigned char B)
{
	std::wstring wstr(1, ch);
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::DrawChar(const char ch, HFONT font, int x, int y, unsigned char R, unsigned char G, unsigned char B)
{
	std::wstring wstr(1, ch);
	NowFontInfo = { R,G,B,font };
	auto TextNeeds = getTextNeeds();
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::AddStaticPaintItem(StaticPaintInfo Item)
{
	StaticPaintList.push_back(Item);
}

void Gp::ClearStaticPaintItem()
{
	StaticPaintList.clear();
}

HDC Gp::GetDC()
{
	if (cachedHDC)
		return hdc;
	else
	{
		cachedHDC = 1;
		return graphic->GetHDC();
	}
}

void Gp::ReleaseDC(const HDC hdc)
{
	if (cachedHDC) {
		graphic->ReleaseHDC(hdc);
		cachedHDC = 0;
	}
}

void Gp::DrawSqare(int xDest, int yDest, int xEnd, int yEnd, int R, int G, int B, bool filled)
{
	hdc = GetDC();
	if (filled) {
		// 绘制实心矩形
		HBRUSH hBrush = CreateSolidBrush(RGB(R, G, B)); // 红色实心刷子
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}
	else {
		// 绘制空心矩形
		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(R, G, B)); // 蓝色画笔
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		HBRUSH hNullBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hNullBrush);
		DeleteObject(hPen);
	}
}