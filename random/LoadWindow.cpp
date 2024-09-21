#include "LoadWindow.h"

#include "mywindows.h"
#include "resource.h"

using namespace std;
using namespace Gdiplus;

extern double prosses;
extern string prosses_str;
HFONT LoadWIndowFont;

#define ROLL 0

LoadWindowExplorer* LoadWindowExplorer::instance = nullptr;
LoadWindowPainter* LoadWindowPainter::instance = nullptr;


LRESULT CALLBACK LoadWindowProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	static LoadWindowPainter* painter = LoadWindowPainter::getInstance(hWnd);
	static POINT last_mouse_pos;
	static bool is_mouse_dragging = false;
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		double x, y, w, h;
		x = mywindows::screenWidth * 0.04;
		y = mywindows::screenHeight * 0.04;
		w = mywindows::screenWidth * 0.31;
		h = mywindows::screenHeight * 0.085;
		painter->DrawSqare(x, y, x + w, y + h, 255, 255, 255, 0);
		x = mywindows::screenWidth * 0.05;
		y = mywindows::screenHeight * 0.05;
		w = mywindows::screenWidth * 0.3 * prosses / 100;
		h = mywindows::screenHeight * 0.075;
		painter->DrawSqare(x, y, w + x, y + h, 243, 123, 122, 1);
		x = mywindows::screenWidth * 0.05 + (prosses / 100.0) * mywindows::screenWidth * 0.3;
		y = mywindows::screenHeight * 0.05;
		w = mywindows::screenHeight * 0.075;
		h = mywindows::screenHeight * 0.075;
		painter->Paint(x, y, w, h, 0);
		painter->DrawStringBetween(prosses_str, LoadWIndowFont, 0, 0, mywindows::screenWidth * 0.4, mywindows::screenHeight * 0.2);
		painter->Flush();
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_NCHITTEST:
	{
		if (wParam == HTLEFT)
		{
			SetCapture(hWnd);
			return HTCAPTION;
		}
		return HTCLIENT;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (is_mouse_dragging)
		{
			POINT currentMousePos;
			GetCursorPos(&currentMousePos);

			RECT rect;
			GetWindowRect(hWnd, &rect);
			int dx = currentMousePos.x - last_mouse_pos.x;
			int dy = currentMousePos.y - last_mouse_pos.y;

			MoveWindow(hWnd, rect.left + dx, rect.top + dy, rect.right - rect.left, rect.bottom - rect.top, TRUE);

			last_mouse_pos = currentMousePos;
		}
		break;
	}
	default:
		break;
	}
}

void LoadWindow::init()
{
	WNDCLASS wndcls{}; //创建一个窗体类
	wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	wndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//设置光标
	wndcls.hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//设置窗体左上角的图标
	wndcls.hInstance = GetModuleHandle(NULL);;//设置窗体所属的应用程序实例
	wndcls.lpfnWndProc = LoadWindowProc;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	wndcls.lpszClassName = L"LoadWindow";//设置窗体的类名
	wndcls.lpszMenuName = NULL;//设置窗体的菜单,没有，填NULL
	wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&wndcls);//向操作系统注册窗体
	int desiredPixelHeight = mywindows::screenWidth * 0.02;
	// 获取设备上下文的 DPI
	HDC hdc = GetDC(nullptr); // 获取桌面设备上下文
	int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	// 计算逻辑单位高度
	int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
	int logicalweidth = logicalHeight * 0.77;
	LoadWIndowFont = CreateFontW(logicalHeight, logicalweidth, 0, 0, FW_NORMAL, 0, 0, 0, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
	hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"LoadWindow", L"LoadWindow", WS_POPUPWINDOW,
		mywindows::screenWidth*0.3,mywindows::screenHeight*0.4,mywindows::screenWidth*0.4,mywindows::screenHeight*0.2, NULL, NULL, NULL, NULL);
	refresh.Describe = "Load Window refresh Timer";
	refresh.setPool(1);
	refresh.setDelay(10);
	refresh.setCallBack([this]()
		{
			InvalidateRect(hwnd, NULL, FALSE);
		});
}

LoadWindow::LoadWindow()
{
	init();
}

LoadWindow::~LoadWindow()
{
	UnregisterClassW(L"LoadWindow", GetModuleHandle(NULL));
	DestroyWindow(hwnd);
	hwnd = 0;

}

void LoadWindow::create()
{
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	refresh.start();
}

void LoadWindow::destroy()
{
	refresh.stop();
	DestroyWindow(hwnd);
	delete this;
}

void LoadWindowExplorer::Load()
{
	GBitmaps.push_back(make_shared<Bitmap>(LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1))));
	for(auto&i:GBitmaps)
	{
		HBITMAP hBitmap;
		i->GetHBITMAP(Color(0, 0, 0), &hBitmap);
		HBitmaps.push_back(make_shared<HBITMAP>(hBitmap));
		BITMAP bitmap;
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);
		Bitmaps.push_back(make_shared<BITMAP>(bitmap));
	}
}

LoadWindowExplorer::LoadWindowExplorer()
{
	Load();
}

LoadWindowExplorer::~LoadWindowExplorer()
{
	GBitmaps.clear();
	HBitmaps.clear();
	Bitmaps.clear();
}

LoadWindowExplorer* LoadWindowExplorer::getInstance()
{
	if (instance == nullptr)
		instance = new LoadWindowExplorer();
	return instance;
}

