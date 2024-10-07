#include"mywindows.h"
#include "floatwindow.h"
#include "init.h"
#include"MSGback.h"
#include<gdiplus.h>

#include "LoadWindow.h"
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
	case WM_COMMAND:MSGback::command(lParam, wParam);	break;
	case WM_CLOSE:MSGback::close(); break;
	case WM_DESTROY:MSGback::destroy(); break;
	case WM_ACTIVATEAPP:MSGback::active(); break;
	case WM_SHOWWINDOW:MSGback::showWindow(wParam);break;
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
	case WM_LBUTTONDOWN:floatwindow::lbuttondown(wParam); break;
	case WM_MOUSEMOVE:floatwindow::mousemove(lParam); break;
	case WM_LBUTTONUP:floatwindow::lbuttonup();break;
	case WM_LBUTTONDBLCLK:floatwindow::lbuttonclick(wParam); break;
	case WM_NCHITTEST:return floatwindow::nchittest(wParam);
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
		const HDC hdc = BeginPaint(mywindows::Quit_hwnd, &ps);
		if (QuitFont == nullptr) {
			const int desiredPixelHeight = mywindows::screenWidth * 0.17;
			// 获取设备上下文的 DPI
			const HDC hdc_1 = GetDC(nullptr); // 获取桌面设备上下文
			const int dpi = GetDeviceCaps(hdc_1, LOGPIXELSY);
			// 计算逻辑单位高度
			const int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
			const int logicalWidth = logicalHeight * 0.77;
			QuitFont = CreateFontW(logicalHeight * 0.7, logicalWidth * 0.7, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		}
		SelectObject(hdc, QuitFont);
		SetTextColor(hdc, RGB(251, 247, 187));
		TextOut_(hdc, mywindows::screenWidth * 0.2, mywindows::screenHeight * 0.05, L"拖至此退出随机点名");
		DeleteDC(hdc);
		EndPaint(mywindows::Quit_hwnd, &ps);
	}
	default:return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

int WINAPI WinMain(
	_In_ const HINSTANCE hInstance,      // handle to current instance
	_In_opt_ HINSTANCE hPrevInstance,  // handle to previous instance
	_In_ LPSTR lpCmdLine,          // command line
	_In_ int nCmdShow              // show state
) {
	mywindows::hinstance = hInstance;
	const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	if(mywindows::debug)
	SetEnvironmentVariable(L"GDIPlusDebugLevel", L"4");
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
