#pragma once
#include"mywindows.h"
#include <Shlobj.h>
#include <tchar.h>
#include <Commctrl.h>

#pragma comment(lib, "comctl32.lib")
#define TEXTBOXHWNDNUMBER 10
#define BOTTONNUMBER 10
#define FIRST_MENU 1
#define SETTING 2
#define SHOW_NAMES_ING 3

typedef struct sNode {
    int number;
    int x;
    int y;
    int xend;
    int yend;
    struct sNode* next;
} sNode;

sNode *shead = NULL;
int exitx, exitxend, exity, exityend,settingpage=1;
BITMAP exitbm,setbm_,setbu;
wchar_t string[3][32];
bool musicplayed = 0, offvideo = 0, reran = 0, initing = 1;
HWND textboxhwnd[TEXTBOXHWNDNUMBER] = { NULL };
int screenmode=FIRST_MENU,textboxnumber = 0;
bool isused[TEXTBOXHWNDNUMBER+BOTTONNUMBER] = { 0 };
HWND bottomhwnd[BOTTONNUMBER] = { NULL };



typedef struct settingxy{
    int x, y, bmy, bmx, bmyend, bmxend,bmw,bmh;
};

settingxy sxy[21];

void switchbm(bool OnOrOff, const wchar_t* name, int number, HDC hdc, HDC hdcMem, HFONT text_mid);
void initsetting();
void paintsettingpage(HDC hdc, HDC hdcMem, HFONT text_mid);
void textbox(int ebnumber,int number, const char* configname, LPCWSTR name, HDC hdc, HDC hdcMem,HFONT text_mid,bool);
void OnButtonClick(HWND hwnd, LPCWSTR title, LPCWSTR filter, int number, const char* configname);
void quitsettingpage();
void settingkicked(int x, int y);


