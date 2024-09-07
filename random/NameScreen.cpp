#include "NameScreen.h"
#include"mywindows.h"
#include "directshow.h"
#include"config.h"
#include "getname.h"
#include"bitmaps.h"
#include "sth2sth.h"
#include "set-json.h"
#include"ui.h"

Log randomedlist("name.txt", 1);

#define SKIP 0
#define addName 1

void CALLBACK TimerProc_(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void CALLBACK Case6TimerProc_(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void CALLBACK InitTimerProc_(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

LPCWSTR names[4][256]{};
int number_t;


void RandomFunc()
{
	getname::fileerr = 0;
	for (short m = 0; m <= 3; m++)
		for (int i = 0; i <= 255; i++) {
			names[m][i] = getname::random(m, i);
			if (getname::fileerr) { MessageBoxA(nullptr, "打开文件失败，请到设置中重新选择名字文件", "错误", MB_SYSTEMMODAL | MB_ICONERROR); return; }
		}
	mywindows::log("random finish");
	return;
}


NameScreen::NameScreen(Gp* p)
{
	this->p = p;
	for(auto& i:B)
	{
		i.setGp(p);
	}
	setButton();
}

NameScreen::NameScreen()
{
	p = nullptr;
	setButton();
}

NameScreen::~NameScreen()
{
	p = nullptr;
	g_StarTimerID = 0; // 定时器ID
	g_InitTimerID = 0; // case 6 定时器 ID
	InitTimerHwnd = nullptr;
	Case6TimerHwnd = nullptr;
	StarTimerHwnd = nullptr;
	firsttimeCase6 = 1;
	number = 0;
	tmp = nullptr;
	paintstarcount = 0;
	g_Case6Count = 0;
	g_StarCount = 0;
	step = 0;
	is5star = 0;
	is4star = 0;
	skipped = 0;
	skipbmx = 0;
	skipbmy = 0;
	skipbmxend = 0;
	skipbmyend = 0;
	random_handle = nullptr;
	icon_star = nullptr;
	for(auto& i:g_Case6TimerID)
	{
		i = 0;
	}
	for (auto& i : names)
	{
		for (auto& j : i)
		{
			j = nullptr;
		}
	}
	for(auto& i:star_)
	{
		i = 0;
	}
	for(auto& i:temp)
	{
		i = nullptr;
	}
	for(auto& i:type)
	{
		i = 0;
	}
}


void NameScreen::setButton()
{
	B[SKIP].setxy2WWWH(0.8, 0.045, 0.9, 0.045 + 0.17 * 0.17);
	B[SKIP].setText(L"跳过>>");
	B[SKIP].refresh();
	B[SKIP].setFont(&ui::text_mid);
	B[SKIP].bind([this] {skip(); });
	B[SKIP].setTextColor(255, 255, 255, 1);
	B[addName].setxy2WWWH(0.4, 0.8, 0.52, 0.836);
	B[addName].setText(L"增加此名字几率");
	B[addName].refresh();
	B[addName].setFont(&ui::text_mid);
	B[addName].bind([this] {addaname(); });
	B[addName].setTextColor(255, 230, 34, 1);
	for(auto & i:B)
	{
		i.setGp(p);
	}
}

void NameScreen::showname1() {
	directshow::music(CLICK_MUSIC);
	ui::printing = 1;
	ui::ing = 1;
	if (number >= 245)rerandom();
	temp[0] = names[ui::mode - 1][number];
	star_[0] = getname::star[ui::mode - 1][number];
	type[0] = getname::type_[ui::mode - 1][number];
	randomedlist << randomedlist.pt()<<"["<<type[0]<<"类型]" << 
		"[" << star_[0] << "星]" << "[" << sth2sth::LWStostr(temp[0]) << "]"
	<< randomedlist.nl();
	if (star_[0] >= 5)is5star = 1;
	if (star_[0] == 4)is4star = 1;
	if (!ui::SS.offvideo) {
		if (is4star == 0 AND is5star == 0)
			directshow::play(config::getpath(SIGNALSTAR3));
		if (is4star == 1 AND is5star == 0)
			directshow::play(config::getpath(SIGNALSTAR4));
		if (is5star == 1)
			directshow::play(config::getpath(SIGNALSTAR5));
	}
	else
	{
		InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
	}
	if (!ui::SS.offmusic)
			mciSendString(L"stop bgm", nullptr, 0, nullptr);
	ui::screenmode = SHOW_NAMES_ING;
	ui::isball1 = 1;
	ui::isball10 = 0;
	is4star = 0;
	is5star = 0;
	step = 0;
	number++;
	mywindows::log("finish ball 1,number is %d", number);
}
void NameScreen::showname10() {
	directshow::music(CLICK_MUSIC);
	ui::printing = 1;
	ui::ing = 1;
	ui::ft = 1;
	randomedlist << randomedlist.pt() << "{" << randomedlist.nl();
	for (const int tmp1 = number + 10; number < tmp1; number++) {
		temp[10 - (tmp1 - number)] = names[ui::mode - 1][number];
		star_[10 - (tmp1 - number)] = getname::star[ui::mode - 1][number];
		type[10 - (tmp1 - number)] = getname::type_[ui::mode - 1][number];
		if (getname::star[ui::mode - 1][number] >= 5)is5star = 1;
		mywindows::log("10balling number=%d,tmp=%d,star=%d", number, tmp1, star_[10 - (tmp1 - number)]);
		randomedlist << "[" << type[10-(tmp1-number)] << "类型]" <<
			"[" << star_[10 - (tmp1 - number)] << "星]" << "[" << 
			sth2sth::LWStostr(temp[10 - (tmp1 - number)]) << "]"
		<< std::endl;
	}
	randomedlist << "}" << randomedlist.nl();
	if (!ui::SS.offmusic)
		mciSendString(L"stop bgm", nullptr, 0, nullptr);
	if (!ui::SS.offvideo) {
		if (is5star == 1)
			directshow::play(config::getpath(GROUPSTAR5));
		else if (is5star == 0)
			directshow::play(config::getpath(GROUPSTAR4));
	}
	else
	{
		InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
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
void NameScreen::out1name() {
	if (ui::printing) {
		number_t = star_[0];
		printnames();
		if (star_[step] == 0) {
			star_[step] = 3;
		}
		printstars(star_[0]);
		ui::printing = !ui::printing;
	}
}

void NameScreen::skip()
{
	if (ui::ball10ing) {
		skipped = true;
		ui::clicked = true;
	}
}

void NameScreen::out10name() {
	if (skipped) {
		menu();
		step = 0;
	}
	else if (ui::printing) {
		if (step == 10) {
			menu();
			step = 0;
			return;
		}
		number_t = star_[step];
		printnames();
		if (star_[step] == 0) {
			star_[step] = 3;
		}
		printstars(star_[step]);
	}
	step++;
}

void NameScreen::printnames(){
	//std::thread a_thread([this] {music(); });
	//a_thread.detach();
	int t;
	mywindows::logf << "begin stopping star music" << std::endl;
	t=mciSendString(L"stop star3", 0, 0, 0);
	t=mciSendString(L"stop star4", 0, 0, 0);
	t=mciSendString(L"stop star5", 0, 0, 0);
	t=mciSendString(L"stop starfull", 0, 0, 0);
	if(t!=0)
	{
		wchar_t errstr[256];
		mciGetErrorStringW(t, errstr, sizeof(errstr));
		mywindows::errlogf<<errstr<<std::endl;
	}
	else
	{
		mywindows::logf << "stop star music success" << std::endl;
	}
	mywindows::logf << "begin play star music number_t="<<number_t << std::endl;
	switch (number_t)
	{
	case 3: {
		t=mciSendString(L"play star3 from 0", 0, 0, 0);
	}break;
	case 4: {
		t=mciSendString(L"play star4 from 0", 0, 0, 0);
	}break;
	case 5: {
		t=mciSendString(L"play star5 from 0", 0, 0, 0);
	}break;
	case 6: {
		t=mciSendString(L"play starfull from 0", 0, 0, 0);
	}
	default:
		break;
	}
	if (t != 0)
	{
		wchar_t errstr[256];
		mciGetErrorStringW(t, errstr, sizeof(errstr));
		mywindows::errlogf << errstr << std::endl;
	}
	else
	{
		mywindows::logf << "play star music success" << std::endl;
	}
	if (p) {
		p->Paint(0, 0, mywindows::WW, mywindows::WH, cardbackground);
		const int x = mywindows::WW * 0.25;
		const int w = mywindows::WW * 0.5;
		int y = mywindows::WH - w;
		y /= 2;
		const int h = w;
		p->Paint(x, y, w, h, type[step] + 21);
		const HDC hdc = p->GetDC();
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, ui::text_mid);
		if (temp[step] == nullptr) {
			temp[step] = L"";
		}
		SetTextColor(hdc, RGB(250, 250, 250));
		TextOut_(hdc, mywindows::WW * 0.175, mywindows::WH * 8 / 13, temp[step]);
		SelectObject(hdc, ui::text_big);
		wchar_t a[2];
		a[1] = '\0';
		static int r = config::getint(TEXTR);
		static int g = config::getint(TEXTG);
		static int b = config::getint(TEXTB);
		SetTextColor(hdc, RGB(r, g, b));
		switch (sth2sth::Lstrtosingal(temp[step])) {
		case 1:break;
		case 0:break;
		case 2:
			a[0] = sth2sth::signame[0];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.44, mywindows::WH * 0.26, tmp);
			a[0] = sth2sth::signame[1];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.44, mywindows::WH * 0.50, tmp);
			break;
		case 3:
			a[0] = sth2sth::signame[0];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.40, mywindows::WH * 0.26, tmp);
			a[0] = sth2sth::signame[1];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.50, mywindows::WH * 0.36, tmp);
			a[0] = sth2sth::signame[2];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.40, mywindows::WH * 0.46, tmp);
			break;
		case 4:
			a[0] = sth2sth::signame[0];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.40, mywindows::WH * 0.26, tmp);
			a[0] = sth2sth::signame[1];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.50, mywindows::WH * 0.26, tmp);
			a[0] = sth2sth::signame[2];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.40, mywindows::WH * 0.46, tmp);
			a[0] = sth2sth::signame[3];
			tmp = &a[0];
			TextOut_(hdc, mywindows::WW * 0.50, mywindows::WH * 0.46, tmp);
			break;
		default:
			TextOut_(hdc, mywindows::WW * 0.30, mywindows::WH * 0.36, temp[step]);
			break;
		}
		p->ReleaseDC(hdc);

		if (ui::ball10ing)
			B[SKIP].paint();
		if (ui::mode == config::getint(SPECIAL))
			B[addName].paint();
	}
}

