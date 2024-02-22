#include<Windows.h>
#include"getname.h"
#include "directshow.h"
//#include "config.h"
#include"mywindows.h"
#include <cstdio>
#include"type.h"
#define BALL1 100
#define BALL10 101
#define MODE1 200 

HWND hVideoWnd,ball10,ball1,mode1,mode2,mode3,mode4,exitbotton; // 按钮句柄
HBITMAP hOverlayBitmap1, hBitmap1,hBitmap2, hOverlayBitmap2, hOverlayBitmap3, hOverlayBitmap4;
HICON pink1, blue1, pink10, blue10;
LPCWSTR names[256];
HINSTANCE hInstance = NULL;
BITMAP overlay1Bm, bm, overbm, overlay2Bm, overlay3Bm, overlay4Bm;
int currentBitmap = 1;      // 当前显示的背景图片
const char* video;
bool startmenu = 1,is5star = 0,is4star=0;
int number=0;
int mode = 1;
int overlay1X = screenWidth * 3 / 9 - screenWidth / 17;
int overlay2X = screenWidth * 4 / 9 - screenWidth / 17;
int overlay3X = screenWidth * 5 / 9 - screenWidth / 17;
int overlay4X = screenWidth * 6 / 9 - screenWidth / 17;
int overlay1Y = screenHeight * 3 / 40;
int overlayW = screenWidth * 3 / 40;
int overlayH = screenHeight * 3 / 40;
int button1x = overlay1X + overlayW;
int button2x = overlay2X + overlayW;
int button3x = overlay3X + overlayW;
int button4x = overlay4X + overlayW;
int buttony = overlay1Y + overlayH;
int overlayX = screenWidth / 10 * 2;
int overlayY = screenHeight / 10 * 2;
static int cx, cy, cxDib, cyDib;

