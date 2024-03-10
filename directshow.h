#include <dshow.h>
#include"mywindows.h"

#pragma comment(lib, "Quartz.lib")
#pragma comment(lib, "Strmiids.lib")
IGraphBuilder* pGraph = NULL;
IMediaControl* pControl = NULL;
IMediaEvent* pEvent = NULL;
IVideoWindow* pVideoWindow = NULL;
IMediaEventEx* pMediaEvent = NULL;

void play(const char* path) {

	// Initialize the COM library.
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))return;
	// Create the filter graph manager and query for interfaces.
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
	if (FAILED(hr))return;
	hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent);
	pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
	hr = pGraph->RenderFile(UTF8To16(path), NULL);
	if (hr != S_OK) { 
		errlog("read video unsuccessfully"); 
		PostQuitMessage(0);
	}
	pVideoWindow->put_WindowStyle(WS_POPUP);
	pVideoWindow->put_Width(screenWidth);
	pVideoWindow->put_Height(screenHeight);
	pVideoWindow->put_Left(0);
	pVideoWindow->put_Top(0);

	if (SUCCEEDED(hr))
	{
		mciSendString(L"stop bgm", NULL, 0, NULL); // Í£Ö¹²¥·Å
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