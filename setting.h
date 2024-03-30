#pragma once
#include"mywindows.h"


int exitx, exitxend, exity, exityend,settingpage=1;
BITMAP exitbm,setbm_,setbu;
wchar_t string[3][32];
bool musicplayed = 0, offvideo = 0;
HWND textboxhwnd[5] = { NULL };
int textboxnumber = 0;

LPWSTR buffer = NULL;

typedef struct settingxy{
    int x, y, bmy, bmx, bmyend, bmxend,bmw,bmh;
};

settingxy sxy[21];

void switchbm(bool OnOrOff, const wchar_t* name, int number, HDC hdc, HDC hdcMem, HFONT text_mid);
void initsetting();
void paintsettingpage(HDC hdc, HDC hdcMem, HFONT text_mid);
LRESULT CALLBACK TextBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
void textbox(int number, const char* configname, LPCWSTR name, HDC hdc, HDC hdcMem,HFONT text_mid);

void paintsettingpage(HDC hdc, HDC hdcMem,HFONT text_mid) {
    SelectObject(hdcMem, hbitmaps[setbm]);
    StretchBlt(hdc, 0, 0, windowWidth, windowHeight, hdcMem, 0, 0, setbm_.bmWidth, setbm_.bmHeight, SRCCOPY);
    switch (settingpage)
    {
    case 1: {
        switchbm(fullscreen, L"全屏模式", 1, hdc, hdcMem, text_mid);
        switchbm(offvideo, L"关闭视频", 2, hdc, hdcMem, text_mid);
        textbox(11, "title name",L"标题名称",hdc,hdcMem,text_mid);
        textbox(12, "namesfile", L"名字文件", hdc, hdcMem, text_mid);
    }
    default:
        break;
    }
    SelectObject(hdcMem, hbitmaps[exitb]);
    StretchBlt(hdc, exitx, exity, exitxend - exitx, exityend - exity, hdcMem, 0, 0, exitbm.bmWidth, exitbm.bmHeight, SRCAND);
    SelectObject(hdcMem, hbitmaps[exiti]);
    StretchBlt(hdc, exitx, exity, exitxend - exitx, exityend - exity, hdcMem, 0, 0, exitbm.bmWidth, exitbm.bmHeight, SRCPAINT);
}

void switchbm(bool OnOrOff, const wchar_t* name, int number,HDC hdc,HDC hdcMem,HFONT text_mid) {
    SelectObject(hdc, text_mid);
    SetBkColor(hdc, RGB(236, 229, 216));
    SelectObject(hdcMem, hbitmaps[setbutton]);
    StretchBlt(hdc, sxy[number].bmx, sxy[number].bmy,sxy[number].bmw, sxy[number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
    TextOut_(hdc, sxy[number].x, sxy[number].y, name);
    if (OnOrOff == 1)TextOut_(hdc, sxy[number].bmx + windowWidth * 0.04, sxy[number].bmy + windowHeight * 0.01, L"开");
    else TextOut_(hdc, sxy[number].bmx + windowWidth * 0.04, sxy[number].bmy + windowHeight * 0.01, L"关");
}

void textbox(int number, const char* configname, LPCWSTR name, HDC hdc, HDC hdcMem, HFONT text_mid) {
    SelectObject(hdc, text_mid);
    SetBkColor(hdc, RGB(236, 229, 216));
    SelectObject(hdcMem, hbitmaps[setbutton]);
    StretchBlt(hdc, sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
    TextOut_(hdc, sxy[number].x, sxy[number].y, name);
    if (textboxhwnd[textboxnumber] == NULL) {
        textboxhwnd[textboxnumber] = CreateEditBox(hWnd, number, sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, getConfigValue(configname));
        textboxnumber++;
    }
}

void initsetting() {
    for (char i = 1; i <= 10; i++) {
        sxy[i].x = windowWidth * 0.05;
        sxy[i].y = windowHeight * (i + 1) * 0.09;
        sxy[i].bmx = windowWidth * 0.25;
        sxy[i].bmxend = windowWidth;
        sxy[i].bmy = sxy[i].y;
        sxy[i].bmyend = sxy[i].y + windowHeight * 0.07;
        sxy[i].bmw = windowWidth * 0.1;
        sxy[i].bmh = windowWidth * 0.03;
    }
    for (char i = 11; i <= 20; i++) {
        sxy[i].x = windowWidth * 0.55;
        sxy[i].y = windowHeight * (i-10 + 1) * 0.09;
        sxy[i].bmx = windowWidth * 0.75;
        sxy[i].bmxend = windowWidth;
        sxy[i].bmy = sxy[i].y;
        sxy[i].bmyend = sxy[i].y + windowHeight * 0.07;
        sxy[i].bmw = windowWidth * 0.1;
        sxy[i].bmh = windowWidth * 0.03;
    }
}

