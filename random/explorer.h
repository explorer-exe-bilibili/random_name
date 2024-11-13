#pragma once
#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>
#include <vector>
#include "VideoPlayer.h"
class explorer
{
	static explorer* instance;
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> GdiImage;
	std::vector<HBITMAP> HBitmap;
	std::vector<BITMAP> Bitmap;
	VideoPlayer* video = nullptr;
	bool playingbgm = false;
public:
	explorer();
	~explorer();
	static explorer* getInstance();
	HBITMAP getHBitmap(int number)const;
	const BITMAP* getBitmap(int number)const;
	const Gdiplus::Image* getGdiImage(int number)const;
	static void PlayMusic(const std::string& alias);
	static void PlayMusic(const std::wstring& alias);
	void PlayVideo(const std::wstring& alias)const;
	static void stopMusic();
	void Load();
	void reloadBitmap(int number);
	void reloadVideo(std::wstring alias) const;
};


