#include "directshow.h"
#include"mywindows.h"
#include "log.h"
#include "config.h"
#include"sth2sth.h"

#pragma comment(lib, "Quartz.lib")
#pragma comment(lib, "Strmiids.lib")


IGraphBuilder* directshow::pGraph=0;
IMediaControl* directshow::pControl=0;
IMediaEvent* directshow::pEvent=0;
IVideoWindow* directshow::pVideoWindow=0;
IMediaEventEx* directshow::pMediaEvent=0;
IDispatch* directshow::pDispatch=0;
IBasicAudio* directshow::pBaicAudio=0;

void directshow::play(const char* path) {
	std::string path_;
	path_ = Log::runpath;
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
	pGraph->QueryInterface(IID_IBasicAudio, (void**)&pBaicAudio);
	hr = pGraph->RenderFile(sth2sth::UTF8To16(path_.c_str()), NULL);
	if (hr != S_OK) {
		mywindows::errlog("read video unsuccessfully");
	}
	if (!(std::stoi(config::get(INWINDOW))))
		pVideoWindow->put_WindowStyle(WS_POPUP);
	else
		pVideoWindow->put_WindowStyle(WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_THICKFRAME);
	pVideoWindow->put_Width(mywindows::mywindows::windowWidth * 1.02);
	pVideoWindow->put_Height(mywindows::mywindows::windowHeight * 1.1);
	pVideoWindow->put_Left(mywindows::windowLeft);
	pVideoWindow->put_Top(mywindows::windowTop);

	if (SUCCEEDED(hr))
	{
		if (!mywindows::offmusic) {
			mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
			pBaicAudio->put_Volume(0);
		}
		if (mywindows::offmusic) pBaicAudio->put_Volume(-10000);
		mywindows::log("play begin");
		hr = pControl->Run();
		long evCode;
		pEvent->WaitForCompletion(INFINITE, &evCode);
	}
	pVideoWindow->Release();
	pControl->Release();
	pEvent->Release();
	pBaicAudio->Release();
	pGraph->Release();
	CoUninitialize();
	mywindows::log("play end");
}

void directshow::music(const char* path) {
	if (!mywindows::offmusic) {
		std::string p;
		const char* miconname;
		miconname = "temp";
		p = "close ";
		p += miconname;
		mciSendStringA(p.c_str(), NULL, 0, NULL); // 关闭音乐文件
		mywindows::log(p.c_str());
		p = "open \"";
		p += Log::runpath;
		p += path;
		p += "\" alias ";
		p += miconname;
		mciSendStringA(p.c_str(), NULL, 0, NULL);
		mywindows::log("打开%s,指令为%s", path, p.c_str());
		p = "play ";
		p += miconname;
		mywindows::log(p.c_str());
		mciSendStringA(p.c_str(), 0, 0, 0);
	}
}