LRESULT CALLBACK WinSunProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    HDC hdc =GetDC(NULL);
    hWnd = hwnd;
    switch (uMsg) {//通过判断消息进行消息响应
    case WM_CREATE:{
        for (int i = 0; i <= 255; i++)
            names[i] = random(i);
        // 加载两个背景图片
        hBitmap2 = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\over.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hBitmap1 = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\wish-background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hOverlayBitmap1 = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\over1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hOverlayBitmap2 = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\over2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hOverlayBitmap3 = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\over3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hOverlayBitmap4 = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\over4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        pink10 = (HICON)LoadImageW(NULL, L".\\files\\icos\\10pink.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        pink1 = (HICON)LoadImageW(NULL, L".\\files\\icos\\1pink.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        blue10 = (HICON)LoadImageW(NULL, L".\\files\\icos\\10blue.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        blue1 = (HICON)LoadImageW(NULL, L".\\files\\icos\\1blue.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        //获取图片的宽高
        ICONINFO iconInfo;
        GetIconInfo(pink10, &iconInfo);
        BITMAP ball10_;
        // 获取图标位图信息
        GetObject(iconInfo.hbmColor, sizeof(BITMAP), &ball10_);
        GetObject(hBitmap1, sizeof(BITMAP), &bm);
        GetObject(hOverlayBitmap1, sizeof(BITMAP), &overlay1Bm);
        GetObject(hOverlayBitmap2, sizeof(BITMAP), &overlay2Bm);
        GetObject(hOverlayBitmap3, sizeof(BITMAP), &overlay3Bm);
        GetObject(hOverlayBitmap4, sizeof(BITMAP), &overlay4Bm);
        // 输出图标的宽度和高度
        int tmpx1 = screenWidth - ball10_.bmWidth * 1.3;
        int tmpy = screenHeight - ball10_.bmHeight * 2;
        int tmpx2 = screenWidth - ball10_.bmWidth * 2.4;
        // 创建静态控件，并设置图像
        ball10 = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, tmpx1, tmpy, ball10_.bmWidth, ball10_.bmHeight, hWnd, (HMENU)BALL10, hInstance, NULL);
        ball1 = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, tmpx2, tmpy, ball10_.bmWidth, ball10_.bmHeight, hWnd, (HMENU)BALL1, hInstance, NULL);
        // 打开并播放背景音乐
        mciSendString(L"open .\\files\\mp3\\backsound.mp3 alias bgm", NULL, 0, NULL); // 打开 MP3 文件并创建一个别名 'bgm'
        mciSendString(L"play bgm repeat", NULL, 0, NULL); // 使用别名 'bgm' 播放音乐，并设置为循环播放
        SetWindowPos(hWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
        ReleaseDC(0, hdc);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (startmenu) {
            {
                HDC hdcMem = CreateCompatibleDC(hdc);
                // 拉伸显示图片
                SetStretchBltMode(hdc, HALFTONE);
                SelectObject(hdcMem, hBitmap1);
                StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
                SelectObject(hdcMem, hOverlayBitmap1);
                StretchBlt(hdc, overlay1X, overlay1Y, overlayW, overlayH, hdcMem, 0, 0, overlay1Bm.bmWidth, overlay1Bm.bmHeight, SRCCOPY);
                SelectObject(hdcMem, hOverlayBitmap2);
                StretchBlt(hdc, overlay2X, overlay1Y, overlayW, overlayH, hdcMem, 0, 0, overlay2Bm.bmWidth, overlay2Bm.bmHeight, SRCCOPY);
                SelectObject(hdcMem, hOverlayBitmap3);
                StretchBlt(hdc, overlay3X, overlay1Y, overlayW, overlayH, hdcMem, 0, 0, overlay3Bm.bmWidth, overlay3Bm.bmHeight, SRCCOPY);
                SelectObject(hdcMem, hOverlayBitmap4);
                StretchBlt(hdc, overlay4X, overlay1Y, overlayW, overlayH, hdcMem, 0, 0, overlay4Bm.bmWidth, overlay4Bm.bmHeight, SRCCOPY);
                if (mode == 1) {
                    SelectObject(hdcMem, hOverlayBitmap1);
                    StretchBlt(hdc, overlayX, overlayY, (screenWidth * 0.6), (screenHeight * 0.6), hdcMem, 0, 0, overlay1Bm.bmWidth, overlay1Bm.bmHeight, SRCCOPY);
                    SendMessage(ball1, STM_SETICON, (WPARAM)pink1, 0);
                    SendMessage(ball10, STM_SETICON, (WPARAM)pink10, 0);//设置图标
                    PrintfDebugString(L"set mode1 successfully\n");
                }
                ShowText(L'你', hdcMem, 100, 100, 100);
                DeleteDC(hdcMem);
            }
        }
        else {
            HDC hdcMem = CreateCompatibleDC(hdc);

            DeleteDC(hdcMem);
        }
        EndPaint(hWnd, &ps);
        PrintfDebugString(L"paint successfully\n");
    }break;
    case WM_LBUTTONDOWN: {
        SetStretchBltMode(hdc, HALFTONE);
        HDC hdcMem = CreateCompatibleDC(hdc);
        // 获取点击位置
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (x > overlay1X AND x<button1x AND y>overlay1Y AND y < buttony) {
            SelectObject(hdcMem, hOverlayBitmap1);
            StretchBlt(hdc, overlayX, overlayY, (screenWidth * 0.6), (screenHeight * 0.6), hdcMem, 0, 0, overlay1Bm.bmWidth, overlay1Bm.bmHeight, SRCCOPY);
            SendMessage(ball1, STM_SETICON, (WPARAM)pink1, 0);
            SendMessage(ball10, STM_SETICON, (WPARAM)pink10, 0);//设置图标
            mode = 1;
            UpdateWindow(hWnd);
        }
        if (x > overlay2X AND x<button2x AND y>overlay1Y AND y < buttony) {
            mode = 2;
            SelectObject(hdcMem, hOverlayBitmap2);
            StretchBlt(hdc, overlayX, overlayY, (screenWidth * 0.6), (screenHeight * 0.6), hdcMem, 0, 0, overlay2Bm.bmWidth, overlay2Bm.bmHeight, SRCCOPY);
            SendMessage(ball1, STM_SETICON, (WPARAM)pink1, 0);
            SendMessage(ball10, STM_SETICON, (WPARAM)pink10, 0);//设置图标
            UpdateWindow(hWnd);
        }
        if (x > overlay3X AND x<button3x AND y>overlay1Y AND y < buttony) {
            mode = 3;
            SelectObject(hdcMem, hOverlayBitmap3);
            StretchBlt(hdc, overlayX, overlayY, (screenWidth * 0.6), (screenHeight * 0.6), hdcMem, 0, 0, overlay3Bm.bmWidth, overlay3Bm.bmHeight, SRCCOPY);
            SendMessage(ball1, STM_SETICON, (WPARAM)pink1, 0);
            SendMessage(ball10, STM_SETICON, (WPARAM)pink10, 0);//设置图标
            UpdateWindow(hWnd);
        }
        if (x > overlay4X AND x<button4x AND y>overlay1Y AND y < buttony) {
            mode = 4;
            SelectObject(hdcMem, hOverlayBitmap4);
            StretchBlt(hdc, overlayX, overlayY, (screenWidth * 0.6), (screenHeight * 0.6), hdcMem, 0, 0, overlay4Bm.bmWidth, overlay4Bm.bmHeight, SRCCOPY);
            SendMessage(ball10, STM_SETICON, (WPARAM)blue10, 0);//设置图标
            SendMessage(ball1, STM_SETICON, (WPARAM)blue1, 0);
            UpdateWindow(hWnd);
        }
        DeleteDC(hdcMem);
    }break;
    case WM_KEYDOWN: switch (wParam) {
    case VK_ESCAPE: {
        PostQuitMessage(0);
        break;
    }
    default:
        break;
    }break;
    
    case WM_COMMAND: switch (LOWORD(wParam)){
        case BALL1: {
            if (star[number] == 5)is5star = 1;
            if (star[number] == 4)is4star = 1;
            mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
            if(is4star==0 AND is5star==0)
                play(".\\files\\video\\3star-single.mp4");
            if(is4star==1 AND is5star==0)
                play(".\\files\\video\\4star-single.mp4");
            if(is5star==1)
                play(".\\files\\video\\5star-single.mp4");
            is4star = 0;
            is5star = 0;
            mciSendString(L"play bgm repeat", NULL, 0, NULL); // 使用别名 'bgm' 播放音乐，并设置为循环播放
            break;
        }
        case BALL10: {
            for (int temp = number + 10; number <= temp; number++) {
                if (star[number] == 5)is5star = 1;
            }
            mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
            if (is5star == 1)
                play(".\\files\\video\\5star-multi.mp4");
            else if(is5star==0)
                play(".\\files\\video\\4star-multi.mp4");
            is5star = 0;
            is4star = 0;
            mciSendString(L"play bgm repeat", NULL, 0, NULL); // 使用别名 'bgm' 播放音乐，并设置为循环播放
            break;
        }
        default:
            break;
    }break;
    
    case WM_DESTROY: {
        // 释放资源并退出程序
        DeleteObject(blue10);
        DeleteObject(blue1);
        DeleteObject(pink1);
        DeleteObject(pink10);
        DeleteObject(hBitmap1);
        DeleteObject(hOverlayBitmap1);
        mciSendString(L"close bgm", NULL, 0, NULL); // 关闭音乐文件
        PostQuitMessage(0);
        break; }
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);//对不感兴趣的消息进行缺省处理，必须有该代码，否则程序有问题
	}
	return 0;
}

