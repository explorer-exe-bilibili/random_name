#include "init.h"
#include "ui.h"
#include<Windows.h>
#include"paintname.h"
#include"mywindows.h"
#include"set-json.h"
#include"config.h"
#include<filesystem>
#include <functional>

#include <gdiplus.h>

#include"resource.h"
#include"log.h"
#include<thread>
#include <dshow.h>

extern set2 setscreen;
WNDPROC w2_, w3_;

void init::font()
{
	int desiredPixelHeight = mywindows::windowWidth * 0.17;
	// 获取设备上下文的 DPI
	HDC hdc = GetDC(NULL); // 获取桌面设备上下文
	int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	// 计算逻辑单位高度
	int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
	int logicalweidth = logicalHeight * 0.77;
	bool isInstall;
	isInstall= cheakIsFontInstalled(L"genshin-icon");
	if (isInstall) {
		paintname::icon_star = CreateFontW(logicalHeight * 0.0862, logicalweidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon_mid = CreateFontW(logicalHeight * 0.16, logicalweidth * 0.22, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon = CreateFontW(logicalHeight * 0.2299, logicalweidth * 0.3008, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
	}
	else {
		MessageBox(NULL, L"请安装字体", L"错误", MB_ICONERROR);
		mywindows::errlogf << "\"genshin - icon\"字体未安装" << std::endl;
		system("files\\ttfs\\icon.ttf");
		mywindows::reboot();
	}
	isInstall = cheakIsFontInstalled(L"SDK_SC_Web");
	if(isInstall){
		ui::text = CreateFontW(logicalHeight * 0.1149, logicalweidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ui::text_mid = CreateFontW(logicalHeight * 0.1724, logicalweidth * 0.1729, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ui::text_list = CreateFontW(logicalHeight * 0.7, logicalweidth * 0.7, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
	}
	else {
		MessageBox(NULL, L"请安装字体", L"错误", MB_ICONERROR);
		mywindows::errlogf << "SDK_SC_Web字体未安装" << std::endl;
		system("files\\ttfs\\text.ttf");
		mywindows::reboot();
	}
	if (!config::getint(UNSUITFONT)) {
		isInstall = cheakIsFontInstalled(L"Aa漆书");
		if (isInstall) {
			ui::text_big = CreateFontW(logicalHeight, logicalweidth, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Aa漆书");
		}
		else {
			MessageBox(NULL, L"请安装字体", L"错误", MB_ICONERROR);
			mywindows::errlogf << "Aa漆书字体未安装" << std::endl;
			system("files\\ttfs\\QS.ttf");
			mywindows::reboot();
		}
	}
	else {
		ui::text_big = CreateFontW(logicalHeight, logicalweidth, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
	}
	ReleaseDC(NULL, hdc);
}
void init::resetxy()
{
	RECT rect;
	GetClientRect(mywindows::main_hwnd, &rect);
	// 计算窗口的宽度和高度
	mywindows::windowTop = rect.left;
	mywindows::windowLeft = rect.top;
	int tempint = rect.right - rect.left;
	if (tempint != mywindows::windowWidth)ui::ScreenModeChanged = 1;
	mywindows::windowWidth = tempint;
	tempint = rect.bottom - rect.top;
	if (tempint != mywindows::windowHeight)ui::ScreenModeChanged = 1;
	mywindows::windowHeight = tempint;
	if (ui::ScreenModeChanged) {
		ui::ballW = 0.18 * mywindows::windowWidth;
		ui::ballH = 0.075 * mywindows::windowHeight;
		ui::ball10x = mywindows::windowWidth - ui::ballW * 1.3;
		ui::bally = mywindows::windowHeight - ui::ballH * 2;
		ui::ball1x = mywindows::windowWidth - ui::ballW * 2.4;
		ui::ball1end = ui::ball1x + ui::ballW;
		ui::ball10end = ui::ball10x + ui::ballW;
		ui::ballyend = ui::bally + ui::ballH;
		ui::overlay1X = mywindows::windowWidth * 3 / 9 - mywindows::windowWidth / 17;
		ui::overlay2X = mywindows::windowWidth * 4 / 9 - mywindows::windowWidth / 17;
		ui::overlay3X = mywindows::windowWidth * 5 / 9 - mywindows::windowWidth / 17;
		ui::overlay4X = mywindows::windowWidth * 6 / 9 - mywindows::windowWidth / 17;
		ui::overlay1Y = mywindows::windowHeight * 3 / 40;
		ui::overlayW = mywindows::windowWidth * 3 / 40;
		ui::overlayH = mywindows::windowHeight * 3 / 40;
		ui::button1x = ui::overlay1X + ui::overlayW;
		ui::button2x = ui::overlay2X + ui::overlayW;
		ui::button3x = ui::overlay3X + ui::overlayW;
		ui::button4x = ui::overlay4X + ui::overlayW;
		ui::buttony = ui::overlay1Y + ui::overlayH;
		ui::overX = mywindows::windowWidth / 10 * 2;
		ui::overlayY = mywindows::windowHeight / 10 * 2;
		ui::bottom1x = mywindows::windowWidth * 0.1;
		ui::bottom1y = mywindows::windowHeight * 0.85;
		ui::listyend = mywindows::windowHeight;
		paintname::skipbmx = mywindows::windowWidth * 0.8;
		paintname::skipbmy = mywindows::windowHeight * 0.045;
		paintname::skipbmxend = paintname::skipbmx + mywindows::windowWidth * 0.1;
		paintname::skipbmyend = paintname::skipbmy + 100;
		ui::settingx = mywindows::windowWidth * 0.05;
		ui::settingy = mywindows::windowHeight * 0.85;
		ui::settingxend = ui::settingx + mywindows::windowWidth * 0.023;
		ui::settingyend = ui::settingy + mywindows::windowHeight * 0.036;
		ui::exitx = mywindows::windowWidth * 0.9;
		ui::exitxend = mywindows::windowWidth * 0.93;
		ui::exity = mywindows::windowHeight * 0.045;
		ui::exityend = mywindows::windowHeight * 0.045 + mywindows::windowWidth * 0.03;
		ui::listx[0] = mywindows::windowWidth * 0.107;
		ui::listxend = mywindows::windowWidth * 0.078;
		ui::addnamex = mywindows::windowWidth * 0.4;
		ui::addnameendx = ui::addnamex + mywindows::windowWidth * 0.12;
		ui::addnamey = mywindows::windowHeight * 0.8;
		ui::addnameendy = ui::addnamey + mywindows::windowHeight * 0.036;
		setscreen.resetplace();
		for (char i = 0; i <= 9; i++) {
			ui::listx[i + 1] = ui::listx[i] + ui::listxend;
		}
		DeleteObject(paintname::icon_star);
		DeleteObject(ui::text_mid);
		DeleteObject(ui::text_big);
		DeleteObject(ui::text);
		DeleteObject(ui::text_list);
		DeleteObject(ui::icon_mid);
		DeleteObject(ui::icon);
		font();
	}
}
void init::main(WNDPROC w1, WNDPROC w2, WNDPROC w3)
{
	std::thread loading(std::bind(&init::regwindow, w1, w2, w3));
	loading.detach();
	SetConsoleOutputCP(65001); // 设置为UTF-8编码
	CHAR run[260] = { 0 };
	GetModuleFileNameA(NULL, run, MAX_PATH);
	int len = MultiByteToWideChar(CP_ACP, 0, run, -1, NULL, 0);
	wchar_t* run_ = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, run, -1, run_, len);
	Log::wrunpath = run_;
	mywindows::log("getting run path \n%s", run);
	mywindows::log(L"getting run path \n%s", run_);
	mywindows::removeFileNameFromPath(run);
	Log::wrunpath = run_;
	mywindows::removeFileNameFromPath(Log::wrunpath);
	Log::runpath = run;
	mywindows::log("initiating run path(string) %s", Log::runpath.c_str());
	mywindows::log(L"initiating run path(wstring) %ws", Log::wrunpath.c_str());
	config::init();
	setscreen.offmusic = config::getint(OFFMUSIC);
	setscreen.FloatWindow = config::getint(FLOATWINDOW);
	DWORD threadId;
	// 创建线程
	mywindows::log("start thread");
	paintname::random_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)paintname::RandomNumberGenerator, NULL, 0,&threadId);
	setscreen.offvideo = config::getint(OFF_VIDEO);
	ui::mode = config::getint(MODE);
	mywindows::debug=config::getint(DEBUG);
	setscreen.offmusic = config::getint(OFFMUSIC);
	DWORD threadId2;
	// 创建线程
	mywindows::log("start thread");
	HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Upgrade, NULL, 0, &threadId2);
	mciSendString(L"open .\\files\\mp3\\backsound.mp3 alias bgm", NULL, 0, NULL); // 打开 MP3 文件并创建一个别名 'bgm'
}
void init::music()
{
	static bool once = 1;
	if (once) {
		std::wstring str = L"open \"";
		str += Log::wrunpath;
		str += L"\\files\\mp3\\backsound.mp3\" alias bgm";
		mywindows::log(L"load bgm,command is :%ws", str.c_str());
		mciSendString(str.c_str(), 0, 0, 0);
		once = 0;
	}// 打开并播放背景音乐
	if (!setscreen.offmusic)
		mciSendString(L"play bgm repeat", NULL, 0, NULL); // 使用别名 'bgm' 播放音乐，并设置为循环播放
}
void init::MainWindow()
{
	setscreen.fullscreen = !config::getint(INWINDOW);
	if (setscreen.fullscreen)mywindows::main_hwnd = CreateWindowW(L"main", config::get(WINDOW_TITEL).c_str(), WS_POPUP | WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN,
		0, 0, mywindows::windowWidth, mywindows::windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
	else {
		mywindows::windowWidth = mywindows::screenWidth;
		mywindows::windowHeight = mywindows::screenHeight;
		mywindows::main_hwnd = CreateWindowW(L"main", config::get(WINDOW_TITEL).c_str(), WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_THICKFRAME,
			0, 0, mywindows::windowWidth, mywindows::windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);
	}
	ShowWindow(mywindows::main_hwnd, SW_SHOWNORMAL);//把窗体显示出来
}
void init::regwindow(WNDPROC w1,WNDPROC w2,WNDPROC w3)
{
	w2_ = w2; w3_ = w3;
	WNDCLASS wndcls{}; //创建一个窗体类
	wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	wndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//设置光标
	wndcls.hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//设置窗体左上角的图标
	wndcls.hInstance = GetModuleHandle(NULL);;//设置窗体所属的应用程序实例
	wndcls.lpfnWndProc = w1;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	wndcls.lpszClassName = L"main";//设置窗体的类名
	wndcls.lpszMenuName = NULL;//设置窗体的菜单,没有，填NULL
	wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&wndcls);//向操作系统注册窗体
	DWORD threadId;
	HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Regwindows, NULL, 0, &threadId);
}
DWORD WINAPI init::Regwindows() {
	WNDCLASS Fwndcls{}; //创建一个窗体类
	Fwndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	Fwndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	Fwndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	Fwndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//设置光标
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//设置窗体左上角的图标
	Fwndcls.hInstance = GetModuleHandle(NULL);;//设置窗体所属的应用程序实例
	Fwndcls.lpfnWndProc = w2_;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	Fwndcls.lpszClassName = L"FLOAT";//设置窗体的类名
	Fwndcls.lpszMenuName = NULL;//设置窗体的菜单,没有，填NULL
	Fwndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&Fwndcls);//向操作系统注册窗体
	WNDCLASS Qwndcls{}; //创建一个窗体类
	Fwndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	Fwndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	Fwndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	Fwndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//设置光标
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//设置窗体左上角的图标
	Fwndcls.hInstance = GetModuleHandle(NULL);;//设置窗体所属的应用程序实例
	Fwndcls.lpfnWndProc = w3_;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	Fwndcls.lpszClassName = L"QUIT";//设置窗体的类名
	Fwndcls.lpszMenuName = NULL;//设置窗体的菜单,没有，填NULL
	Fwndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&Fwndcls);//向操作系统注册窗体
	return 0;
}
DWORD WINAPI init::Upgrade(){
	mywindows::log("writes version");
	if (!std::filesystem::exists("version")) {
		std::string file_version;
		std::ifstream file("version");
		if (file.is_open()) {
			std::getline(file, file_version);
		}

		const std::string CURRENT_VERSION = "1.3.0"; // 假设当前版本号是 "1.2.3"

		if (file_version.empty() || file_version != CURRENT_VERSION) {
			mywindows::logf << "版本号不同" << file_version << "->" << CURRENT_VERSION << std::endl;
			remove("files\\setting.json");
			file.clear();
			std::ofstream output_file("version", std::ios::trunc);
			if (output_file.is_open()) {
				output_file << CURRENT_VERSION;
				output_file.close();
			}
		}
	}
	if (std::filesystem::exists("upgrade_temp.exe"))
	{
		std::filesystem::remove("upgrade.exe");
		std::filesystem::rename("upgrade_temp.exe", "upgrade.exe");
	}
	ShellExecute(NULL, L"open", L"upgrade.exe", NULL, NULL, SW_SHOWNORMAL);
	return 0;
}

bool init::cheakIsFontInstalled(const std::wstring fontName) {
	using namespace Gdiplus;
	// 要检查的字体名称
	bool isFontInstalled = false;
	InstalledFontCollection installedFontCollection;
	INT          count = 0;
	INT          found = 0;
	WCHAR        familyName[LF_FACESIZE];  // enough space for one family name
	WCHAR* familyList = NULL;
	FontFamily* pFontFamily = NULL;

	// How many font families are installed?
	count = installedFontCollection.GetFamilyCount();

	// Allocate a buffer to hold the array of FontFamily
	// objects returned by GetFamilies.
	pFontFamily = new FontFamily[count];

	// Get the array of FontFamily objects.
	installedFontCollection.GetFamilies(count, pFontFamily, &found);

	// The loop below creates a large string that is a comma-separated
	// list of all font family names.
	// Allocate a buffer large enough to hold that string.
	familyList = new WCHAR[count * (sizeof(familyName) + 3)];
	StringCchCopy(familyList, 1, L"");

	for (INT j = 0; j < count; ++j)
	{
		pFontFamily[j].GetFamilyName(familyName);
		if (fontName == familyName) {
			isFontInstalled = 1;
		}
	}
	delete[] pFontFamily;
	delete[] familyList;

	return isFontInstalled;
}