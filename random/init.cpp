#include "init.h"
#include "ui.h"
#include<Windows.h>
#include"NameScreen.h"
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

WNDPROC w2_, w3_;

void init::font()
{
	int desiredPixelHeight = mywindows::WW * 0.17;
	// 获取设备上下文的 DPI
	HDC hdc = GetDC(nullptr); // 获取桌面设备上下文
	int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	// 计算逻辑单位高度
	int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
	int logicalweidth = logicalHeight * 0.77;
	bool isInstall;
	isInstall= cheakIsFontInstalled(L"genshin-icon");
	if (isInstall) {
		ui::NS.icon_star = CreateFontW(logicalHeight * 0.0862, logicalweidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon_mid = CreateFontW(logicalHeight * 0.16, logicalweidth * 0.22, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon = CreateFontW(logicalHeight * 0.2299, logicalweidth * 0.3008, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
	}
	else {
		MessageBox(nullptr, L"请安装字体", L"错误", MB_ICONERROR);
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
		MessageBox(nullptr, L"请安装字体", L"错误", MB_ICONERROR);
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
			MessageBox(nullptr, L"请安装字体", L"错误", MB_ICONERROR);
			mywindows::errlogf << "Aa漆书字体未安装" << std::endl;
			system("files\\ttfs\\QS.ttf");
			mywindows::reboot();
		}
	}
	else {
		ui::text_big = CreateFontW(logicalHeight, logicalweidth, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
	}
	ReleaseDC(nullptr, hdc);
}
void init::resetxy()
{
	RECT rect;
	GetClientRect(mywindows::main_hwnd, &rect);
	// 计算窗口的宽度和高度
	mywindows::windowTop = rect.left;
	mywindows::windowLeft = rect.top;
	int tempint = rect.right - rect.left;
	mywindows::WW = tempint;
	tempint = rect.bottom - rect.top;
	mywindows::WH = tempint;
	if (ui::ScreenModeChanged) {
		ui::overX = mywindows::WW / 10 * 2;
		ui::overlayY = mywindows::WH / 10 * 2;
		ui::bottom1x = mywindows::WW * 0.1;
		ui::bottom1y = mywindows::WH * 0.85;
		ui::listyend = mywindows::WH;
		ui::NS.skipbmx = mywindows::WW * 0.8;
		ui::NS.skipbmy = mywindows::WH * 0.045;
		ui::NS.skipbmxend = ui::NS.skipbmx + mywindows::WW * 0.1;
		ui::NS.skipbmyend = ui::NS.skipbmy + 100;
		ui::settingx = mywindows::WW * 0.05;
		ui::settingy = mywindows::WH * 0.85;
		ui::settingxend = ui::settingx + mywindows::WW * 0.023;
		ui::settingyend = ui::settingy + mywindows::WH * 0.036;
		ui::exitx = mywindows::WW * 0.9;
		ui::exitxend = mywindows::WW * 0.93;
		ui::exity = mywindows::WH * 0.045;
		ui::exityend = mywindows::WH * 0.045 + mywindows::WW * 0.03;
		ui::listx[0] = mywindows::WW * 0.107;
		ui::listxend = mywindows::WW * 0.078;
		ui::addnamex = mywindows::WW * 0.4;
		ui::addnameendx = ui::addnamex + mywindows::WW * 0.12;
		ui::addnamey = mywindows::WH * 0.8;
		ui::addnameendy = ui::addnamey + mywindows::WH * 0.036;
		ui::his.x = ui::bottom1x + mywindows::WW * 0.08;
		ui::his.y = ui::bottom1y;
		ui::his.xE = ui::his.x + mywindows::WW * 0.073;
		ui::his.yE = ui::bottom1y + mywindows::WH * 0.039;
		ui::FS.resetPoint();
		ui::SS.resetplace();
		ui::NS.resetPoint();
		for (char i = 0; i <= 9; i++) {
			ui::listx[i + 1] = ui::listx[i] + ui::listxend;
		}
		DeleteObject(ui::NS.icon_star);
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
	ui::SS.offmusic = config::getint(OFFMUSIC);
	ui::SS.FloatWindow = config::getint(FLOATWINDOW);
	config();
	DWORD threadId2;
	HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Upgrade, NULL, 0, &threadId2);
	mciSendString(L"open .\\files\\mp3\\backsound.mp3 alias bgm", NULL, 0, NULL);
	// 打开 MP3 文件并创建一个别名 'bgm'
}
void init::music()
{
	static bool once = 1;
	if (once) {
		std::wstring str = L"open \"";
		std::wstring cmd;
		str += Log::wrunpath;
		cmd = str;
		str += L"\\files\\mp3\\backsound.mp3\" alias bgm";
		mywindows::log(L"load bgm,command is :%ws", str.c_str());
		mciSendString(str.c_str(), NULL, 0, NULL);

		once = 0;
	}// 打开并播放背景音乐
	if (!ui::SS.offmusic)
		mciSendString(L"play bgm repeat", NULL, 0, NULL); // 使用别名 'bgm' 播放音乐，并设置为循环播放
}

void init::config()
{
	config::init();
	ui::NS.rerandom();
	ui::SS.offvideo = config::getint(OFF_VIDEO);
	ui::SS.offmusic= config::getint(OFFMUSIC);
	mywindows::debug = config::getint(DEBUG);
	ui::mode = config::getint(MODE);
	ui::SS.FloatWindow= config::getint(FLOATWINDOW);
	ui::SS.fullscreen= config::getint(INWINDOW);
}

void init::MainWindow()
{
	ui::SS.fullscreen = !config::getint(INWINDOW);
	if (ui::SS.fullscreen)mywindows::main_hwnd = CreateWindowW(L"main", config::get(WINDOW_TITEL).c_str(), WS_POPUP | WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN,
		0, 0, mywindows::WW, mywindows::WH, NULL, NULL, GetModuleHandle(NULL), NULL);
	else {
		mywindows::WW = mywindows::screenWidth;
		mywindows::WH = mywindows::screenHeight;
		mywindows::main_hwnd = CreateWindowW(L"main", config::get(WINDOW_TITEL).c_str(), WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_THICKFRAME,
			0, 0, mywindows::WW, mywindows::WH, NULL, NULL, GetModuleHandle(NULL), NULL);
	}
}
void init::regwindow(const WNDPROC w1, const WNDPROC w2, const WNDPROC w3)
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

		const std::string CURRENT_VERSION = "1.5.0"; // 假设当前版本号是 "1.2.3"

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
	WCHAR* familyList = nullptr;
	FontFamily* pFontFamily = nullptr;

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