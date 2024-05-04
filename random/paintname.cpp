#include "paintname.h"
#include"mywindows.h"
#include "directshow.h"
#include"config.h"
#include "getname.h"
#include"bitmaps.h"
#include "sth2sth.h"
#include "set-json.h"
#include"ui.h"

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
int paintname::type[10] = { 0 };
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

extern set2 setscreen;

void paintname::showname1() {
	directshow::music(CLICK);
	ui::printing = 1;
	ui::ing = 1;
	if (number >= 245)rerandom();
	temp[0] = names[ui::mode - 1][number];
	star_[0] = getname::star[ui::mode - 1][number];
	type[0] = getname::type[ui::mode - 1][number];
	randomedlist << randomedlist.pt() << "[" << star_[0] << "星]" << "[" << sth2sth::LWStostr(temp[0]) << "]" << randomedlist.nl();
	if (star_[0] >= 5)is5star = 1;
	if (star_[0] == 4)is4star = 1;
	if (!setscreen.offvideo) {
		if (is4star == 0 AND is5star == 0)
			directshow::play(config::getpath(SIGNALSTAR3));
		if (is4star == 1 AND is5star == 0)
			directshow::play(config::getpath(SIGNALSTAR4));
		if (is5star == 1)
			directshow::play(config::getpath(SIGNALSTAR5));
	}
	if (!setscreen.offmusic)
			mciSendString(L"stop bgm", NULL, 0, NULL);
	ui::screenmode = SHOW_NAMES_ING;
	ui::isball1 = 1;
	ui::isball10 = 0;
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
		type[10 - (tmp - number)] = getname::type[ui::mode - 1][number];
		if (getname::star[ui::mode - 1][number] >= 5)is5star = 1;
		mywindows::log("10balling number=%d,tmp=%d,star=%d", number, tmp, star_[10 - (tmp - number)]);
		randomedlist << "[" << star_[10 - (tmp - number)] << "星]" << "[" << sth2sth::LWStostr(temp[10 - (tmp - number)]) << "]" 
			<< "[" << type[10 - (tmp - number)] << "类型]" << std::endl;
	}
	randomedlist << "}" << randomedlist.nl();
	if (!setscreen.offmusic)
		mciSendString(L"stop bgm", NULL, 0, NULL);
	if (!setscreen.offvideo) {
		if (is5star == 1)
			directshow::play(config::getpath(GROUPSTAR5));
		else if (is5star == 0)
			directshow::play(config::getpath(GROUPSTAR4));
	}
	is5star = 0;
	is4star = 0;
	ui::screenmode = SHOW_NAMES_ING;
	ui::isball1 = 0;
	ui::isball10 = 1;
	ui::ball10ing = 1;
	mywindows::log("finish ball 10,number is %d", number);
	step = 0;
}
void paintname::out1name(Gp* p) {
	if (ui::printing) {
		printnames(p);
		if (star_[step] == 0) {
			star_[step] = 3;
		}
		printstars(p, star_[0]);
		ui::printing = !ui::printing;
	}
}
void paintname::out10name(Gp* p) {
	if (skipped) {
		menu(p);
		step = 0;
	}
	else if (ui::printing) {
		if (step == 10) {
			menu(p);
			step = 0;
			return;
		}
		printnames(p);
		if (star_[step] == 0) {
			star_[step] = 3;
		}
		printstars(p, star_[step]);
	}
	step++;
}
void paintname::printnames(Gp* p) {
	HDC hdc;
	if (star_[step] < 5) {
		p->Paint(0, 0, mywindows::windowWidth, mywindows::windowHeight, cardbackground);
		p->Paint(mywindows::windowWidth * 0.45, mywindows::windowHeight * 0.45, mywindows::windowWidth * 0.1, mywindows::windowHeight * 0.1, type[step] + 21);
		hdc = p->GetDC();
		SetTextColor(hdc, RGB(240, 240, 240));
	}
	else {
		p->Paint(0, 0, mywindows::windowWidth, mywindows::windowHeight, goldencardbg);
		p->Paint(mywindows::windowWidth * 0.45, mywindows::windowHeight * 0.45, mywindows::windowWidth * 0.1, mywindows::windowHeight * 0.1, type[step] + 21);
		hdc = p->GetDC();
		SetTextColor(hdc, RGB(240, 240, 240));
	}
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, ui::text_mid);
	if (temp[step] == NULL) {
		temp[step] = L"";
	}
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
	if (ui::mode == config::getint(SPECIAL))
		TextOut_(hdc, ui::addnamex, ui::addnamey, L"增加此名字几率");
	p->ReleaseDC(hdc);
}
void paintname::menu(Gp* p) {
	p->Paint(0, 0, mywindows::windowWidth, mywindows::windowHeight, listbg);
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
	for (int i = 0; i < count; ++i) {
		Sleep(20);
		p->Paint(ui::listx[i], ui::listy, ui::listxend, ui::listyend, list6);
		HDC hdc = p->GetDC();
		SetTextColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, ui::text_list);
		SetBkMode(hdc, TRANSPARENT);

		listpainter(tmp_[i], i, hdc);
		p->ReleaseDC(hdc);
	}
	for (int i = count; i < count + count1; ++i) {
		Sleep(20);
		p->Paint(ui::listx[i], ui::listy, ui::listxend, ui::listyend, list5);
		HDC hdc = p->GetDC();
		SetTextColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, ui::text_list);
		SetBkMode(hdc, TRANSPARENT);

		listpainter(tmp_[i], i, hdc);
		p->ReleaseDC(hdc);
	}
	for (int i = count + count1; i < count + count1 + count2; ++i) {
		Sleep(20);
		p->Paint(ui::listx[i], ui::listy, ui::listxend, ui::listyend, list4);
		HDC hdc = p->GetDC();
		SetTextColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, ui::text_list);
		SetBkMode(hdc, TRANSPARENT);

		listpainter(tmp_[i], i, hdc);
		p->ReleaseDC(hdc);
	}
	for (int i = count + count1 + count2; i < count + count1 + count2 + count3; ++i) {
		Sleep(20);
		p->Paint(ui::listx[i], ui::listy, ui::listxend, ui::listyend, list3);
		HDC hdc = p->GetDC();
		SetTextColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, ui::text_list);
		SetBkMode(hdc, TRANSPARENT);

		listpainter(tmp_[i], i, hdc);
		p->ReleaseDC(hdc);
	}
	skipped = 0;
	ui::ScreenModeChanged = 1;
	ui::printing = 0;
	ui::ing = 0;
	ui::isball10 = 0;
	ui::ball10ing = 0;
	step = 0;
}
void paintname::paint(Gp* p)
{
	ui::ScreenModeChanged = 0;
	if (ui::printing) {
		if (ui::isball1) {
			out1name(p);
			ui::ing = 0;
			ui::ScreenModeChanged = 1;
		}
		if (ui::isball10) {
			if (ui::ft) {
				if (!skipped)out10name(p);
				ui::ft = 0;
			}
			else if (ui::clicked)
				out10name(p);
			ui::ing = 0;
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
void paintname::printstars(Gp* p, int number) {
	HDC hdc = p->GetDC();
	SetBkMode(hdc, TRANSPARENT);

	TextOut_(hdc, -1201, -1234, STAR);
	if (number <= 4) {
		SelectObject(hdc, icon_star);
		SetTextColor(hdc, RGB(220, 184, 14));
	}
	else {
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
	p->ReleaseDC(hdc);
}
void paintname::rerandom() {
	for (short m = 0; m <= 3; m++)
		for (int i = 0; i < 256; i++)names[m][i] = 0;
	CloseHandle(random_handle);
	random_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RandomNumberGenerator, NULL, 0, NULL);
	setscreen.reran = 0;
	number = 0;
}
void paintname::addaname() {
	std::ofstream File;
	if (ui::mode == 1)File.open(config::getpath(NAMES1), std::ios::app);
	if (ui::mode == 2)File.open(config::getpath(NAMES2), std::ios::app);
	if (ui::mode == 3)File.open(config::getpath(NAMES3), std::ios::app);
	if (ui::mode == 4)File.open(config::getpath(NAMES4), std::ios::app);
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
	std::string a = sth2sth::wstr2stru(t);
	File << std::endl << a;
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
	HDC hdc = GetDC(mywindows::main_hwnd);
	SelectObject(hdc, icon_star);
	SetBkColor(hdc, RGB(53, 35, 27));
	SetTextColor(hdc, RGB(220, 184, 14));
	SetBkMode(hdc, TRANSPARENT);

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
	HDC hdc = GetDC(mywindows::main_hwnd);
	SelectObject(hdc, icon_star);
	switch (paintstarcount)
	{
	case 3:
		SetTextColor(hdc, RGB(220, 184, 14));
		SetBkMode(hdc, TRANSPARENT);

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
		SetTextColor(hdc, RGB(220, 184, 14));
		SetBkMode(hdc, TRANSPARENT);

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
		SetTextColor(hdc, RGB(220, 184, 14));
		SetBkMode(hdc, TRANSPARENT);

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

