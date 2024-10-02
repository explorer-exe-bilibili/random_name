#pragma once

#include <map>
#include <string>
#include <dshow.h>

#define CLICK_MUSIC "click"
#define ENTER "enter"

class VideoPlayer {
public:
    void load(const std::wstring& id, const std::wstring& path);
    void play(const std::wstring& id);
    void unload(const std::wstring& id);
    static VideoPlayer* getInstance();
	~VideoPlayer();

private:
    struct VideoData {
        IGraphBuilder* pGraph = nullptr;
        IMediaControl* pControl = nullptr;
        IMediaEvent* pEvent = nullptr;
        IVideoWindow* pVideoWindow = nullptr;
        IBasicAudio* pBasicAudio = nullptr;
        IMediaSeeking* pSeeking = nullptr;
    };
    static VideoPlayer* Instance;
    std::map<std::wstring, VideoData> videos;
    bool comInitialized = false;
};