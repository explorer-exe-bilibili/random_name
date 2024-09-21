#include"getname.h"
#include "directshow.h"
#include"mywindows.h"
#include <filesystem>
#include "floatwindow.h"
#include"set-json.h"
#include "init.h"
#include"MSGback.h"
#include<gdiplus.h>

#include "LoadWindow.h"

#define BALL1 100
#define BALL10 101
#define mode1 200
#define EXIT 444

using namespace std;

LoadWindow* loadWindow = nullptr;

LRESULT CALLBACK WinSunProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	mywindows::main_hwnd = hwnd;
	switch (uMsg) {//通过判断消息进行消息响应
	case WM_CREATE:MSGback::create(); break;
	case WM_PAINT:MSGback::paint(); break;
	case WM_SIZE:MSGback::size(); break;
	case WM_LBUTTONDOWN:MSGback::click(lParam); break;
	case WM_KEYDOWN:MSGback::keyDown(wParam); break;
	case WM_COMMAND:MSGback::commond(lParam, wParam);	break;
	case WM_CLOSE:MSGback::close(); break;
	case WM_DESTROY:MSGback::destroy(); break;
	case WM_ACTIVATEAPP:MSGback::active(); break;
	case WM_SHOWWINDOW:MSGback::showwindow(wParam);break;
	default:
		return DefWindowProc(mywindows::main_hwnd, uMsg, wParam, lParam);//对不感兴趣的消息进行缺省处理，必须有该代码，否则程序有问题
	}
	return 0;
}



LRESULT CALLBACK FloatWindowProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:floatwindow::create(); break;
	case WM_PAINT:floatwindow::paint(); break;
	case WM_NCHITTEST:return floatwindow::nchittest(wParam); floatwindow::showQuitWindow(); break;
	case WM_LBUTTONDOWN:floatwindow::lbuttondown(wParam); break;
	case WM_MOUSEMOVE:floatwindow::mousemove(lParam); break;
	case WM_LBUTTONUP:floatwindow::lbuttonup();break;
	case WM_LBUTTONDBLCLK:floatwindow::lbuttonclick(wParam); break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

HFONT QuitFont=nullptr;

LRESULT CALLBACK KillWindowProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	switch (message) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(mywindows::Quit_hwnd, &ps);
		if (QuitFont == nullptr) {
			int desiredPixelHeight = mywindows::screenWidth * 0.17;
			// 获取设备上下文的 DPI
			HDC hdc = GetDC(nullptr); // 获取桌面设备上下文
			int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
			// 计算逻辑单位高度
			int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
			int logicalweidth = logicalHeight * 0.77;
			QuitFont = CreateFontW(logicalHeight * 0.7, logicalweidth * 0.7, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		}
		SelectObject(hdc, QuitFont);
		SetTextColor(hdc, RGB(251, 247, 187));
		TextOut_(hdc, mywindows::screenWidth * 0.2, mywindows::screenHeight * 0.05, L"拖至此退出随机点名");
		DeleteDC(hdc);
		EndPaint(mywindows::Quit_hwnd, &ps);
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(
	_In_ const HINSTANCE hInstance,      // handle to current instance
	_In_opt_ HINSTANCE hPrevInstance,  // handle to previous instance
	_In_ LPSTR lpCmdLine,          // command line
	_In_ int nCmdShow              // show state
) {
	mywindows::hinstance = hInstance;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	loadWindow=new LoadWindow();
	loadWindow->create();
	init::main(WinSunProc, FloatWindowProc, KillWindowProc);
	init::MainWindow();
	mywindows::log("INIT COMPETENTLY SUCCESSFULLY");
	MSG msg;
	while (GetMessageW(&msg, nullptr, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	mywindows::log("bye!");
	return msg.wParam;
}