void paintsettingpage(HDC hdc, HDC hdcMem,HFONT text_mid) {
    SelectObject(hdcMem, hbitmaps[setbm]);
    StretchBlt(hdc, 0, 0, windowWidth, windowHeight, hdcMem, 0, 0, setbm_.bmWidth, setbm_.bmHeight, SRCCOPY);
    switch (settingpage)
    {
    case 1: {
        switchbm(fullscreen, L"全屏模式", 1, hdc, hdcMem, text_mid);
        switchbm(offvideo, L"关闭视频", 2, hdc, hdcMem, text_mid);
        textbox(1, 11, WINDOW_TITEL, L"标题名称", hdc, hdcMem, text_mid, 0);
        textbox(0, 12, SPECIAL, L"抽背卡池", hdc, hdcMem, text_mid, 0);
        textbox(2, 3, OVER1, L"卡池1图片", hdc, hdcMem, text_mid, 1);
        textbox(3, 13, OVER2, L"卡池2图片", hdc, hdcMem, text_mid, 1);
        textbox(4, 4, OVER3, L"卡池3图片", hdc, hdcMem, text_mid, 1);
        textbox(5, 14, OVER4, L"卡池4图片", hdc, hdcMem, text_mid, 1);
        textbox(6, 5, NAMES1, L"名字文件1", hdc, hdcMem, text_mid, 1);
        textbox(7, 15, NAMES2, L"名字文件2", hdc, hdcMem, text_mid, 1);
        textbox(8, 6, NAMES3, L"名字文件3", hdc, hdcMem, text_mid, 1);
        textbox(9, 16, NAMES4, L"名字文件4", hdc, hdcMem, text_mid, 1);
        switchbm(offmusic, L"关闭音乐", 7, hdc, hdcMem, text_mid);
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
    int x = sxy[number].bmx, y = sxy[number].bmy, xend = sxy[number].bmxend, yend = sxy[number].bmh + sxy[number].bmy;
    sNode* newnode = (sNode*)malloc(sizeof(sNode));
    if (newnode == NULL) {
        errlog("Memory allocation error(textbox)");
    };
    newnode->x = x;
    newnode->xend = xend;
    newnode->y = y;
    newnode->yend = yend;
    newnode->next = shead;
    newnode->number = number;
    shead = newnode;
}

void textbox(int ebnumber, int number, const char* configname, LPCWSTR name, HDC hdc, HDC hdcMem, HFONT text_mid,bool getfile) {
    SelectObject(hdc, text_mid);
    SetBkColor(hdc, RGB(236, 229, 216));
    SelectObject(hdcMem, hbitmaps[setbutton]);
    StretchBlt(hdc, sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
    TextOut_(hdc, sxy[number].x, sxy[number].y, name);
    std::string st;
    const char* cstd= getConfigValue(configname);
    st = cstd;
    if (isused[ebnumber] == 0) {
        isused[ebnumber] = 1;
        textboxhwnd[ebnumber] = CreateEditBox(hWnd, ebnumber, sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, UTF_82ASCII((st)).c_str());
    }
    if (getfile) {
        if (!isused[ebnumber + TEXTBOXHWNDNUMBER]) {
            SelectObject(hdc, text_mid);
            SetBkColor(hdc, RGB(236, 229, 216));
            SelectObject(hdcMem, hbitmaps[setbutton]);
            StretchBlt(hdc, sxy[number].bmxend, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
            TextOut_(hdc, sxy[number].bmxend+windowWidth*0.02, sxy[number].y, L"选择文件");
            int x = sxy[number].bmxend, y = sxy[number].bmy, xend = sxy[number].bmxend + sxy[number].bmw, yend = sxy[number].bmh + sxy[number].bmy;
            sNode *newnode=(sNode*)malloc(sizeof(sNode));
            if (newnode == NULL) {
                errlog("Memory allocation error(textbox)");
            };
            newnode->x = x;
            newnode->xend = xend;
            newnode->y = y;
            newnode->yend = yend;
            newnode->next = shead;
            newnode->number = number;
            shead = newnode;
        }
    }
}

void settingkicked(int x,int y) {
    sNode *current=shead;
    int number = -1;
    if (x >= exitx AND x <= exitxend AND y >= exity AND y <= exityend)quitsettingpage();
    while (current != NULL) {
        if (x >= current->x AND x <= current->xend AND y >= current->y AND y <= current->yend) { number = current->number; break; }
        current = current->next;
    }
    if (number != -1) {
        openmusic(CLICK);
        switch (number)
        {
        case 1: {
            fullscreen = !fullscreen;
            if (fullscreen) replaceConfigOption(INWINDOW, "0");
            else replaceConfigOption(INWINDOW, "1");
        }break;
        case 2:
            offvideo = !offvideo;
            if (offvideo)
                replaceConfigOption(OFF_VIDEO, "1");
            else replaceConfigOption(OFF_VIDEO, "0");
            break;
        case 3:
            OnButtonClick(hWnd, L"选择1卡池图片文件", L"bmp图片文件(*.bmp)\0 * .bmp\0", 2,OVER1);
            hbitmaps[over1] = (HBITMAP)LoadImageA(NULL, getConfigValue(OVER1), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            GetObject(hbitmaps[over1], sizeof(BITMAP), &overlay1Bm);
            break;
        case 4:
            OnButtonClick(hWnd, L"选择3卡池图片文件", L"bmp图片文件(*.bmp)\0 * .bmp\0", 4, OVER3);
            hbitmaps[over3] = (HBITMAP)LoadImageA(NULL, getConfigValue(OVER3), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            GetObject(hbitmaps[over3], sizeof(BITMAP), &overlay3Bm);
            break;
        case 13:
            OnButtonClick(hWnd, L"选择2卡池图片文件", L"bmp图片文件(*.bmp)\0 * .bmp\0", 3, OVER2);
            hbitmaps[over2] = (HBITMAP)LoadImageA(NULL, getConfigValue(OVER2), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            GetObject(hbitmaps[over2], sizeof(BITMAP), &overlay2Bm);
            break;
        case 14:
            OnButtonClick(hWnd, L"选择4卡池图片文件", L"bmp图片文件(*.bmp)\0 * .bmp\0", 5, OVER4);
            hbitmaps[over4] = (HBITMAP)LoadImageA(NULL, getConfigValue(OVER4), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            GetObject(hbitmaps[over4], sizeof(BITMAP), &overlay4Bm);
            break;
        case 5:
            OnButtonClick(hWnd, L"选择名字文件", L"姓名文件(*.txt)\0 * .txt\0", 6, NAMES1);
            reran = 1;
            break;
        case 15:
            OnButtonClick(hWnd, L"选择名字文件", L"姓名文件(*.txt)\0 * .txt\0", 7, NAMES2);
            reran = 1;
            break;
        case 6:
            OnButtonClick(hWnd, L"选择名字文件", L"姓名文件(*.txt)\0 * .txt\0", 8, NAMES3);
            reran = 1;
            break;
        case 16:
            OnButtonClick(hWnd, L"选择名字文件", L"姓名文件(*.txt)\0 * .txt\0", 9, NAMES4);
            reran = 1;
            break;
        case 7:
            offmusic = !offmusic;
            if (offmusic) {
                mciSendString(L"stop bgm", NULL, 0, NULL);
                replaceConfigOption(OFFMUSIC, "1");
            }
            else
            {
                mciSendString(L"play bgm repeat", NULL, 0, NULL);
                replaceConfigOption(OFFMUSIC, "0");
            }break;
        default:
            break;
        }
    }
}

void seteditbox(LPARAM lParam,WPARAM wParam) {
    // 获取文本框的句柄，确保它是有效的
    HWND editBoxHwnd = (HWND)(lParam);
    int numberoftextbox = LOWORD(wParam);
    if (editBoxHwnd != NULL) {
        // 分配缓冲区大小，这里假设文本框中的文本不会超过256个字符
        TCHAR sz[256];
        Edit_GetText(editBoxHwnd, sz, 256);
        // 显示文本框中的文本
        if (_tcslen(sz)!=0) {
            char* tmp = TCHAR2CHAR(sz);
            switch (numberoftextbox)
            {
            case 0:
                if (tmp != "") {
                    if (std::stoi(tmp) < 0 || std::stoi(tmp) > 4) {
                        MessageBox(NULL, L"输入一个0-4之间的数字（0表示禁用）", L"错误", MB_ICONERROR);
                        return;
                    }
                    else replaceConfigOption(SPECIAL, tmp);
                }
                break;
            case 1: 
                replaceConfigOption(WINDOW_TITEL, tmp); SetWindowTextW(hWnd, UTF8To16(tmp)); break;
            case 2:
                replaceConfigOption(OVER1, tmp); break;
            case 3:
                replaceConfigOption(OVER2, tmp); break;
            case 4:
                replaceConfigOption(OVER3, tmp); break;
            case 5:
                replaceConfigOption(OVER4, tmp); break;
            case 6:
                replaceConfigOption(NAMES1, tmp); break;
            case 7:
                replaceConfigOption(NAMES2, tmp); break;
            case 8:
                replaceConfigOption(NAMES3, tmp); break;
            case 9:
                replaceConfigOption(NAMES4, tmp); break;
            default:
                break;
            }
        }
    }
    initing = 1;
}

void initsetting() {
    for (char i = 1; i <= 10; i++) {
        sxy[i].x = windowWidth * 0.05;
        sxy[i].y = windowHeight * (i + 1) * 0.09;
        sxy[i].bmx = windowWidth * 0.25;
        sxy[i].bmxend = windowWidth * 0.35;
        sxy[i].bmy = sxy[i].y;
        sxy[i].bmyend = sxy[i].y + windowHeight * 0.07;
        sxy[i].bmw = windowWidth * 0.1;
        sxy[i].bmh = windowWidth * 0.03;
    }
    for (char i = 11; i <= 20; i++) {
        sxy[i].x = windowWidth * 0.55;
        sxy[i].y = windowHeight * (i-10 + 1) * 0.09;
        sxy[i].bmx = windowWidth * 0.75;
        sxy[i].bmxend = windowWidth * 0.85;
        sxy[i].bmy = sxy[i].y;
        sxy[i].bmyend = sxy[i].y + windowHeight * 0.07;
        sxy[i].bmw = windowWidth * 0.1;
        sxy[i].bmh = windowWidth * 0.03;
    }
}


// 假设这是在一个事件处理函数中，例如按钮点击事件
void OnButtonClick(HWND hwnd,LPCWSTR title,LPCWSTR filter,int number,const char* configname)
{
    loop:
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名
    ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小
    ofn.hwndOwner = hWnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
    ofn.lpstrFilter = filter/*L"名字文件\0*.txt\0所有文件\0*.*\0\0"*/;//设置过滤
    ofn.nFilterIndex = 1;//过滤器索引
    ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL
    ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
    ofn.lpstrInitialDir = NULL;//初始目录为默认
    ofn.lpstrTitle = title;//使用系统默认标题留空即可
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项
    if (GetOpenFileName(&ofn))
    {
        MessageBox(NULL, strFilename, TEXT("选择的文件"), 0);
        //replaceConfigOption(configname, LWStostr(strFilename).c_str());
        Edit_SetText(textboxhwnd[number], strFilename);
    }
    else {
        MessageBox(NULL, TEXT("请选择一个文件"), NULL, MB_ICONERROR);
        goto loop;
    }

}

void quitsettingpage() {
    openmusic(ENTER);
    char n = 0;
    while (textboxhwnd[n] != 0) {
        DestroyWindow(textboxhwnd[n]);
        n++;
    }
    n = 0;
    while (n <= BOTTONNUMBER) {
        DestroyWindow(bottomhwnd[n]);
        n++;
    }
    screenmode = FIRST_MENU;
    initing = 1;
    for (n = 0; n < TEXTBOXHWNDNUMBER + BOTTONNUMBER; n++)isused[n] = 0;
}