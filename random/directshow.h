#pragma once
#include <dshow.h>
#include<string>

#define _1_6 "\\files\\mp3\\reveal-fullstar.mp3"
#define _1_5 "\\files\\mp3\\reveal-5star.mp3"
#define _1_4 "\\files\\mp3\\reveal-4star.mp3"
#define _1_3 "\\files\\mp3\\reveal-3star.mp3"
#define CLICK_MUSIC "click"
#define ENTER "exter"
class directshow
{
	static IGraphBuilder* pGraph;
	static IMediaControl* pControl;
	static IMediaEvent* pEvent;
	static IVideoWindow* pVideoWindow;
	static IMediaEventEx* pMediaEvent;
	static IDispatch* pDispatch;
	static IBasicAudio* pBaicAudio;
public:
	static void play(std::wstring path);
	static void stopmusic();
	static void startbgm();
	static bool playingbgm;
};

