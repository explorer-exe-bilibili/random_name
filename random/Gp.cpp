#include "Gp.h"
#include"config.h"
#include "mywindows.h"
using namespace Gdiplus;

void Gp::Load()
{
	bitmaps.clear();
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER1).c_str()));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER2).c_str()));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER3).c_str()));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER4).c_str()));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\wish-background.jpg"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\cardbg.jpg"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink1.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink1.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink10.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink10.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue1.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue1.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue10.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue10.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\buttom.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\goldenbg.jpg"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\list.jpg"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\3list.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\4list.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\5list.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\fulllist.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\exit.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\exit.png"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\setting.jpg"));
	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\settingbuttom.png"));

}

void Gp::Prepar(HDC hdc)
{
	Cbitmaps.clear();
	Graphics gra(hdc);
	for (const auto &i : bitmaps) {
		Cbitmaps.push_back(std::make_shared<Gdiplus::CachedBitmap>(i.get(),&gra));
	}
}
Gp::~Gp()
{
}
Gp::Gp()
{
	Load();
}
void Gp::Paint(HDC hdc, int xDest, int yDest, int wDest, int hDest, Image* image)
{
	Graphics graphics(hdc);
	graphics.DrawImage(image, xDest, yDest, wDest, hDest);
}
void Gp::Paint(HDC hdc, int xDest, int yDest, Image* image)
{
	Graphics graphics(hdc);
	graphics.DrawImage(image, xDest, yDest);
}
void Gp::Paint_trans(HDC hdc, int xDest, int yDest, int wDest, int hDest, Image* image)
{
	Graphics graphics(hdc);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetCompositingQuality(CompositingQualityHighQuality);
	graphics.DrawImage(image, xDest, yDest, wDest, hDest);
}
void Gp::Paint_trans(HDC hdc, int xDest, int yDest, Image* image)
{
	Graphics graphics(hdc);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetCompositingQuality(CompositingQualityHighQuality);
	graphics.DrawImage(image, xDest, yDest);
}
void Gp::Paint(HDC hdc, int xDest, int yDest, int wDest, int hDest, int number)
{
	Graphics graphics(hdc);
	graphics.DrawImage(bitmaps[number].get(), xDest, yDest, wDest, hDest);
}
void Gp::Paint(HDC hdc, int xDest, int yDest, int number)
{
	Graphics graphics(hdc);
	graphics.DrawImage(bitmaps[number].get(), xDest, yDest);
}
void Gp::pPaint(HDC hdc, int xDest, int yDest, int number)
{
	mywindows::log("Draw img");
	Graphics graphics(hdc);
	if (Cbitmaps.size() >= number) {
		CachedBitmap c(bitmaps[number].get(), &graphics);
		graphics.DrawCachedBitmap(&c, xDest, yDest);
	}
	else {
		graphics.DrawImage(bitmaps[number].get(), xDest, yDest);
		mywindows::errlog("Image cache FAIL %d", number);
	}
}void Gp::pPaint(HDC hdc, int xDest, int yDest, int wDest, int hDest, int number)
{
	mywindows::log("Draw img");
	Graphics graphics(hdc);
	if (Cbitmaps.size() >= number) {
		CachedBitmap c(bitmaps[number].get(), &graphics);
		graphics.DrawCachedBitmap(&c, xDest, yDest);
	}
	else {
		graphics.DrawImage(bitmaps[number].get(), xDest, yDest,wDest,hDest);
		mywindows::errlog("Image cache FAIL %d", number);
	}
}
void Gp::Paint_trans(HDC hdc, int xDest, int yDest, int wDest, int hDest, int number)
{
	Graphics graphics(hdc);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetCompositingQuality(CompositingQualityHighQuality);
	graphics.DrawImage(bitmaps[number].get(), xDest, yDest, wDest, hDest);
}
void Gp::Paint_trans(HDC hdc, int xDest, int yDest, int number)
{
	Graphics graphics(hdc);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetCompositingQuality(CompositingQualityHighQuality);
	graphics.DrawImage(bitmaps[number].get(), xDest, yDest);
}