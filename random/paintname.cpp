#include "paintname.h"
#include"mywindows.h"
#include"setting.h"
#include "directshow.h"
#include"config.h"
#include "getname.h"
#include"bitmaps.h"
#include "sth2sth.h"

UINT_PTR paintname::g_StarTimerID = 0; // 定时器ID
UINT_PTR paintname::g_Case6TimerID[3] = { 0 };
UINT_PTR paintname::g_InitTimerID = 0; // case 6 定时器 ID
HFONT paintname::icon_star = 0;
int paintname::g_Case6Count = 500;
int paintname::paintstarcount = 0;
int paintname::skipbmx=0;
int paintname::skipbmy=0;
int paintname::skipbmxend=0;
int paintname::skipbmyend=0;
int paintname::step=0;
int paintname::star_[10] = { 0 };
int paintname::g_StarCount=0;
bool paintname::is5star = 0;
bool paintname::is4star = 0;
bool paintname::skipped = 0;
LPCWSTR  paintname::temp[10] = { 0 };
LPCWSTR paintname::names[4][256] = { 0 };
int paintname::number = 0;
HANDLE paintname::random_handle=0;
wchar_t* paintname::tmp=nullptr;
Log randomedlist("name.txt", 1);
bool paintname::firsttimeCase6 = 1;
HWND paintname::InitTimerHwnd = 0;
HWND paintname::Case6TimerHwnd = 0;
HWND paintname::StarTimerHwnd = 0;

extern HBITMAP hbitmaps[BitmapCounts];
extern BITMAP overlay1Bm, bm, ball, overlay2Bm, overlay3Bm, overlay4Bm, cardbg_, exitinfo_, goldenbg, listbm_, list4star_, list5star_, list6star_, list3star_, buttom_;

