#include "Gp.h"
#include"config.h"
#include "mywindows.h"
using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")

void Gp::Load_() {
	bitmaps.clear();
	bitmaps.push_back(std::make_shared<Bitmap>(config::getpath(FLOATPHOTO).c_str()));
	for (const auto& i : bitmaps) {
		if (i.get()->GetLastStatus() != Ok) {
			mywindows::errlog("打开图片失败");
		}
		else
		{
			HBITMAP tmp;
			BITMAP tmp_b;
			i->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &tmp);
			GetObject(tmp, sizeof(BITMAP), &tmp_b);
			HBitmaps.push_back(tmp);
			BITMAPs.push_back(tmp_b);
		}
	}
	//bitmaps.clear();
}
void Gp::Load()
{
	bitmaps.clear();
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER1).c_str()));//0
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER2).c_str()));//1
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER3).c_str()));//2
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER4).c_str()));//3
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\wish-background.jpg"));//4
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\cardbg.jpg"));//5
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink1.png"));//6
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink10.png"));//7
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue1.png"));//8
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue10.png"));//9
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\buttom.png"));//10
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\goldenbg.jpg"));//11
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\list.jpg"));//12
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\3list.png"));//13
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\4list.png"));//14
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\5list.png"));//15
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\fulllist.png"));//16
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\exit.png"));//17
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\exit.png"));//18
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\setting.jpg"));//19
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\settingbuttom.png"));//20
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-bow.png"));//21
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-sword.png"));//22
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-magic.png"));//23
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-claymore.png"));//24
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-polearm.png"));//25
	for (const auto& i : bitmaps) {
		if (i.get()->GetLastStatus() != Ok) {
			mywindows::errlog("打开图片失败");
		}
		else
		{
			HBITMAP tmp;
			BITMAP tmp_B;
			i->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &tmp);
			GetObject(tmp, sizeof(BITMAP), &tmp_B);
			HBitmaps.push_back(tmp);
			BITMAPs.push_back(tmp_B);
		}
	}
	//bitmaps.clear();
}
Gp::~Gp()
{
}
Gp::Gp(HWND hwnd)
{
	graphic = make_shared<Graphics>(hwnd);
	if (hwnd == mywindows::main_hwnd)
		Load();
	else if (hwnd == mywindows::float_hWnd)
		Load_();
}
void Gp::Paint(const int xDest, const int yDest, Bitmap* image, const int wDest = 0, const int hDest = 0)
{
	if (wDest != 0 && hDest != 0) {
		graphic.get()->DrawImage(image, xDest, yDest, wDest, hDest);
	}
	else {
		graphic.get()->DrawImage(image, xDest, yDest);
	}
}
void Gp::Paint(const int xDest, const int yDest, Bitmap* image)
{
	graphic.get()->DrawImage(image, xDest, yDest);
}
void Gp::Paint(const int xDest, const int yDest, const int wDest, const int hDest, const int number)
{
	graphic.get()->DrawImage(bitmaps[number].get(), xDest, yDest, wDest, hDest);
	//HDC hdc = GetDC();
	//HDC hdcMem = CreateCompatibleDC(hdc);
	//SelectObject(hdcMem, HBitmaps[number]);
	//int w = BITMAPs[number].bmWidth;
	//int h = BITMAPs[number].bmHeight;
	//// 设置混合函数
	//BLENDFUNCTION blendFunc;
	//blendFunc.BlendOp = AC_SRC_OVER;
	//blendFunc.BlendFlags = 0;
	//blendFunc.SourceConstantAlpha = 255; // 255 = 不透明，0 = 完全透明
	//blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	//AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
}
void Gp::Paint(const int xDest, const int yDest, const int number)
{
	graphic.get()->DrawImage(bitmaps[number].get(), xDest, yDest);
	//HDC hdc = GetDC();
	//HDC hdcMem = CreateCompatibleDC(hdc);
	//SelectObject(hdcMem, HBitmaps[number]);
	//int w = BITMAPs[number].bmWidth;
	//int h = BITMAPs[number].bmHeight;
	//// 设置混合函数
	//BLENDFUNCTION blendFunc;
	//blendFunc.BlendOp = AC_SRC_OVER;
	//blendFunc.BlendFlags = 0;
	//blendFunc.SourceConstantAlpha = 255; // 255 = 不透明，0 = 完全透明
	//blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	//AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
}

void Gp::DrawString(std::wstring str, const HFONT font, const int x, const int y, const int R, const int G, const int B)
{
	HDC hdc;
	hdc= GetDC();
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, font);
	SetTextColor(hdc, RGB(R, G, B));
	TextOut_(hdc, x, y, str.c_str());
	ReleaseDC(hdc);
}

void Gp::DrawString(std::string str, const HFONT font, const int x, const int y, const int R, const int G, const int B)
{
	HDC hdc;
	hdc = GetDC();
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, font);
	SetTextColor(hdc, RGB(R, G, B));
	TextOutA(hdc, x, y, str.c_str(), str.size());
	ReleaseDC(hdc);
}

HDC Gp::GetDC()
{
	return graphic.get()->GetHDC();
}

void Gp::ReleaseDC(const HDC hdc)
{
	graphic.get()->ReleaseHDC(hdc);
}
