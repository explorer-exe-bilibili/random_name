#include<Windows.h>
#include<gdiplus.h>

#include "Gp.h"
#include "resource.h"
using namespace std;

Gp* p = nullptr;
namespace mywindows {
	HWND main_hwnd;
	int WW = 800;
	int WH = 600;
}
void paint() {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
	if (p!=nullptr)
	p->RenderOpenGL();
	EndPaint(mywindows::main_hwnd, &ps);
	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
}

void size() {
	RECT rect;
	GetClientRect(mywindows::main_hwnd, &rect);
	mywindows::WW = rect.right - rect.left;
	mywindows::WH = rect.bottom - rect.top;
	if (p == nullptr)return;
	p->CleanupOpenGL();
	p->initOpenGL(mywindows::main_hwnd);
}

void destroy() {
	PostQuitMessage(0);
}

LRESULT CALLBACK WinSunProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	switch (uMsg) {//通过判断消息进行消息响应
	case WM_PAINT:paint(); break;
	case WM_SIZE:size(); break;
	case WM_DESTROY:destroy(); break;
	case WM_ERASEBKGND:
		// 阻止Windows默认擦除背景，返回非零值表示已处理
		return 1;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);//对不感兴趣的消息进行缺省处理，必须有该代码，否则程序有问题
	}
	return 0;
}

int WINAPI WinMain(
	_In_ const HINSTANCE hInstance,      // handle to current instance
	_In_opt_ HINSTANCE hPrevInstance,  // handle to previous instance
	_In_ LPSTR lpCmdLine,          // command line
	_In_ int nCmdShow              // show state
) {
	WNDCLASS wndcls; //创建一个窗体类
	wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	wndcls.hbrBackground = HBRUSH(GetStockObject(NULL_BRUSH));//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	wndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//设置光标
	wndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_SMALL));//设置窗体左上角的图标
	wndcls.hInstance = GetModuleHandle(nullptr);;//设置窗体所属的应用程序实例
	wndcls.lpfnWndProc = WinSunProc;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	wndcls.lpszClassName = L"main";//设置窗体的类名
	wndcls.lpszMenuName = nullptr;//设置窗体的菜单,没有，填NULL
	wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&wndcls);//向操作系统注册窗体
	mywindows::main_hwnd = CreateWindowW(L"main",L"test", WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_THICKFRAME, 0, 0, mywindows::WW, mywindows::WH, NULL, NULL, GetModuleHandle(NULL), NULL);
	ShowWindow(mywindows::main_hwnd, SW_SHOW);
	p = new Gp(mywindows::main_hwnd);
	MSG msg;
	while (GetMessageW(&msg, nullptr, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;
}