void paintname::showname1() {
	directshow::music(CLICK);
	ui::printing = 1;
	ui::ing = 1;
	if (number >= 245)rerandom();
	temp[0] = names[ui::mode - 1][number];
	star_[0] = getname::star[ui::mode - 1][number];
	randomedlist << randomedlist.pt() << "[" << star_[0] << "星]" << "[" << sth2sth::LWStostr(temp[0]) << "]" << randomedlist.nl();
	if (star_[0] >= 5)is5star = 1;
	if (star_[0] == 4)is4star = 1;
	if (!setting::offvideo) {
		if (is4star == 0 AND is5star == 0)
			directshow::play(config::get(SIGNALSTAR3));
		if (is4star == 1 AND is5star == 0)
			directshow::play(config::get(SIGNALSTAR4));
		if (is5star == 1)
			directshow::play(config::get(SIGNALSTAR5));
	}
	if (!setting::offmusic)
			mciSendString(L"stop bgm", NULL, 0, NULL);
	ui::screenmode = SHOW_NAMES_ING;
	ui::isball1 = 1;
	ui::isball10 = 0;
	//InvalidateRect(mywindows::hWnd, NULL, FALSE);
	is4star = 0;
	is5star = 0;
	step = 0;
	number++;
	mywindows::log("finish ball 1,number is %d", number);
}
void paintname::showname10() {
	directshow::music(CLICK);
	ui::printing = 1;
	ui::ing = 1;
	ui::ft = 1;
	randomedlist << randomedlist.pt() << "{" << randomedlist.nl();
	for (int tmp = number + 10; number < tmp; number++) {
		temp[10 - (tmp - number)] = names[ui::mode - 1][number];
		star_[10 - (tmp - number)] = getname::star[ui::mode - 1][number];
		if (getname::star[ui::mode - 1][number] >= 5)is5star = 1;
		mywindows::log("10balling number=%d,tmp=%d,star=%d", number, tmp, star_[10 - (tmp - number)]);
		randomedlist << "[" << star_[10 - (tmp - number)] << "星]" << "[" << sth2sth::LWStostr(temp[10 - (tmp - number)]) << "]" << randomedlist.nl();
	}
	randomedlist << "}" << randomedlist.nl();
	if (!setting::offmusic)
		mciSendString(L"stop bgm", NULL, 0, NULL);
	if (!setting::offvideo) {
		if (is5star == 1)
			directshow::play(config::get(GROUPSTAR5));
		else if (is5star == 0)
			directshow::play(config::get(GROUPSTAR4));
	}
	if (!setting::offmusic)
		mciSendString(L"stop bgm", NULL, 0, NULL);
	is5star = 0;
	is4star = 0;
	ui::screenmode = SHOW_NAMES_ING;
	ui::isball1 = 0;
	ui::isball10 = 1;
	ui::ball10ing = 1;
	//InvalidateRect(mywindows::hWnd, NULL, FALSE);
	mywindows::log("finish ball 10,number is %d", number);
	step = 0;
}
void paintname::out1name(HDC hdc, HDC hdcMem) {
	if (ui::printing) {
		printnames(hdc, hdcMem);
		printstars(hdc, star_[0]);
		ui::printing = !ui::printing;
	}
}
void paintname::out10name(HDC hdc, HDC hdcMem) {
	if (skipped) {
		menu(hdc, hdcMem);
	}
	else if (ui::printing) {
		if (step == 10) {
			menu(hdc, hdcMem);
			step = 0;
			return;
		}
		printnames(hdc, hdcMem);
		printstars(hdc, star_[step]);
	}
	step++;
}
void paintname::printnames(HDC hdc, HDC hdcMem) {
	if (star_[step] < 5) {
		SelectObject(hdcMem, hbitmaps[cardbackground]);
		StretchBlt(hdc, 0, 0, mywindows::windowWidth, mywindows::windowHeight, hdcMem, 0, 0, cardbg_.bmWidth, cardbg_.bmHeight, SRCCOPY);
		SetTextColor(hdc, RGB(240, 240, 240));
		SetBkColor(hdc, RGB(21, 26, 38));
	}
	else {
		SelectObject(hdcMem, hbitmaps[goldencardbg]);
		StretchBlt(hdc, 0, 0, mywindows::windowWidth, mywindows::windowHeight, hdcMem, 0, 0, goldenbg.bmWidth, goldenbg.bmHeight, SRCCOPY);
		SetTextColor(hdc, RGB(240, 240, 240));
		SetBkColor(hdc, RGB(69, 47, 29));
	}
	SelectObject(hdc, ui::text_mid);
	TextOut_(hdc, mywindows::windowWidth * 0.175, mywindows::windowHeight * 8 / 13, temp[step]);
	SelectObject(hdc, ui::text_big);
	wchar_t a[2];
	a[1] = '\0';
	switch (sth2sth::Lstrtosingal(temp[step])) {
	case 1:break;
	case 0:break;
	case 2:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.44, mywindows::windowHeight * 0.26, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.44, mywindows::windowHeight * 0.50, tmp);
		break;
	case 3:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.40, mywindows::windowHeight * 0.26, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.50, mywindows::windowHeight * 0.36, tmp);
		a[0] = sth2sth::signame[2];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.40, mywindows::windowHeight * 0.46, tmp);
		break;
	case 4:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.40, mywindows::windowHeight * 0.26, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.50, mywindows::windowHeight * 0.26, tmp);
		a[0] = sth2sth::signame[2];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.40, mywindows::windowHeight * 0.46, tmp);
		a[0] = sth2sth::signame[3];
		tmp = &a[0];
		TextOut_(hdc, mywindows::windowWidth * 0.50, mywindows::windowHeight * 0.46, tmp);
		break;
	default:
		TextOut_(hdc, mywindows::windowWidth * 0.30, mywindows::windowHeight * 0.36, temp[step]);
		break;
	}
	SelectObject(hdc, ui::text_mid);
	SetTextColor(hdc, RGB(255, 255, 255));
	if (ui::ball10ing)
		TextOut_(hdc, skipbmx, skipbmy, L"跳过>>");
	if (ui::mode == std::stoi(config::get(SPECIAL)))
		TextOut_(hdc, ui::addnamex, ui::addnamey, L"增加此名字几率");
}
void paintname::printstars(HDC hdc, int number) {
	TextOut_(hdc, -1201, -1234, STAR);
	setting::musicplayed = 1;
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
		directshow::music(_1_3);
		paintstarcount = 3;
		g_InitTimerID = SetTimer(NULL, 0, 2000, InitTimerProc);
	}break;
	case 4: {
		directshow::music(_1_4);
		paintstarcount = 4;
		g_InitTimerID = SetTimer(NULL, 0, 2000, InitTimerProc);
	}break;
	case 5: {
		directshow::music(_1_5);
		paintstarcount = 5;
		g_InitTimerID = SetTimer(NULL, 0, 2000, InitTimerProc);
	}break;
	case 6: {
		directshow::music(_1_6);
		firsttimeCase6 = 1;
		g_Case6Count = 500;
		g_Case6TimerID[0] = SetTimer(NULL, 0, 2150, Case6TimerProc);
	default:
		break;
	}
	}
}
void paintname::listpainter(LPCWSTR tmp_, int i, HDC hdc) {
	wchar_t a[2];
	a[1] = '\0';
	switch (sth2sth::Lstrtosingal(tmp_)) {
	case 1:break;
	case 0:break;
	case 2:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.33, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.46, tmp);
		break;
	case 3:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.28, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.43, tmp);
		a[0] = sth2sth::signame[2];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.58, tmp);
		break;
	case 4:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.20, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.35, tmp);
		a[0] = sth2sth::signame[2];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.50, tmp);
		a[0] = sth2sth::signame[3];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::windowWidth * 0.01, ui::listy + mywindows::windowHeight * 0.65, tmp);
		break;
	default:
		MessageBox(NULL, tmp_, L"这名字也太长了qwq", MB_SYSTEMMODAL | MB_ICONMASK);
		break;
	}
}
void paintname::menu(HDC hdc, HDC hdcMem) {
	SelectObject(hdcMem, hbitmaps[listbg]);
	StretchBlt(hdc, 0, 0, mywindows::windowWidth, mywindows::windowHeight, hdcMem, 0, 0, listbm_.bmWidth, listbm_.bmHeight, SRCCOPY);
	LPCWSTR tmp_[10] = { 0 };
	int count, count1, count2, count3;
	int* itmp = mywindows::find(star_, sizeof(star_) / sizeof(getname::star[ui::mode - 1][0]), 6, &count);
	if (itmp != NULL) for (int i = 0; i < count; ++i) tmp_[i] = temp[itmp[i]];
	itmp = mywindows::find(star_, sizeof(star_) / sizeof(getname::star[ui::mode - 1][0]), 5, &count1);
	if (itmp != NULL) for (int i = 0; i < count1; ++i) tmp_[i + count] = temp[itmp[i]];
	itmp = mywindows::find(star_, sizeof(star_) / sizeof(getname::star[ui::mode - 1][0]), 4, &count2);
	if (itmp != NULL) for (int i = 0; i < count2; ++i) tmp_[i + count + count1] = temp[itmp[i]];
	itmp = mywindows::find(star_, sizeof(star_) / sizeof(getname::star[ui::mode - 1][0]), 3, &count3);
	if (itmp != NULL) for (int i = 0; i < count3; ++i) tmp_[i + count + count1 + count2] = temp[itmp[i]];
	SetTextColor(hdc, RGB(255, 255, 255));
	for (int i = 0; i < count; ++i) {
		Sleep(20);
		SelectObject(hdcMem, hbitmaps[list6]);
		StretchBlt(hdc, ui::listx[i], ui::listy, ui::listxend, ui::listyend, hdcMem, 0, 0, list6star_.bmWidth, list6star_.bmHeight, SRCCOPY);
		SelectObject(hdc, ui::text_list);
		SetBkColor(hdc, RGB(223, 228, 158));
		listpainter(tmp_[i], i, hdc);
	}
	for (int i = count; i < count + count1; ++i) {
		Sleep(20);
		SelectObject(hdcMem, hbitmaps[list5]);
		StretchBlt(hdc, ui::listx[i], ui::listy, ui::listxend, ui::listyend, hdcMem, 0, 0, list5star_.bmWidth, list5star_.bmHeight, SRCCOPY);
		SelectObject(hdc, ui::text_list);
		SetBkColor(hdc, RGB(223, 228, 158));
		listpainter(tmp_[i], i, hdc);
	}
	for (int i = count + count1; i < count + count1 + count2; ++i) {
		Sleep(20);
		SelectObject(hdcMem, hbitmaps[list4]);
		StretchBlt(hdc, ui::listx[i], ui::listy, ui::listxend, ui::listyend, hdcMem, 0, 0, list4star_.bmWidth, list4star_.bmHeight, SRCCOPY);
		SelectObject(hdc, ui::text_list);
		SetBkColor(hdc, RGB(154, 130, 220));
		listpainter(tmp_[i], i, hdc);
	}
	for (int i = count + count1 + count2; i < count + count1 + count2 + count3; ++i) {
		Sleep(20);
		SelectObject(hdcMem, hbitmaps[list3]);
		StretchBlt(hdc, ui::listx[i], ui::listy, ui::listxend, ui::listyend, hdcMem, 0, 0, list3star_.bmWidth, list3star_.bmHeight, SRCCOPY);
		SelectObject(hdc, ui::text_list);
		SetBkColor(hdc, RGB(149, 157, 136));
		listpainter(tmp_[i], i, hdc);
	}
	skipped = 0;
	setting::initing = 1;
	ui::printing = 0;
	ui::ing = 0;
	ui::isball10 = 0;
	ui::ball10ing = 0;
	step = 0;
}
void paintname::rerandom() {
	for (short m = 0; m <= 3; m++)
		for (int i = 0; i < 256; i++)names[m][i] = 0;
	CloseHandle(random_handle);
	random_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RandomNumberGenerator, NULL, 0, NULL);
	setting::reran = 0;
	number = 0;
}
void paintname::addaname() {
	std::wofstream File;
	if (ui::ui::mode == 1)File.open(config::get(NAMES1), std::ios::app);
	if (ui::ui::mode == 2)File.open(config::get(NAMES2), std::ios::app);
	if (ui::ui::mode == 3)File.open(config::get(NAMES3), std::ios::app);
	if (ui::ui::mode == 4)File.open(config::get(NAMES4), std::ios::app);
	LPCWSTR tmp = NULL;
	int sr = 3;
	if (ui::ball10ing) {
		sr = star_[step - 1];
		tmp = temp[step - 1];
	}
	else {
		sr = star_[step];
		tmp = temp[step];
	}
	std::wstring t = tmp;
	t += L"-";
	t += std::to_wstring(sr);
	File << std::endl << t;
	MessageBoxW(NULL, t.c_str(), L"info", MB_OK);
	File.close();
}
DWORD WINAPI paintname::RandomNumberGenerator() {
	getname::fileerr = 0;
	for (short m = 0; m <= 3; m++)
		for (unsigned int i = 0; i <= 255; i++) {
			names[m][i] = getname::random(m, i);
			if (getname::fileerr) { MessageBoxA(NULL, "打开文件失败，请到设置中重新选择名字文件", "错误", MB_SYSTEMMODAL | MB_ICONERROR); return -1; }
		}
	mywindows::log("random finish");
	return 0;
}
void CALLBACK paintname::InitTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	InitTimerHwnd = hwnd;
	KillTimer(hwnd, g_InitTimerID); // 停止初始延时定时器
	g_InitTimerID = 0;
	g_StarTimerID = SetTimer(NULL, 0, 200, TimerProc);
	// 其他 case...
}
void CALLBACK paintname::Case6TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	Case6TimerHwnd = hwnd;
	if (firsttimeCase6) {
		KillTimer(hwnd, g_Case6TimerID[0]);
		KillTimer(hwnd, g_Case6TimerID[1]);
		KillTimer(hwnd, g_Case6TimerID[2]);
		g_Case6TimerID[0] = SetTimer(hwnd, 0, 0.3, Case6TimerProc);
		g_Case6TimerID[1] = SetTimer(hwnd, 0, 0.3, Case6TimerProc);
		g_Case6TimerID[2] = SetTimer(hwnd, 0, 0.3, Case6TimerProc);
		firsttimeCase6 = 0;
	}
	HDC hdc = GetDC(mywindows::hWnd);
	SelectObject(hdc, icon_star);
	SetBkColor(hdc, RGB(53, 35, 27));
	SetTextColor(hdc, RGB(220, 184, 14));
	if (g_Case6Count > 0)
	{
		int x = getname::randomIntegerBetween(0, mywindows::windowWidth * 0.99);
		int y = getname::randomIntegerBetween(0, mywindows::windowHeight * 0.99);
		TextOut_(hdc, x, y, STAR);
		g_Case6Count--;
	}
	else
	{
		firsttimeCase6 = 1;
		KillTimer(hwnd, g_Case6TimerID[0]); // 停止定时器
		KillTimer(hwnd, g_Case6TimerID[1]); // 停止定时器
		KillTimer(hwnd, g_Case6TimerID[2]); // 停止定时器
		g_Case6TimerID[0] = 0;
		g_Case6TimerID[1] = 0;
		g_Case6TimerID[2] = 0;
	}

	ReleaseDC(hwnd, hdc);
}
void CALLBACK paintname::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	StarTimerHwnd = hwnd;
	HDC hdc = GetDC(mywindows::hWnd);
	SelectObject(hdc, icon_star);
	switch (paintstarcount)
	{
	case 3:
		SetBkColor(hdc, RGB(21, 26, 38));
		SetTextColor(hdc, RGB(220, 184, 14));
		switch (g_StarCount)
		{
		case 0:
			TextOut_(hdc, mywindows::windowWidth * 0.177, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 1:
			TextOut_(hdc, mywindows::windowWidth * 0.187, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 2:
			TextOut_(hdc, mywindows::windowWidth * 0.197, mywindows::windowHeight * 31 / 52, STAR);
			KillTimer(hwnd, g_StarTimerID); // 停止定时器
			g_StarCount = -1; // 重置计数器
			break;
		default:
			break;
		}
		break;
	case 4:
		SetBkColor(hdc, RGB(21, 26, 38));
		SetTextColor(hdc, RGB(220, 184, 14));
		switch (g_StarCount)
		{
		case 0:
			TextOut_(hdc, mywindows::windowWidth * 0.177, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 1:
			TextOut_(hdc, mywindows::windowWidth * 0.187, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 2:
			TextOut_(hdc, mywindows::windowWidth * 0.197, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 3:
			TextOut_(hdc, mywindows::windowWidth * 0.207, mywindows::windowHeight * 31 / 52, STAR);
			KillTimer(hwnd, g_StarTimerID); // 停止定时器
			g_StarCount = -1; // 重置计数器
			break;
		default:
			break;
		}
		break;
	case 5:
		SetBkColor(hdc, RGB(53, 35, 27));
		SetTextColor(hdc, RGB(220, 184, 14));
		switch (g_StarCount)
		{
		case 0:
			TextOut_(hdc, mywindows::windowWidth * 0.177, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 1:
			TextOut_(hdc, mywindows::windowWidth * 0.187, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 2:
			TextOut_(hdc, mywindows::windowWidth * 0.197, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 3:
			TextOut_(hdc, mywindows::windowWidth * 0.207, mywindows::windowHeight * 31 / 52, STAR);
			break;
		case 4:
			TextOut_(hdc, mywindows::windowWidth * 0.217, mywindows::windowHeight * 31 / 52, STAR);
			KillTimer(hwnd, g_StarTimerID); // 停止定时器
			g_StarCount = -1; // 重置计数器
			break;
		default:
			break;
		}
		break;
	}
	g_StarCount++;
	ReleaseDC(hwnd, hdc);
}
void paintname::KillAllTimer()
{
	KillTimer(InitTimerHwnd, g_InitTimerID);
	KillTimer(StarTimerHwnd, g_StarTimerID);
	KillTimer(Case6TimerHwnd, g_Case6TimerID[0]);
	KillTimer(Case6TimerHwnd, g_Case6TimerID[1]);
	KillTimer(Case6TimerHwnd, g_Case6TimerID[2]);
	g_Case6TimerID[0] = 0;
	g_Case6TimerID[1] = 0;
	g_Case6TimerID[2] = 0;
	g_InitTimerID = 0;
	g_StarTimerID = 0;
	g_StarCount = 0;
}