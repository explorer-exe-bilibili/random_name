#include "Gp.h"

#include <filesystem>
#include "mywindows.h"
#include "explorer.h"
using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")

Gp::~Gp() = default;
Gp::Gp(HWND hwnd):hwnd(hwnd)
{
	SizeChanged();
	ptr = explorer::getInstance();
}

void Gp::Flush() {
	if (cachedHDC)
		ReleaseDC(hdc);
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
	if(cachedHDC)
		ReleaseDC(hdc);
	if (wDest != 0 && hDest != 0) {
		graphic.get()->DrawImage(image, xDest, yDest, wDest, hDest);
	}
	else {
		graphic.get()->DrawImage(image, xDest, yDest);
	}
}
void Gp::Paint(const int xDest, const int yDest, Bitmap* image)
{
	if (cachedHDC)
		ReleaseDC(hdc);
	graphic.get()->DrawImage(image, xDest, yDest);
}
void Gp::Paint(const int xDest, const int yDest, const int wDest, const int hDest, const int number)
{
	if (!cachedHDC)
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
	blendFunc.SourceConstantAlpha = 255; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}
void Gp::Paint(const int xDest, const int yDest, const int number)
{
	if (!cachedHDC)
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
	blendFunc.SourceConstantAlpha = 255; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void Gp::DrawString(std::wstring str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {
	if (!cachedHDC)
		hdc = GetDC();
	// 创建一个GDI+ Graphics对象
	Graphics graphics(hdc);
	// 设置文本渲染模式为抗锯齿
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// 创建一个GDI+ Font对象
	Font gdiPlusFont(hdc, font);
	// 创建一个GDI+ SolidBrush对象
	SolidBrush brush(Color(255, R, G, B));

	// 绘制文本
	graphics.DrawString(str.c_str(), -1, &gdiPlusFont, PointF(x, y), &brush);
}

void Gp::DrawString(std::string str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {
	if (!cachedHDC)
		hdc = GetDC();

	// 创建一个GDI+ Graphics对象
	Graphics graphics(hdc);
	// 设置文本渲染模式为抗锯齿
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// 创建一个GDI+ Font对象
	Font gdiPlusFont(hdc, font);
	// 创建一个GDI+ SolidBrush对象
	SolidBrush brush(Color(255, R, G, B));

	// 绘制文本
	std::wstring wstr(str.begin(), str.end());
	graphics.DrawString(wstr.c_str(), -1, &gdiPlusFont, PointF(x, y), &brush);
}

void Gp::DrawStringBetween(std::wstring str, HFONT font, int x, int y, int xend, int yend, unsigned char R,
	unsigned char G, unsigned char B)
{
	if (!cachedHDC)
		hdc = GetDC();
	// 创建一个GDI+ Graphics对象
	Graphics graphics(hdc);
	// 设置文本渲染模式为抗锯齿
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// 创建一个GDI+ Font对象
	Font gdiPlusFont(hdc, font);
	// 创建一个GDI+ SolidBrush对象
	SolidBrush brush(Color(255, R, G, B));

	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	// 绘制文本
	graphics.DrawString(str.c_str(), -1, &gdiPlusFont, layoutRect, &format, &brush);
}

void Gp::DrawstringBetween(std::string str, HFONT font, int x, int y, int xend, int yend, unsigned char R, unsigned char G, unsigned char B)
{
	if (!cachedHDC)
		hdc = GetDC();
	// 创建一个GDI+ Graphics对象
	Graphics graphics(hdc);
	// 设置文本渲染模式为抗锯齿
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// 创建一个GDI+ Font对象
	Font gdiPlusFont(hdc, font);
	// 创建一个GDI+ SolidBrush对象
	SolidBrush brush(Color(255, R, G, B));

	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	// 绘制文本
	std::wstring wstr(str.begin(), str.end());
	graphics.DrawString(wstr.c_str(), -1, &gdiPlusFont, layoutRect, &format, &brush);

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