HBITMAP LoadWindowExplorer::getHBitmap(const int number) const
{
	return *HBitmaps[number];
}

const BITMAP* LoadWindowExplorer::getBitmap(const int number) const
{
	return Bitmaps[number].get();
}

const Gdiplus::Image* LoadWindowExplorer::getGdiImage(const int number) const
{
	return GBitmaps[number].get();
}

LoadWindowPainter::LoadWindowPainter(HWND hwnd)
{
	hdc = 0;
	this->hwnd = hwnd;
	ptr = LoadWindowExplorer::getInstance();
	ReleaseDC(hdc);
	// 获取窗口的客户区大小
	RECT rc;
	GetClientRect(mywindows::main_hwnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	// 重新创建一个与窗口客户区大小相同的Bitmap作为离屏缓冲区
	buffer = make_shared<Bitmap>(width, height, PixelFormat32bppARGB);
	// 重新创建Graphics对象用于绘制到离屏缓冲区
	graphic = make_shared<Graphics>(buffer.get());
}

LoadWindowPainter::~LoadWindowPainter()
{
	if (cachedHDC)
		ReleaseDC(hdc);
	delete ptr;
	hwnd = 0;
	hdc = 0;
}

LoadWindowPainter* LoadWindowPainter::getInstance(HWND hwnd)
{
	if (instance == nullptr)
		instance = new LoadWindowPainter(hwnd);
	return instance;
}

LoadWindowPainter* LoadWindowPainter::getInstance()
{
	return instance;
}

void LoadWindowPainter::Flush()
{
	if (cachedHDC)
		ReleaseDC(hdc);
	// 获取窗口的设备上下文
	HDC hdcWindow = ::GetDC(hwnd);

	// 创建一个离屏DC
	HDC hdcOffscreen = CreateCompatibleDC(hdcWindow);

	// 创建一个与窗口大小相同的位图
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, buffer->GetWidth(), buffer->GetHeight());

	// 将位图选入离屏DC
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcOffscreen, hBitmap);

	// 将离屏缓冲区的内容绘制到离屏DC上
	Gdiplus::Graphics graphicsOffscreen(hdcOffscreen);
	graphicsOffscreen.DrawImage(buffer.get(), 0, 0);

	// 将离屏DC的内容绘制到窗口上
	BitBlt(hdcWindow, 0, 0, buffer->GetWidth(), buffer->GetHeight(), hdcOffscreen, 0, 0, SRCCOPY);

	// 释放资源
	SelectObject(hdcOffscreen, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcOffscreen);

	// 释放设备上下文
	::ReleaseDC(hwnd, hdcWindow);
}

void LoadWindowPainter::Paint(int xDest, int yDest, int wDest, int hDest, int number)
{
	if (!cachedHDC)
		hdc = GetDC();
	HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP* t_bitmap = ptr->getBitmap(number);
	int w = t_bitmap->bmWidth;
	int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void LoadWindowPainter::Paint(int xDest, int yDest, int number)
{
	if (!cachedHDC)
		hdc = GetDC();
	HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP* t_bitmap = ptr->getBitmap(number);
	int w = t_bitmap->bmWidth;
	int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void LoadWindowPainter::DrawStringBetween(std::wstring str, HFONT font, int x, int y, int xend, int yend,
	unsigned char R, unsigned char G, unsigned char B)
{
	if (!cachedHDC)
		hdc = GetDC();
	// 创建一个GDI+ Graphics对象
	Graphics graphics(hdc);
	// 设置文本渲染模式为抗锯齿
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// 创建一个GDI+ Font对象
	Font gdiPlusFont(hdc, font);
	// 创建一个GDI+ SolidBrush对象
	SolidBrush brush(Color(255, R, G, B));

	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	// 绘制文本
	graphics.DrawString(str.c_str(), -1, &gdiPlusFont, layoutRect, &format, &brush);
}

void LoadWindowPainter::DrawStringBetween(std::string str, HFONT font, int x, int y, int xend, int yend,
	unsigned char R, unsigned char G, unsigned char B)
{
	if (!cachedHDC)
		hdc = GetDC();
	// 创建一个GDI+ Graphics对象
	Graphics graphics(hdc);
	// 设置文本渲染模式为抗锯齿
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// 创建一个GDI+ Font对象
	Font gdiPlusFont(hdc, font);
	// 创建一个GDI+ SolidBrush对象
	SolidBrush brush(Color(255, R, G, B));

	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	// 绘制文本
	std::wstring wstr(str.begin(), str.end());
	graphics.DrawString(wstr.c_str(), -1, &gdiPlusFont, layoutRect, &format, &brush);
}

void LoadWindowPainter::DrawSqare(int xDest, int yDest, int xEnd, int yEnd, int R, int G, int B, bool filled)
{
	hdc = GetDC();
	if (filled) {
		// 绘制实心矩形
		HBRUSH hBrush = CreateSolidBrush(RGB(R, G, B)); // 红色实心刷子
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}
	else {
		// 绘制空心矩形
		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(R, G, B)); // 蓝色画笔
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		HBRUSH hNullBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hNullBrush);
		DeleteObject(hPen);
	}
}

HDC LoadWindowPainter::GetDC()
{
	if (cachedHDC)
		return hdc;
	else
	{
		cachedHDC = 1;
		return graphic->GetHDC();
	}
}

void LoadWindowPainter::ReleaseDC(HDC hdc)
{
	if (cachedHDC) {
		graphic->ReleaseHDC(hdc);
		cachedHDC = 0;
	}
}