//main
int WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    {
        WNDCLASS wndcls; //创建一个窗体类
        wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
        wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
        wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
        wndcls.hCursor = LoadCursorW(NULL, IDC_CROSS);//设置光标
        wndcls.hIcon = LoadIconW(NULL, IDI_ERROR);//设置窗体左上角的图标
        wndcls.hInstance = hInstance;//设置窗体所属的应用程序实例
        wndcls.lpfnWndProc = WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
        wndcls.lpszClassName = L"test";//设置窗体的类名
        wndcls.lpszMenuName = NULL;//设置窗体的菜单,没有，填NULL
        wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
        RegisterClass(&wndcls);//向操作系统注册窗体
    }
    hInstance = hInstance_;
    initconfig();
    inittype();
    mode = std::stoi(getConfigValue("first mode"));
	hWnd = CreateWindowW(L"test", L"原神", WS_POPUP| WS_VISIBLE| WS_CLIPSIBLINGS| WS_OVERLAPPED| WS_CLIPCHILDREN, 0, 0, screenWidth, screenHeight, NULL, NULL, hInstance_, NULL);
	ShowWindow(hWnd, SW_SHOWNORMAL);//把窗体显示出来
    PrintfDebugString(L"initsucce");
	MSG msg;
	while (GetMessageW(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
    }

}