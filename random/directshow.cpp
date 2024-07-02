#include "directshow.h"
#include "mywindows.h"
#include "log.h"
#include "config.h"
#include "set-json.h"
#include "sth2sth.h"
#include "ui.h"

#pragma comment(lib, "Quartz.lib")
#pragma comment(lib, "Strmiids.lib")

using namespace std;

IGraphBuilder* directshow::pGraph=nullptr;
IMediaControl* directshow::pControl=nullptr;
IMediaEvent* directshow::pEvent=nullptr;
IVideoWindow* directshow::pVideoWindow=nullptr;
IMediaEventEx* directshow::pMediaEvent=nullptr;
IDispatch* directshow::pDispatch=nullptr;
IBasicAudio* directshow::pBaicAudio=nullptr;
bool directshow::playingbgm=0;


void directshow::play(wstring path) {
	std::wstring path_;
	if (!std::filesystem::exists(path)) {
		mywindows::errlog("file not found");
		MessageBox(nullptr, L"文件不存在，请重新选择", L"error", MB_ICONERROR);
		return;
	}
	if (path.find_first_of(L'\\') == 0) {
		path_ = Log::wrunpath;
		path_ += path;
	}
	else {
		path_ = path;
	}
	// Initialize the COM library.
	HRESULT hr = CoInitialize(nullptr);
	// Create the filter graph manager and query for interfaces.
	hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
	HANDLE hFile= nullptr;
	if (mywindows::debug) {
		std::wstring logfile = mywindows::logf.FileNamew + L".directshow.log";
		hFile = CreateFile(logfile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		pGraph->SetLogFile((DWORD_PTR)hFile);
	}
	hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent);
	pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
	pGraph->QueryInterface(IID_IBasicAudio, (void**)&pBaicAudio);
	hr = pGraph->RenderFile(path_.c_str(), nullptr);
	if (hr != S_OK) {
		mywindows::errlog("the file exists but read video unsuccessfully");
		int mbox=MessageBox(nullptr,L"如果播放失败请安装K-Lite_Codec, 是否打开官网下载页面？", L"error", MB_ICONERROR|MB_YESNO);
		if (mbox == IDYES)
		{
			ShellExecute(nullptr, L"open", L"https://codecguide.com/download_k-lite_codec_pack_basic.htm", nullptr, nullptr, SW_SHOWNORMAL);
			wstring p = Log::wrunpath + L"\\files\\imgs\\tips.png";
			if (std::filesystem::exists(p))
			{
				Sleep(500);
				ShellExecuteW(nullptr, L"open", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
			}
		}
	}
	if (!config::getint(INWINDOW)) {
		pVideoWindow->put_WindowStyle(WS_POPUP|WS_CHILD);
	}
	else {
		pVideoWindow->put_WindowStyle(WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_THICKFRAME);
	}
	pVideoWindow->put_Left(mywindows::windowLeft);
	pVideoWindow->put_Top(mywindows::windowTop);
	pVideoWindow->put_Width(mywindows::WW * 1.02);
	pVideoWindow->put_Height(mywindows::WH * 1.1);
	if (SUCCEEDED(hr))
	{
		if (!ui::SS.offmusic) {
			mciSendString(L"stop bgm", nullptr, 0, nullptr); // 停止播放
			pBaicAudio->put_Volume(0);
		}
		if (ui::SS.offmusic) pBaicAudio->put_Volume(-10000);
		mywindows::log("play begin");
		pVideoWindow->SetWindowPosition(0, 0, mywindows::WW, mywindows::WH);
		hr = pControl->Run();
		long evCode;
		pEvent->WaitForCompletion(INFINITE, &evCode);
	}
	pVideoWindow->Release();
	pControl->Release();
	pEvent->Release();
	pBaicAudio->Release();
	pGraph->Release();
	if(hFile!= nullptr)CloseHandle(hFile);
	CoUninitialize();
	mywindows::log("play end");
}

void directshow::music(const char* path) {
	if (!ui::SS.offmusic) {
		std::wstring p;
		p = L"close ";
		p += L"temp";
		mciSendString(p.c_str(), nullptr, 0, nullptr); // 关闭音乐文件
		mywindows::log(p.c_str());
		p = L"open \"";
		p += Log::wrunpath;
		p += sth2sth::str2wstr(path);
		p += L"\" alias ";
		p += L"temp";
		mciSendString(p.c_str(), nullptr, 0, nullptr);
		if(mywindows::debug)
		mywindows::logf << "打开" << path << "指令为" << p;
		p = L"play ";
		p += L"temp";
		mywindows::log(p.c_str());
		mciSendString(p.c_str(), nullptr, 0, nullptr);
	}
}

void directshow::stopmusic() {
	playingbgm = 0;
	mciSendStringA("close temp", nullptr, 0, nullptr);
	mciSendString(L"stop bgm", nullptr, 0, nullptr);
}

void directshow::startbgm() {
	if (!playingbgm) {
		if(!ui::SS.offmusic)
		mciSendString(L"play bgm repeat", nullptr, 0, nullptr);
		playingbgm = 1;
	}
}