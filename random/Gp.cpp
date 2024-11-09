#include "Gp.h"

#include <filesystem>
#include "mywindows.h"
#include "explorer.h"
using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")

bool Gp::FontInfo::operator==(const FontInfo& newFont) const
{
	if (argb == newFont.argb && font == newFont.font)
		return true;
	else return false;
}

bool Gp::FontInfo::operator!=(const FontInfo& newFont) const
{
	if (argb!=newFont.argb || font != newFont.font)
		return true;
	else return false;
}

Gp::FontInfo::operator bool() const
{
	if (font)
		return true;
	else return false;
}

struct Gp::TextNeeds Gp::getTextNeeds()
{
	hdc = GetDC();
	if (!cachedGraphics)
	{
		TextNeeds.hdc_graphics = make_shared<Graphics>(hdc);
		TextNeeds.hdc_graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
		cachedGraphics = true;
	}
	if (!cachedFont)
	{
		TextNeeds.font = make_shared<Font>(hdc, NowFontInfo.font);
		cachedFont = true;
	}
	if (!cachedBrush)
	{
		TextNeeds.brush = make_shared<SolidBrush>(Color(NowFontInfo.argb));
		cachedBrush = true;
	}
	if(NowFontInfo!=LastFontInfo)
	{
		TextNeeds.brush.reset();
		TextNeeds.font.reset();
		TextNeeds.font = make_shared<Font>(hdc, NowFontInfo.font);
		TextNeeds.brush = make_shared<SolidBrush>(Color(NowFontInfo.argb));
		
		LastFontInfo = NowFontInfo;
	}
	return TextNeeds;
}

void Gp::releaseTextNeeds()
{
	TextNeeds.brush.reset();
	TextNeeds.font.reset();
	TextNeeds.hdc_graphics.reset();
	cachedBrush = false;
	cachedFont = false;
	cachedGraphics = false;
}

void Gp::PaintStaticItems()
{
	std::lock_guard lock(SizeChangeMutex);
	for (auto& i : StaticPaintList)
	{
		if (IsBadReadPtr(i.str.c_str(), sizeof(wchar_t) * i.str.length()))continue;
		if (i.str.empty())continue;
		DrawString(i.str, i.font, i.xDest, i.yDest, i.argb);
	}
}

Gp::~Gp(){
	if (cachedHDC)
		ReleaseDC(hdc);
	delete ptr;
}
Gp::Gp(HWND hwnd):hwnd(hwnd){
	SizeChanged();
	ptr = explorer::getInstance();
}

void Gp::Flush() {
	PaintStaticItems();
	ReleaseDC(hdc);
	releaseTextNeeds();
    // ��ȡ���ڵ��豸������
	const HDC hdcWindow = ::GetDC(hwnd);

    // ����һ������DC
	const HDC hdcOffscreen = CreateCompatibleDC(hdcWindow);

    // ����һ���봰�ڴ�С��ͬ��λͼ
	const HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, buffer->GetWidth(), buffer->GetHeight());

    // ��λͼѡ������DC
	const HBITMAP hOldBitmap = HBITMAP(SelectObject(hdcOffscreen, hBitmap));

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
	std::lock_guard lock(SizeChangeMutex);
	if (cachedHDC)
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
	releaseTextNeeds();
	cachedBrush = false;
	cachedFont = false;
	cachedGraphics = false;
	cachedHDC = false;
	StaticPaintList.clear();
}

void Gp::Paint(const int xDest, const int yDest, Bitmap* image, const int wDest = 0, const int hDest = 0)
{
	std::lock_guard lock(SizeChangeMutex);
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
	std::lock_guard lock(SizeChangeMutex);
	ReleaseDC(hdc);
	graphic->DrawImage(image, xDest, yDest);
}
void Gp::Paint(int xDest, int yDest, int wDest, int hDest, int number, unsigned char alpha_count)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	const HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP* t_bitmap = ptr->getBitmap(number);
	const int w = t_bitmap->bmWidth;
	const int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// ���û�Ϻ���
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = ��͸����0 = ��ȫ͸��
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // ʹ��Դͼ���Alphaͨ��

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}
void Gp::Paint(int xDest, int yDest, int number, unsigned char alpha_count)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	const HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP *t_bitmap = ptr->getBitmap(number);
	const int w = t_bitmap->bmWidth;
	const int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// ���û�Ϻ���
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = ��͸����0 = ��ȫ͸��
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // ʹ��Դͼ���Alphaͨ��

	AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void Gp::Paint(int xDest, int yDest, int wDest, int hDest, HBITMAP hbitmap, unsigned char alpha_count)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	const HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = hbitmap;
	BITMAP t_bitmap;
	GetObject(t_hbtiamp, sizeof(BITMAP), &t_bitmap);
	const int w = t_bitmap.bmWidth;
	const int h = t_bitmap.bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// ���û�Ϻ���
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = ��͸����0 = ��ȫ͸��
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // ʹ��Դͼ���Alphaͨ��

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void Gp::DrawString(const std::wstring& str, const HFONT font, const int x, const int y,uint32_t argb) {
	//std::lock_guard lock(SizeChangeMutex);
	if (!font)return;
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// �����ı�
	wstring wstr = str;
	if (wstr.empty())wstr = L" ";
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::DrawString(const std::string& str, const HFONT font, const int x, const int y,uint32_t argb) {
	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	const std::wstring wstr(str.begin(), str.end());
	// �����ı�
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::DrawStringBetween(const std::wstring& str, HFONT font, int x, int y, int xend, int yend, uint32_t argb){
	// ����һ��GDI+ RectF���󣬱�ʾ�ı����Ƶ�����
	std::lock_guard lock(SizeChangeMutex);
	const RectF layoutRect(x, y, xend - x, yend - y);

	// ����һ��GDI+ StringFormat�������������ı���ʽ
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// �����ı�
	TextNeeds.hdc_graphics->DrawString(str.c_str(), -1, TextNeeds.font.get(), layoutRect, &format, TextNeeds.brush.get());
}

void Gp::DrawstringBetween(const std::string& str, HFONT font, int x, int y, int xend, int yend,uint32_t argb)
{
	std::lock_guard lock(SizeChangeMutex);
	// ����һ��GDI+ RectF���󣬱�ʾ�ı����Ƶ�����
	const RectF layoutRect(x, y, xend - x, yend - y);

	// ����һ��GDI+ StringFormat�������������ı���ʽ
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// �����ı�
	const std::wstring wstr(str.begin(), str.end());
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), layoutRect, &format, TextNeeds.brush.get());

}

