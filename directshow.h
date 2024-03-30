#include <dshow.h>
#include"mywindows.h"

#pragma comment(lib, "Quartz.lib")
#pragma comment(lib, "Strmiids.lib")
IGraphBuilder* pGraph = NULL;
IMediaControl* pControl = NULL;
IMediaEvent* pEvent = NULL;
IVideoWindow* pVideoWindow = NULL;
IMediaEventEx* pMediaEvent = NULL;

#define _1_6 "\\files\\mp3\\reveal-fullstar.mp3"
#define _1_5 "\\files\\mp3\\reveal-5star.mp3"
#define _1_4 "\\files\\mp3\\reveal-4star.mp3"
#define _1_3 "\\files\\mp3\\reveal-3star.mp3"

void play(const char* path) {
	std::string path_;
	path_ = runpath;
	path_ += path;
	// Initialize the COM library.
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))return;
	// Create the filter graph manager and query for interfaces.
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
	if (FAILED(hr))return;
	hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent);
	pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
	hr = pGraph->RenderFile(UTF8To16(path_.c_str()), NULL);
	if (hr != S_OK) { 
		errlog("read video unsuccessfully"); 
		errend();
	}
	if (!(std::stoi(getConfigValue(INWINDOW))))
		pVideoWindow->put_WindowStyle(WS_POPUP);
	else
		pVideoWindow->put_WindowStyle(WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_THICKFRAME);
	pVideoWindow->put_Width(windowWidth);
	pVideoWindow->put_Height(windowHeight);
	pVideoWindow->put_Left(windowLeft);
	pVideoWindow->put_Top(windowTop);

	if (SUCCEEDED(hr))
	{
		mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
		log("play begin");
		hr = pControl->Run();
        long evCode;
        pEvent->WaitForCompletion(INFINITE, &evCode);
	}
    pVideoWindow->Release();
	pControl->Release();
	pEvent->Release();
	pGraph->Release();
	CoUninitialize();
	log("play end");
}

void openmusic(const char* path,const char* miconname) {
	std::string p;
	p = "close ";
	p += miconname;
	mciSendStringA(p.c_str(), NULL, 0, NULL); // 关闭音乐文件
	log(p.c_str());
	p = "open \"";
	p += runpath;
	p += path;
	p += "\" alias ";
	p += miconname;
	mciSendStringA(p.c_str(), NULL, 0, NULL);
	log("打开%s,指令为%s",path, p.c_str());
	p = "play ";
	p += miconname;
	log(p.c_str());
	mciSendStringA(p.c_str(), 0, 0, 0);
}