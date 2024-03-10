#pragma once
#include<Windows.h>
#include"getname.h"
#include "directshow.h"
#include"mywindows.h"
#include <cstdio>


#define BALL1 100
#define BALL10 101
#define MODE1 200 
#define EXIT 444
#define STAR L"E"

std::chrono::steady_clock::time_point start, end;
HANDLE random_handle;
DWORD random_;
HFONT text, icon,icon_star, text_big,text_mid,text_list;
HWND hVideoWnd, ball10, ball1, exitbotton; // 按钮句柄
HBITMAP hOverlayBitmap1, hBitmap1, hOverlayBitmap2, hOverlayBitmap3, hOverlayBitmap4, cardbg, blue1, blue10, pink1, pink10,buttom,goldencardbg,listbm, list4star, list5star, list6star, list3star;
BITMAP overlay1Bm, bm, ball, overlay2Bm, overlay3Bm, overlay4Bm, cardbg_, exitinfo_,goldenbg,listbm_,list4star_,list5star_,list6star_,list3star_;
HICON exitbuttom_;
ICONINFO exitinfo;
LPCWSTR names[256], temp[10];
HINSTANCE hInstance = NULL;
UINT_PTR timerID;// 在全局范围内定义计时器ID
int currentBitmap = 1;      // 当前显示的背景图片
const char* video;
wchar_t signame[10],*tmp;
bool firsttime = 1, is5star = 0, is4star = 0, firstmenu = 1, isball1 = 0, ing = 0, printing = 0, musicplayed = 0, offvideo = 0, isball10 = 0, ball10ing = 0,clicked=0,ft=0,initing=1;
int number = 0,printnum,star_[10];
int mode = 1, step = 0;
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
int ball10x, bally, ball1x, ball1end, ball10end, ballyend;
static int cx, cy, cxDib, cyDib;
int listx[11], listy=0,listxend, listyend = screenHeight;

DWORD static WINAPI RandomNumberGenerator();
void destoryall();
void showname1();
void showname10();
void printstars(HDC hdc ,int number);
void printfirstmenu(HDC hdc, HDC hdcMem);
void printnames(HDC hdc, HDC hdcMem);
char Lstrtosingal(LPCWSTR in);
void creatbuttom(HDC hdc, HDC hdcMem, int x, int y, LPCWSTR text_out);
void out10name(HDC hdc, HDC hdcMem);
void out1name(HDC hdc, HDC hdcMem);
void menu(HDC hdc, HDC hdcMem);

void out10name(HDC hdc, HDC hdcMem) {
    if (printing) {
        printnames(hdc, hdcMem);
        printstars(hdc, star_[step]);
        if (step == 9) {
            menu(hdc, hdcMem);
            printing = !printing;
            ing = 0;
            isball10 = 0;
            ball10ing = 0;
            step = 0;
        }
    }
    step++;
}