void NameScreen::menu() {
	mciSendString(L"stop star3", 0, 0, 0);
	mciSendString(L"stop star4", 0, 0, 0);
	mciSendString(L"stop star5", 0, 0, 0);
	mciSendString(L"stop starfull", 0, 0, 0);
	if (p)
	{
		p->Paint(0, 0, mywindows::WW, mywindows::WH, listbg);
		LPCWSTR tmp_[10] = { nullptr };
		int count, count1, count2, count3;
		int* itmp = mywindows::find(star_, std::size(star_), 6, &count);
		if (itmp != nullptr) for (int i = 0; i < count; ++i) tmp_[i] = temp[itmp[i]];
		itmp = mywindows::find(star_, std::size(star_), 5, &count1);
		if (itmp != nullptr) for (int i = 0; i < count1; ++i) tmp_[i + count] = temp[itmp[i]];
		itmp = mywindows::find(star_, std::size(star_), 4, &count2);
		if (itmp != nullptr) for (int i = 0; i < count2; ++i) tmp_[i + count + count1] = temp[itmp[i]];
		itmp = mywindows::find(star_, std::size(star_), 3, &count3);
		if (itmp != nullptr) for (int i = 0; i < count3; ++i) tmp_[i + count + count1 + count2] = temp[itmp[i]];

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
}


void NameScreen::paint()
{
	ui::ScreenModeChanged = 0;
	if (ui::printing) {
		if (ui::isball1) {
			out1name();
			ui::ing = 0;
			ui::ScreenModeChanged = 1;
		}
		if (ui::isball10) {
			if (ui::ft) {
				if (!skipped)out10name();
				ui::ft = 0;
			}
			else if (ui::clicked)
				out10name();
			ui::ing = 0;
		}
	}
}

void NameScreen::resetPoint()
{
	for(auto& i:B)
	{
		i.refresh();
	}
}

void NameScreen::changeGp(Gp* p)
{
	this->p = p;
	for (auto& i : B)
	{
		i.setGp(this->p);
	}
}

void NameScreen::click(int x, int y)
{
	for(auto &i:B)
	{
		i.click(x, y);
	}
}

void NameScreen::listpainter(const LPCWSTR tmp_, const int i, const HDC hdc) {
	wchar_t a[2];
	a[1] = '\0';
	switch (sth2sth::Lstrtosingal(tmp_)) {
	case 1:break;
	case 0:break;
	case 2:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.33, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.46, tmp);
		break;
	case 3:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.28, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.40, tmp);
		a[0] = sth2sth::signame[2];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.52, tmp);
		break;
	case 4:
		a[0] = sth2sth::signame[0];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.20, tmp);
		a[0] = sth2sth::signame[1];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.33, tmp);
		a[0] = sth2sth::signame[2];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.46, tmp);
		a[0] = sth2sth::signame[3];
		tmp = &a[0];
		TextOut_(hdc, ui::listx[i] + mywindows::WW * 0.01, ui::listy + mywindows::WH * 0.59, tmp);
		break;
	default:
		MessageBox(nullptr, tmp_, L"这名字也太长了qwq", MB_SYSTEMMODAL | MB_ICONMASK);
		break;
	}
}
void NameScreen::printstars(const int star_number) {
	if (p) {
		HDC hdc = p->GetDC();
		SetBkMode(hdc, TRANSPARENT);

		TextOut_(hdc, -1201, -1234, STAR);
		if (star_number <= 4) {
			SelectObject(hdc, icon_star);
			SetTextColor(hdc, RGB(220, 184, 14));
		}
		else {
			SelectObject(hdc, icon_star);
			SetTextColor(hdc, RGB(220, 184, 14));
		}
		switch (star_number)
		{
		case 3: {
			//directshow::music(_1_3);
			paintstarcount = 3;
			g_InitTimerID = SetTimer(nullptr, 0, 2000, TIMERPROC(InitTimerProc_));
		}break;
		case 4: {
			//directshow::music(_1_4);
			paintstarcount = 4;
			g_InitTimerID = SetTimer(nullptr, 0, 2000, TIMERPROC(InitTimerProc_));
		}break;
		case 5: {
			//directshow::music(_1_5);
			paintstarcount = 5;
			g_InitTimerID = SetTimer(nullptr, 0, 2000, TIMERPROC(InitTimerProc_));
		}break;
		case 6: {
			//directshow::music(_1_6);
			firsttimeCase6 = 1;
			g_Case6Count = 500;
			g_Case6TimerID[0] = SetTimer(nullptr, 0, 2150, TIMERPROC(Case6TimerProc_));
		default:
			break;
		}
		}
		p->ReleaseDC(hdc);
	}
	number_t = star_number;
}
void NameScreen::rerandom() {
	for (short m = 0; m <= 3; m++)
		for (int i = 0; i < 256; i++)names[m][i] = nullptr;
	CloseHandle(random_handle);
	random_handle = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(RandomFunc), nullptr, 0, nullptr);
	ui::SS.reran = 0;
	number = 0;
}
void NameScreen::addaname() const
{
	std::ofstream File;
	if (ui::mode == 1)File.open(config::getpath(NAMES1), std::ios::app);
	if (ui::mode == 2)File.open(config::getpath(NAMES2), std::ios::app);
	if (ui::mode == 3)File.open(config::getpath(NAMES3), std::ios::app);
	if (ui::mode == 4)File.open(config::getpath(NAMES4), std::ios::app);
	LPCWSTR lpcwstr = nullptr;
	int sr = 3;
	if (ui::ball10ing) {
		sr = star_[step - 1];
		lpcwstr = temp[step - 1];
	}
	else {
		sr = star_[step];
		lpcwstr = temp[step];
	}
	std::wstring t = lpcwstr;
	t += L"-";
	t += std::to_wstring(sr);
	std::string a = sth2sth::wstr2stru(t);
	File << std::endl << a;
	MessageBoxW(nullptr, t.c_str(), L"info", MB_OK);
	File.close();
}

