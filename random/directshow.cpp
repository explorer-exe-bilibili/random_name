#include "directshow.h"
#include <filesystem>
#include <thread>
#include <windows.h>

#include "config.h"
#include "mywindows.h"
#include"configitem.h"
#include "ui.h"
#include "Log.h"

#pragma comment(lib, "Quartz.lib")
#pragma comment(lib, "Strmiids.lib")

directshow* directshow::Instance = nullptr;
void trySetMEMMode();

void directshow::load(const std::wstring& id, const std::wstring& path) {
	std::thread Load([this, id, path] {

		if (!std::filesystem::exists(path)) {
			mywindows::errlog("file not found");
			MessageBox(nullptr, L"文件不存在，请重新选择", L"error", MB_ICONERROR);
			return;
		}

		if (!comInitialized) {
			HRESULT hr = CoInitialize(nullptr);
			if (FAILED(hr)) {
				mywindows::errlog("COM initialization failed");
				return;
			}
			comInitialized = true;
		}

		VideoData videoData;
		HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&videoData.pGraph);
		if (FAILED(hr)) {
			mywindows::errlog("Failed to create filter graph manager");
			trySetMEMMode();
			return;
		}

		if (mywindows::debug) {
			std::wstring logfile = mywindows::logf.FileNamew + L".directshow.log";
			HANDLE hFile = CreateFile(logfile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile == INVALID_HANDLE_VALUE) {
				mywindows::errlog("Failed to create log file");
			}
			else {
				videoData.pGraph->SetLogFile((DWORD_PTR)hFile);
			}
		}

		hr = videoData.pGraph->QueryInterface(IID_IMediaControl, (void**)&videoData.pControl);
		if (FAILED(hr)) {
			mywindows::errlog("Failed to query IMediaControl interface");
			trySetMEMMode();
			return;
		}

		hr = videoData.pGraph->QueryInterface(IID_IMediaEvent, (void**)&videoData.pEvent);
		if (FAILED(hr)) {
			mywindows::errlog("Failed to query IMediaEvent interface");
			trySetMEMMode();
			return;
		}

		hr = videoData.pGraph->QueryInterface(IID_IVideoWindow, (void**)&videoData.pVideoWindow);
		if (FAILED(hr)) {
			mywindows::errlog("Failed to query IVideoWindow interface");
			trySetMEMMode();
			return;
		}

		hr = videoData.pGraph->QueryInterface(IID_IBasicAudio, (void**)&videoData.pBasicAudio);
		if (FAILED(hr)) {
			mywindows::errlog("Failed to query IBasicAudio interface");
			trySetMEMMode();
			return;
		}

		hr = videoData.pGraph->QueryInterface(IID_IMediaSeeking, (void**)&videoData.pSeeking);
		if (FAILED(hr)) {
			mywindows::errlog("Failed to query IMediaSeeking interface");
			trySetMEMMode();
			return;
		}

		hr = videoData.pGraph->RenderFile(path.c_str(), nullptr);
		if (FAILED(hr)) {
			mywindows::errlog("the file exists but read video unsuccessfully");
			int mbox = MessageBox(nullptr, L"如果播放失败,请安装K-Lite_Codec后再试, 是否打开官网下载页面？", L"error", MB_ICONERROR | MB_YESNO);
			if (mbox == IDYES) {
				ShellExecute(nullptr, L"open", L"https://codecguide.com/download_k-lite_codec_pack_basic.htm", nullptr, nullptr, SW_SHOWNORMAL);
				std::wstring p = Log::wrunpath + L"\\files\\imgs\\tips.png";
				if (std::filesystem::exists(p)) {
					Sleep(500);
					ShellExecuteW(nullptr, L"open", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
				}
			}
			exit(-1);
		}

		videos[id] = videoData;
		});
	Load.detach();
}

directshow* directshow::getInstance()
{
	if(!Instance)
	{
		Instance = new directshow();
	}
	return Instance;
}

void directshow::play(const std::wstring& id) {
	auto it = videos.find(id);
	if (it == videos.end()) {
		mywindows::errlog("Video not loaded");
		MessageBox(nullptr, L"视频未加载，请先加载视频", L"error", MB_ICONERROR);
		return;
	}

	VideoData& videoData = it->second;
	// 重置视频位置到起始位置
	if (videoData.pSeeking) {
		LONGLONG pos = 0;
		videoData.pSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
	}
	if (!config::getint(INWINDOW)) {
		videoData.pVideoWindow->put_WindowStyle(WS_POPUP | WS_CHILD);
	}
	else {
		videoData.pVideoWindow->put_WindowStyle(WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_THICKFRAME);
	}

	videoData.pVideoWindow->put_Owner((OAHWND)mywindows::main_hwnd);
	videoData.pVideoWindow->put_MessageDrain((OAHWND)mywindows::main_hwnd);

	if (!ui::SS->offmusic) {
		mciSendString(L"stop bgm", nullptr, 0, nullptr); // 停止播放
		videoData.pBasicAudio->put_Volume(0);
	}
	if (ui::SS->offmusic) videoData.pBasicAudio->put_Volume(-10000);

	mywindows::log("play begin");
	RECT WindowRect;
	GetWindowRect(mywindows::main_hwnd, &WindowRect);
	HRESULT hr = videoData.pControl->Run();
	if (SUCCEEDED(hr)) {
		long evCode;
		hr = videoData.pVideoWindow->SetWindowPosition(0, 0, mywindows::WW, mywindows::WH);
		videoData.pEvent->WaitForCompletion(INFINITE, &evCode);
	}
	mywindows::log("play end");
	// 隐藏视频窗口
	videoData.pVideoWindow->put_Visible(OAFALSE);
}

void directshow::unload(const std::wstring& id) {
	auto it = videos.find(id);
	if (it != videos.end()) {
		VideoData& videoData = it->second;
		videoData.pVideoWindow->Release();
		videoData.pControl->Release();
		videoData.pEvent->Release();
		videoData.pBasicAudio->Release();
		videoData.pGraph->Release();
		videoData.pSeeking->Release();
		videos.erase(it);
	}
}

directshow::~directshow() {
	for (auto& pair : videos) {
		VideoData& videoData = pair.second;
		videoData.pVideoWindow->Release();
		videoData.pControl->Release();
		videoData.pEvent->Release();
		videoData.pBasicAudio->Release();
		videoData.pGraph->Release();
		videoData.pSeeking->Release();
	}
	if (comInitialized) {
		CoUninitialize();
	}
}
