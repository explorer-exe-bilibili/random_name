#pragma once
#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>
#include <vector>
class explorer
{
	static explorer* instance;
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> GdiImage;
	std::vector<HBITMAP> HBitmap;
	std::vector<BITMAP> Bitmap;
public:
	explorer();
	~explorer();
	static explorer* getInstance();
	HBITMAP getHBitmap(const int number)const;
	const BITMAP* getBitmap(const int number)const;
	const Gdiplus::Image* getGdiImage(const int number)const;
	void PlayMusic(const std::string& alias)const;
	void PlayMusic(const std::wstring& alias)const;
	void Load();
	void reloadBitmap(int number);
};