void menu(HDC hdc, HDC hdcMem) {
    SelectObject(hdcMem, listbm);
    StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcMem, 0, 0, listbm_.bmWidth, listbm_.bmHeight, SRCCOPY);
    LPCWSTR tmp_[10];
    int count, count1, count2, count3;
    int* itmp = find(star_, sizeof(star_) / sizeof(star[0]), 6, &count);
    if (itmp != NULL) for (int i = 0; i < count; ++i) tmp_[i] = temp[itmp[i]];
    itmp = find(star_, sizeof(star_) / sizeof(star[0]), 5, &count1);
    if (itmp != NULL) for (int i = 0; i < count1; ++i) tmp_[i + count] = temp[itmp[i]];
    itmp = find(star_, sizeof(star_) / sizeof(star[0]), 4, &count2);
    if (itmp != NULL) for (int i = 0; i < count2; ++i) tmp_[i + count + count1] = temp[itmp[i]];
    itmp = find(star_, sizeof(star_) / sizeof(star[0]), 3, &count3);
    if (itmp != NULL) for (int i = 0; i < count3; ++i) tmp_[i + count + count1 + count2] = temp[itmp[i]];
    SetTextColor(hdc, RGB(255, 255, 255));
    for (int i = 0; i < count; ++i) {
        Sleep(20);
        SelectObject(hdcMem, list6star);
        StretchBlt(hdc, listx[i], listy, listxend,listyend, hdcMem, 0, 0,list6star_.bmWidth, list6star_.bmHeight, SRCCOPY);
        SelectObject(hdc, text_list);
        SetBkColor(hdc, RGB(223, 228, 158));
        wchar_t a[2];
        a[1] = '\0';
        switch (Lstrtosingal(tmp_[i])) {
        case 2:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.33, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.46, tmp);
            break;
        case 3:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.28, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.43, tmp);
            a[0] = signame[2];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.58, tmp);
            break;
        }
    }    
    for (int i = count; i < count+count1; ++i) {
        Sleep(20);
        SelectObject(hdcMem, list5star);
        StretchBlt(hdc, listx[i], listy, listxend, listyend, hdcMem, 0, 0, list5star_.bmWidth, list5star_.bmHeight, SRCCOPY);
        SelectObject(hdc, text_list);
        SetBkColor(hdc, RGB(223, 228, 158));
        wchar_t a[2];
        a[1] = '\0';
        switch (Lstrtosingal(tmp_[i])) {
        case 2:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.33, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.46, tmp);
            break;
        case 3:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.28, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.43, tmp);
            a[0] = signame[2];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.58, tmp);
            break;
        }
    }
    for (int i = count + count1; i < count + count1 + count2; ++i) {
        Sleep(20);
        SelectObject(hdcMem, list4star);
        StretchBlt(hdc, listx[i], listy, listxend, listyend, hdcMem, 0, 0, list4star_.bmWidth, list4star_.bmHeight, SRCCOPY);
        SelectObject(hdc, text_list);
        SetBkColor(hdc, RGB(154, 130, 220));
        wchar_t a[2];
        a[1] = '\0';
        switch (Lstrtosingal(tmp_[i])) {
        case 2:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.33, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.46, tmp);
            break;
        case 3:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.28, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.43, tmp);
            a[0] = signame[2];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.58, tmp);
            break;
        }
    }
    for (int i = count + count1+count2; i < count + count1 + count2+count3; ++i) {
        Sleep(20);
        SelectObject(hdcMem, list3star);
        StretchBlt(hdc, listx[i], listy, listxend, listyend, hdcMem, 0, 0, list3star_.bmWidth, list3star_.bmHeight, SRCCOPY);
        SelectObject(hdc, text_list);
        SetBkColor(hdc, RGB(149, 157, 136));
        wchar_t a[2];
        a[1] = '\0';
        switch (Lstrtosingal(tmp_[i])) {
        case 2:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.33, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.46, tmp);
            break;
        case 3:
            a[0] = signame[0];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.28, tmp);
            a[0] = signame[1];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.43, tmp);
            a[0] = signame[2];
            tmp = &a[0];
            TextOut_(hdc, listx[i] + screenWidth * 0.01, listy + screenHeight * 0.58, tmp);
            break;
        }
    }
}
void out1name(HDC hdc,HDC hdcMem){
    if (printing) {
        printnames(hdc,hdcMem);
        printstars(hdc, star_[0]);
        printing = !printing;
    }
}

void printnames(HDC hdc,HDC hdcMem) {
    if (star_[step] < 5) {
        SelectObject(hdcMem, cardbg);
        StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcMem, 0, 0, cardbg_.bmWidth, cardbg_.bmHeight, SRCCOPY);
        SetTextColor(hdc, RGB(240, 240, 240));
        SetBkColor(hdc, RGB(21, 26, 38));
    }
    else {
        SelectObject(hdcMem, goldencardbg);
        StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcMem, 0, 0, goldenbg.bmWidth, goldenbg.bmHeight, SRCCOPY);
        SetTextColor(hdc, RGB(240, 240, 240));
        SetBkColor(hdc, RGB(69, 47, 29));
    }
    SelectObject(hdc, text_mid);
    TextOut_(hdc, screenWidth * 0.175, screenHeight * 8 / 13, temp[step]);
    SelectObject(hdc, text_big);
    wchar_t a[2];
    a[1] = '\0';
    switch (Lstrtosingal(temp[step])) {
    case 2:
        a[0] = signame[0];
        tmp = &a[0];
        TextOut_(hdc, screenWidth * 0.44, screenHeight * 0.26, tmp);
        a[0] = signame[1];
        tmp = &a[0];
        TextOut_(hdc, screenWidth * 0.44, screenHeight * 0.50, tmp);
        break;
    case 3:
        a[0] = signame[0];
        tmp = &a[0];
        TextOut_(hdc, screenWidth * 0.40, screenHeight * 0.26, tmp);
        a[0] = signame[1];
        tmp = &a[0];
        TextOut_(hdc, screenWidth * 0.50, screenHeight * 0.36, tmp);
        a[0] = signame[2];
        tmp = &a[0];
        TextOut_(hdc, screenWidth * 0.40, screenHeight * 0.46, tmp);
        break;
    }
}