void Gp::DrawVerticalString(const std::wstring& str, const HFONT font, const int x, const int y,uint32_t argb) {
	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// ����ÿ���ַ�
	int yOffset = y;
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // ����yƫ����
	}
}

void Gp::DrawVerticalString(const std::string& str, const HFONT font, const int x, const int y,uint32_t argb) {

	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// ����ÿ���ַ�
	int yOffset = y;
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // ����yƫ����
	}
}

void Gp::DrawVerticalStringBetween(const std::wstring& str, const HFONT font, const int x, const int y, const int xend, const int yend,uint32_t argb) {
	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// �����ַ������ܸ߶�
	int totalHeight = 0;
	for (const wchar_t& ch : str) {
		totalHeight += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get());
	}

	// ������ʼyƫ������ʹ�ַ����ڴ�ֱ�������
	int yOffset = y + (yend - y - totalHeight) / 2;

	// ������ʼxƫ������ʹ�ַ�����ˮƽ�������
	const int xOffset = x + (xend - x - TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get())) / 2;

	// ����ÿ���ַ�
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(),
			PointF(xOffset, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // ����yƫ����
	}
}

void Gp::DrawVerticalStringBetween(const std::string& str, const HFONT font, const int x, const int y, const int xend, const int yend,uint32_t argb) {
	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// �����ַ������ܸ߶�
	int totalHeight = 0;
	for (const wchar_t& ch : str) {
		totalHeight += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get());
	}

	// ������ʼyƫ������ʹ�ַ����ڴ�ֱ�������
	int yOffset = y + (yend - y - totalHeight) / 2;

	// ������ʼxƫ������ʹ�ַ�����ˮƽ�������
	const int xOffset = x + (xend - x - TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get())) / 2;

	// ����ÿ���ַ�
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), 
			PointF(xOffset, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // ����yƫ����
	}
}

void Gp::DrawChar(const wchar_t ch, HFONT font, int x, int y,uint32_t argb)
{
	std::lock_guard lock(SizeChangeMutex);
	const std::wstring wstr(1, ch);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// �����ı�
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::DrawChar(const char ch, HFONT font, int x, int y,uint32_t argb)
{
	std::lock_guard lock(SizeChangeMutex);
	const std::wstring wstr(1, ch);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// �����ı�
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::AddStaticPaintItem(const StaticPaintInfo& Item)
{
	std::lock_guard lock(SizeChangeMutex);
	StaticPaintList.push_back(Item);
}

void Gp::ClearStaticPaintItem()
{
	std::lock_guard lock(SizeChangeMutex);
	StaticPaintList.clear();
}

HDC Gp::GetDC()
{
	if (cachedHDC)
		return hdc;
	else
	{
		cachedHDC = true;
		return graphic->GetHDC();
	}
}

void Gp::ReleaseDC(const HDC hdc)
{
	if (cachedHDC) {
		graphic->ReleaseHDC(hdc);
		cachedHDC = false;
	}
}

void Gp::DrawSquare(int xDest, int yDest, int xEnd, int yEnd, uint32_t color, bool filled)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	if (filled) {
		// ����ʵ�ľ���
		const HBRUSH hBrush = CreateSolidBrush(color&0x00FFFFFF); // ��ɫʵ��ˢ��
		const HBRUSH hOldBrush = HBRUSH(SelectObject(hdc, hBrush));
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}
	else {
		// ���ƿ��ľ���
		const HPEN hPen = CreatePen(PS_SOLID, 2, color&0x00FFFFFF); // ��ɫ����
		const HPEN hOldPen = HPEN(SelectObject(hdc, hPen));
		const HBRUSH hNullBrush = HBRUSH(SelectObject(hdc, GetStockObject(NULL_BRUSH)));
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hNullBrush);
		DeleteObject(hPen);
	}
}

void Gp::LockWindowSize() const
{
	// ��ȡ��ǰ������ʽ
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	// �Ƴ��ɵ�����С����󻯰�ť����ʽ
	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	// �����µĴ�����ʽ
	SetWindowLong(hwnd, GWL_STYLE, style);
	// ǿ�ƴ������»�����Ӧ���µ���ʽ
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void Gp::UnlockWindowSize() const
{
	// ��ȡ��ǰ������ʽ
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	// ��ӿɵ�����С����󻯰�ť����ʽ
	style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
	// �����µĴ�����ʽ
	SetWindowLong(hwnd, GWL_STYLE, style);
	// ǿ�ƴ������»�����Ӧ���µ���ʽ
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}
