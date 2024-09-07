#pragma once
#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>
#include <vector>
class resources_manager
{
	static resources_manager* instance;
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> GdiImage;
	std::vector<HBITMAP> HBitmap;
	std::vector<BITMAP> Bitmap;
public:
	resources_manager();
	~resources_manager();
	static resources_manager* getInstance();
	HBITMAP getHBitmap(const int number)const;
	const BITMAP* getBitmap(const int number)const;
	const Gdiplus::Image* getGdiImage(const int number)const;
	void PlayMusic(const std::string& alias)const;
	void PlayMusic(const std::wstring& alias)const;
	void Load();

};