void destoryall() {
    // 释放资源并退出程序
    DeleteObject(exitbuttom_);
    DeleteObject(blue10);
    DeleteObject(blue1);
    DeleteObject(pink1);
    DeleteObject(icon);
    DeleteObject(text);
    DeleteObject(pink10);
    DeleteObject(hBitmap1);
    DeleteObject(hOverlayBitmap1);
    mciSendString(L"close bgm", NULL, 0, NULL); // 关闭音乐文件
    mciSendString(L"close star3", NULL, 0, NULL); // 关闭音乐文件
    mciSendString(L"close star4", NULL, 0, NULL); // 关闭音乐文件
    mciSendString(L"close star5", NULL, 0, NULL); // 关闭音乐文件
    mciSendString(L"close starfull", NULL, 0, NULL); // 关闭音乐文件
    mciSendString(L"close listbgm", NULL, 0, NULL); // 关闭音乐文件
    cleanupConfigList();
    //relase_ferrtype();
    log("destory objects successfully");
}

void showname10() {
    printing = 1;
    ing = 1;
    ft = 1;
    for (int tmp = number + 10; number < tmp; number++) {
        temp[10-(tmp - number)] = names[number];
        star_[10-(tmp - number)] = star[number];
        if (star[number] == 5)is5star = 1;
        log("10balling number=%d,tmp=%d,star=%d", number, tmp,star_[tmp-number]);
    }
    mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
    if (!offvideo) {
        if (is5star == 1)
            play(".\\files\\video\\5star-multi.mp4");
        else if (is5star == 0)
            play(".\\files\\video\\4star-multi.mp4");
    }
    else
        mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
    is5star = 0;
    is4star = 0;
    firstmenu = 0;
    isball1 = 0;
    isball10 = 1;
    ball10ing = 1;
    InvalidateRect(hWnd, NULL, FALSE);
    log("finish ball 10,number is %d", number);
}

void printfirstmenu(HDC hdc,HDC hdcMem) {
    // 拉伸显示图片
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
    SelectObject(hdcMem, hOverlayBitmap1);
    StretchBlt(hdc, overlayX, overlayY, (screenWidth * 0.6), (screenHeight * 0.6), hdcMem, 0, 0, overlay1Bm.bmWidth, overlay1Bm.bmHeight, SRCCOPY);
    SelectObject(hdcMem, pink1);
    StretchBlt(hdc, ball1x, bally, ball.bmWidth, ball.bmHeight, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCCOPY);
    SelectObject(hdcMem, pink10);
    StretchBlt(hdc, ball10x, bally, ball.bmWidth, ball.bmHeight, hdcMem, 0, 0, ball.bmWidth, ball.bmHeight, SRCCOPY);
    SelectObject(hdc, text);
    SetTextColor(hdc, RGB(0, 0, 0));
    TextOut_(hdc, screenWidth * 0.075, screenHeight * 0.05, L"祈愿");
    SelectObject(hdc, icon);
    SetTextColor(hdc, RGB(211, 188, 142));
    TextOut_(hdc, screenWidth * 0.045, screenHeight * 0.04, L"r");
    firsttime = !firsttime;
}

char Lstrtosingal(LPCWSTR in) {
    // 创建一个迭代器，指向字符串的开始
    LPCWSTR it = in;
    int i = 0;
    for (LPCWSTR it = in; *it != L'\0'; it++) { 
        signame[i] = *it;
        i++;
    }
    return i;
}

void creatbuttom(HDC hdc, HDC hdcMem, int x, int y, LPCWSTR text_out) {
    SelectObject(hdcMem, buttom);
    StretchBlt(hdc, x, y, 100, 30, hdcMem, 0, 0, 123, 31, SRCCOPY);
    SelectObject(hdc, text);
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkColor(hdc, RGB(225, 222, 213));
    TextOut_(hdc, x+screenWidth*0.0073 , y+screenHeight*0.0065, text_out);
}

