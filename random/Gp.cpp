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
    // ��ȡ���ڵ��豸������
    HDC hdcWindow = ::GetDC(hwnd);

    // ����һ������DC
    HDC hdcOffscreen = CreateCompatibleDC(hdcWindow);

    // ����һ���봰�ڴ�С��ͬ��λͼ
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, buffer->GetWidth(), buffer->GetHeight());

    // ��λͼѡ������DC
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcOffscreen, hBitmap);

    // �����������������ݻ��Ƶ�����DC��
    Graphics graphicsOffscreen(hdcOffscreen);
    graphicsOffscreen.DrawImage(buffer.get(), 0, 0);

    // ������DC�����ݻ��Ƶ�������
    BitBlt(hdcWindow, 0, 0, buffer->GetWidth(), buffer->GetHeight(), hdcOffscreen, 0, 0, SRCCOPY);

    // �ͷ���Դ
    SelectObject(hdcOffscreen, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcOffscreen);

    // �ͷ��豸������
    ::ReleaseDC(hwnd, hdcWindow);
}

void Gp::SizeChanged()
{
	if(cachedHDC)
		ReleaseDC(hdc);
	// ��ȡ���ڵĿͻ�����С
	RECT rc;
	GetClientRect(mywindows::main_hwnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	// ���´���һ���봰�ڿͻ�����С��ͬ��Bitmap��Ϊ����������
	buffer = make_shared<Bitmap>(width, height, PixelFormat32bppARGB);
	// ���´���Graphics�������ڻ��Ƶ�����������
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
	// ���û�Ϻ���
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255; // 255 = ��͸����0 = ��ȫ͸��
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // ʹ��Դͼ���Alphaͨ��

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
	// ���û�Ϻ���
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255; // 255 = ��͸����0 = ��ȫ͸��
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // ʹ��Դͼ���Alphaͨ��

	AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void Gp::DrawString(std::wstring str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {
	if (!cachedHDC)
		hdc = GetDC();
	// ����һ��GDI+ Graphics����
	Graphics graphics(hdc);
	// �����ı���ȾģʽΪ�����
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// ����һ��GDI+ Font����
	Font gdiPlusFont(hdc, font);
	// ����һ��GDI+ SolidBrush����
	SolidBrush brush(Color(255, R, G, B));

	// �����ı�
	graphics.DrawString(str.c_str(), -1, &gdiPlusFont, PointF(x, y), &brush);
}

void Gp::DrawString(std::string str, const HFONT font, const int x, const int y, unsigned const char R, unsigned const char G, unsigned const char B) {
	if (!cachedHDC)
		hdc = GetDC();

	// ����һ��GDI+ Graphics����
	Graphics graphics(hdc);
	// �����ı���ȾģʽΪ�����
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// ����һ��GDI+ Font����
	Font gdiPlusFont(hdc, font);
	// ����һ��GDI+ SolidBrush����
	SolidBrush brush(Color(255, R, G, B));

	// �����ı�
	std::wstring wstr(str.begin(), str.end());
	graphics.DrawString(wstr.c_str(), -1, &gdiPlusFont, PointF(x, y), &brush);
}

void Gp::DrawStringBetween(std::wstring str, HFONT font, int x, int y, int xend, int yend, unsigned char R,
	unsigned char G, unsigned char B)
{
	if (!cachedHDC)
		hdc = GetDC();
	// ����һ��GDI+ Graphics����
	Graphics graphics(hdc);
	// �����ı���ȾģʽΪ�����
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// ����һ��GDI+ Font����
	Font gdiPlusFont(hdc, font);
	// ����һ��GDI+ SolidBrush����
	SolidBrush brush(Color(255, R, G, B));

	// ����һ��GDI+ RectF���󣬱�ʾ�ı����Ƶ�����
	RectF layoutRect(x, y, xend - x, yend - y);

	// ����һ��GDI+ StringFormat�������������ı���ʽ
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	// �����ı�
	graphics.DrawString(str.c_str(), -1, &gdiPlusFont, layoutRect, &format, &brush);
}

void Gp::DrawstringBetween(std::string str, HFONT font, int x, int y, int xend, int yend, unsigned char R, unsigned char G, unsigned char B)
{
	if (!cachedHDC)
		hdc = GetDC();
	// ����һ��GDI+ Graphics����
	Graphics graphics(hdc);
	// �����ı���ȾģʽΪ�����
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// ����һ��GDI+ Font����
	Font gdiPlusFont(hdc, font);
	// ����һ��GDI+ SolidBrush����
	SolidBrush brush(Color(255, R, G, B));

	// ����һ��GDI+ RectF���󣬱�ʾ�ı����Ƶ�����
	RectF layoutRect(x, y, xend - x, yend - y);

	// ����һ��GDI+ StringFormat�������������ı���ʽ
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	// �����ı�
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
