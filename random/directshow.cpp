#include "directshow.h"
#include "mywindows.h"
#include "log.h"
#include "config.h"
#include "set-json.h"
#include "sth2sth.h"

#pragma comment(lib, "Quartz.lib")
#pragma comment(lib, "Strmiids.lib")

using namespace std;

IGraphBuilder* directshow::pGraph=0;
IMediaControl* directshow::pControl=0;
IMediaEvent* directshow::pEvent=0;
IVideoWindow* directshow::pVideoWindow=0;
IMediaEventEx* directshow::pMediaEvent=0;
IDispatch* directshow::pDispatch=0;
IBasicAudio* directshow::pBaicAudio=0;
bool directshow::playingbgm=0;

extern set2 setscreen;

void directshow::play(wstring path) {
	std::wstring path_;
	if (path.find_first_of(L'\\') == 0) {
		path_ = Log::wrunpath;
		path_ += path;
	}
	else {
		path_ = path;
	}
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
	hr = pGraph->RenderFile(path_.c_str(), NULL);
	if (hr != S_OK) {
		mywindows::errlog("read video unsuccessfully");
	}
	if (!config::getint(INWINDOW)) {
		pVideoWindow->put_WindowStyle(WS_POPUP|WS_CHILD);
	}
	else {
		pVideoWindow->put_WindowStyle(WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_THICKFRAME);
		pVideoWindow->put_Width(mywindows::windowWidth * 1.02);
		pVideoWindow->put_Height(mywindows::windowHeight * 1.1);
	}
	pVideoWindow->put_Left(mywindows::windowLeft);
	pVideoWindow->put_Top(mywindows::windowTop);
	if (SUCCEEDED(hr))
	{
		if (!setscreen.offmusic) {
			mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
			pBaicAudio->put_Volume(0);
		}
		if (setscreen.offmusic) pBaicAudio->put_Volume(-10000);
		mywindows::log("play begin");
		pVideoWindow->SetWindowPosition(0, 0, mywindows::windowWidth, mywindows::windowHeight);
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
	if (!setscreen.offmusic) {
		std::wstring p;
		p = L"close ";
		p += L"temp";
		mciSendString(p.c_str(), NULL, 0, NULL); // 关闭音乐文件
		mywindows::log(p.c_str());
		p = L"open \"";
		p += Log::wrunpath;
		p += sth2sth::str2wstr(path);
		p += L"\" alias ";
		p += L"temp";
		mciSendString(p.c_str(), NULL, 0, NULL);
		mywindows::log("打开%s,指令为%s", path, p.c_str());
		p = L"play ";
		p += L"temp";
		mywindows::log(p.c_str());
		mciSendString(p.c_str(), 0, 0, 0);
	}
}

void directshow::stopmusic() {
	playingbgm = 0;
	mciSendStringA("close temp", NULL, 0, 0);
	mciSendString(L"stop bgm", 0, 0, 0);
}

void directshow::startbgm() {
	if (!playingbgm) {
		if(!setscreen.offmusic)
		mciSendString(L"play bgm repeat", 0, 0, 0);
		playingbgm = 1;
	}
}