void printstars(HDC hdc, int number) {
    musicplayed = 1;
    if (number <= 4) {
        SetBkColor(hdc, RGB(21, 26, 38));
        SelectObject(hdc, icon_star);
        SetTextColor(hdc, RGB(220, 184, 14));
    }
    else {
        SetBkColor(hdc, RGB(53, 35, 27));
        SelectObject(hdc, icon_star);
        SetTextColor(hdc, RGB(220, 184, 14));
    }
    switch (number)
    {
    case 3: {
        mciSendString(L"close star3", NULL, 0, NULL); // 关闭音乐文件
        mciSendString(L"open .\\files\\mp3\\reveal-3star.mp3 alias star3", NULL, 0, NULL);
        mciSendString(L"play star3", 0, 0, 0);
        Sleep(2000);
        TextOut_(hdc, screenWidth * 0.177, screenHeight * 31 / 52, STAR);
        Sleep(200);
        TextOut_(hdc, screenWidth * 0.187, screenHeight * 31 / 52, STAR);
        Sleep(200);
        TextOut_(hdc, screenWidth * 0.197, screenHeight * 31 / 52, STAR);
    }break;
    case 4: {
        mciSendString(L"close star4", NULL, 0, NULL); // 关闭音乐文件
        mciSendString(L"open .\\files\\mp3\\reveal-4star.mp3 alias star4", NULL, 0, NULL);
        mciSendString(L"play star4", 0, 0, 0);
        Sleep(2100);
        TextOut_(hdc, screenWidth * 0.177, screenHeight * 31 / 52, STAR);
        Sleep(200);
        TextOut_(hdc, screenWidth * 0.187, screenHeight * 31 / 52, STAR);
        Sleep(200);
        TextOut_(hdc, screenWidth * 0.197, screenHeight * 31 / 52, STAR);
        Sleep(130);
        TextOut_(hdc, screenWidth * 0.207, screenHeight * 31 / 52, STAR);
    }break;
    case 5: {
        mciSendString(L"close star5", NULL, 0, NULL); // 关闭音乐文件
        mciSendString(L"open .\\files\\mp3\\reveal-5star.mp3 alias star5", NULL, 0, NULL);
        mciSendString(L"play star5", 0, 0, 0);
        Sleep(2150);
        TextOut_(hdc, screenWidth * 0.177, screenHeight * 31 / 52, STAR);
        Sleep(110);
        TextOut_(hdc, screenWidth * 0.187, screenHeight * 31 / 52, STAR);
        Sleep(170);
        TextOut_(hdc, screenWidth * 0.197, screenHeight * 31 / 52, STAR);
        Sleep(160);
        TextOut_(hdc, screenWidth * 0.207, screenHeight * 31 / 52, STAR);
        Sleep(145);
        TextOut_(hdc, screenWidth * 0.217, screenHeight * 31 / 52, STAR);
    }break;
    case 6: {
        mciSendString(L"close starfull", NULL, 0, NULL); // 关闭音乐文件
        mciSendString(L"open .\\files\\mp3\\reveal-fullstar.mp3 alias starfull", NULL, 0, NULL);
        mciSendString(L"play starfull", 0, 0, 0);
        Sleep(2150);
        int x, y;
        for (int i = 500; i > 0; i--) {
            x = randomIntegerBetween(0, screenWidth * 0.99);
            y = randomIntegerBetween(0, screenHeight * 0.99);
            TextOut_(hdc, x, y, STAR);
            Sleep(7);
        }
        mciSendString(L"stop starfull", 0, 0, 0);
    }break;
    default:
        break;
    }
}

void showname1() {
    printing = 1;
    ing = 1;
    number++;
    temp[0] = names[number];
    star_[0] = star[number];
    printnum = 1;
    if (star_[0] == 5)is5star = 1;
    if (star_[0] == 4)is4star = 1;
    if (!offvideo) {
        if (is4star == 0 AND is5star == 0)
            play(".\\files\\video\\3star-single.mp4");
        if (is4star == 1 AND is5star == 0)
            play(".\\files\\video\\4star-single.mp4");
        if (is5star == 1)
            play(".\\files\\video\\5star-single.mp4");
    }
    else
        mciSendString(L"stop bgm", NULL, 0, NULL); // 停止播放
    firstmenu = 0;
    isball1 = 1;
    isball10 = 0;
    InvalidateRect(hWnd, NULL, FALSE);
    is4star = 0;
    is5star = 0;
    step = 0;
    log("finish ball 1,number is %d", number);
}

DWORD static WINAPI RandomNumberGenerator() {
    for (unsigned int i = 0; i <= 255; i++) {
        names[i] = random(i);
    }
    log("random finish");
    return 0;
}

