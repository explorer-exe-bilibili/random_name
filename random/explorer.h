#pragma once
#include <string>
#include <vector>
#include "VideoPlayer.h"
#include "EasyGL/EasyGL.h"
class explorer
{
	static explorer* instance;
	std::vector<Bitmap> Bitmaps;
	VideoPlayer* video = nullptr;
	bool playingbgm = false;
public:
	explorer();
	~explorer();
	static explorer* getInstance();
	const Bitmap* getBitmap(int number)const;
	static void PlayMusic(const std::string& alias);
	static void PlayMusic(const std::wstring& alias);
	void PlayVideo(const std::wstring& alias)const;
	static void stopMusic();
	void Load();
	void reloadBitmap(int number);
	void reloadVideo(std::wstring alias) const;
};