void CALLBACK NameScreen::InitTimerProc(const HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	InitTimerHwnd = hwnd;
	KillTimer(hwnd, g_InitTimerID); // 停止初始延时定时器
	g_InitTimerID = 0;
	g_StarTimerID = SetTimer(nullptr, 0, 200, TIMERPROC(TimerProc_)); // 启动星星定时器
}
void CALLBACK NameScreen::Case6TimerProc(const HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	Case6TimerHwnd = hwnd;
	if (firsttimeCase6) {
		KillTimer(hwnd, g_Case6TimerID[0]);
		KillTimer(hwnd, g_Case6TimerID[1]);
		KillTimer(hwnd, g_Case6TimerID[2]);
		g_Case6TimerID[0] = SetTimer(hwnd, 0, 0, TIMERPROC(Case6TimerProc_));
		g_Case6TimerID[1] = SetTimer(hwnd, 0, 0, TIMERPROC(Case6TimerProc_));
		g_Case6TimerID[2] = SetTimer(hwnd, 0, 0, TIMERPROC(Case6TimerProc_));
		firsttimeCase6 = 0;
	}
	HDC hdc = GetDC(mywindows::main_hwnd);
	SelectObject(hdc, icon_star);
	SetBkColor(hdc, RGB(53, 35, 27));
	SetTextColor(hdc, RGB(220, 184, 14));
	SetBkMode(hdc, TRANSPARENT);

	if (g_Case6Count > 0)
	{
		int x = getname::randomIntegerBetween(0, mywindows::WW * 0.99);
		int y = getname::randomIntegerBetween(0, mywindows::WH * 0.99);
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
void CALLBACK NameScreen::TimerProc(const HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
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
			TextOut_(hdc, mywindows::WW * 0.177, mywindows::WH * 31 / 52, STAR);
			break;
		case 1:
			TextOut_(hdc, mywindows::WW * 0.187, mywindows::WH * 31 / 52, STAR);
			break;
		case 2:
			TextOut_(hdc, mywindows::WW * 0.197, mywindows::WH * 31 / 52, STAR);
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
			TextOut_(hdc, mywindows::WW * 0.177, mywindows::WH * 31 / 52, STAR);
			break;
		case 1:
			TextOut_(hdc, mywindows::WW * 0.187, mywindows::WH * 31 / 52, STAR);
			break;
		case 2:
			TextOut_(hdc, mywindows::WW * 0.197, mywindows::WH * 31 / 52, STAR);
			break;
		case 3:
			TextOut_(hdc, mywindows::WW * 0.207, mywindows::WH * 31 / 52, STAR);
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
			TextOut_(hdc, mywindows::WW * 0.177, mywindows::WH * 31 / 52, STAR);
			break;
		case 1:
			TextOut_(hdc, mywindows::WW * 0.187, mywindows::WH * 31 / 52, STAR);
			break;
		case 2:
			TextOut_(hdc, mywindows::WW * 0.197, mywindows::WH * 31 / 52, STAR);
			break;
		case 3:
			TextOut_(hdc, mywindows::WW * 0.207, mywindows::WH * 31 / 52, STAR);
			break;
		case 4:
			TextOut_(hdc, mywindows::WW * 0.217, mywindows::WH * 31 / 52, STAR);
			KillTimer(hwnd, g_StarTimerID); // 停止定时器
			g_StarCount = -1; // 重置计数器
			break;
		default:
			break;
		}
		break;
	default: ;
	}
	g_StarCount++;
	ReleaseDC(hwnd, hdc);
}
void NameScreen::KillAllTimer()
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

void CALLBACK InitTimerProc_(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	ui::NS.InitTimerProc(hwnd, uMsg, idEvent, dwTime);
}

void CALLBACK Case6TimerProc_(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	ui::NS.Case6TimerProc(hwnd, uMsg, idEvent, dwTime);
}

void CALLBACK TimerProc_(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	ui::NS.TimerProc(hwnd, uMsg, idEvent, dwTime);
}
