#include "Gp.h"
#include"config.h"
#include "mywindows.h"
using namespace Gdiplus;
using namespace std;

void Gp::Load_() {
	bitmaps.clear();
	bitmaps.push_back(std::make_shared<Bitmap>(config::getpath(FLOATPHOTO).c_str()));
	for (const auto& i : bitmaps) {
		if (i.get()->GetLastStatus() != Ok) {
			mywindows::errlog("��ͼƬʧ��");
		}
	}
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
			mywindows::errlog("��ͼƬʧ��");
		}
	}
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
void Gp::Paint(int xDest, int yDest, Bitmap* image, int wDest = 0, int hDest = 0)
{
	if (wDest != 0 && hDest != 0) {
		graphic.get()->DrawImage(image, xDest, yDest, wDest, hDest);
	}
	else {
		graphic.get()->DrawImage(image, xDest, yDest);
	}
}
void Gp::Paint(int xDest, int yDest, Bitmap* image)
{
	graphic.get()->DrawImage(image, xDest, yDest);
}
void Gp::Paint(int xDest, int yDest, int wDest, int hDest, int number)
{
	graphic.get()->DrawImage(bitmaps[number].get(), xDest, yDest, wDest, hDest);
}
void Gp::Paint(int xDest, int yDest, int number)
{
	graphic.get()->DrawImage(bitmaps[number].get(), xDest, yDest);
}

void Gp::DrawString(std::wstring str, HFONT font, int x, int y)
{
	HDC hdc;
	hdc= GetDC();
	SelectObject(hdc, font);
	TextOut_(hdc, x, y, str.c_str());
	ReleaseDC(hdc);
}

void Gp::DrawString(std::string str, HFONT font, int x, int y)
{
	HDC hdc;
	hdc = GetDC();
	SelectObject(hdc, font);
	TextOutA(hdc, x, y, str.c_str(), str.size());
	ReleaseDC(hdc);
}

HDC Gp::GetDC()
{
	return graphic.get()->GetHDC();
}

void Gp::ReleaseDC(HDC hdc)
{
	graphic.get()->ReleaseHDC(hdc);
}
