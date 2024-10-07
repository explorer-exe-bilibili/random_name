#include<Windows.h>
#include<filesystem>
#include<thread>
#include <dshow.h>

#include"resource.h"
#include "FirstScreen.h"
#include "HistoryScreen.h"
#include "MenuScreen.h"
#include"set-json.h"
#include"NameScreen.h"
#include"mywindows.h"
#include"configitem.h"
#include "init.h"
#include "ui.h"

WNDPROC w2_, w3_;

void init::font()
{
	const int desiredPixelHeight = mywindows::WW * 0.17;
	// 获取设备上下文的 DPI
	const HDC hdc = GetDC(nullptr); // 获取桌面设备上下文
	const int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	// 计算逻辑单位高度
	const int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
	const int logicalWidth = logicalHeight * 0.77;
	bool isInstall = checkIsFontInstalled(L"genshin-icon");
	if (isInstall) {
		ui::NS->icon_star = CreateFontW(logicalHeight * 0.0862, logicalWidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon_mid = CreateFontW(logicalHeight * 0.16, logicalWidth * 0.22, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon = CreateFontW(logicalHeight * 0.2299, logicalWidth * 0.3008, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
	}
	else {
		MessageBox(nullptr, L"请安装字体", L"错误", MB_ICONERROR);
		mywindows::errlogf << R"("genshin - icon"字体未安装)" << std::endl;
		system("files\\ttfs\\icon.ttf");
		mywindows::reboot();
	}
	isInstall = checkIsFontInstalled(L"SDK_SC_Web");
	if(isInstall){
		ui::text = CreateFontW(logicalHeight * 0.1149, logicalWidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ui::text_mid = CreateFontW(logicalHeight * 0.1724, logicalWidth * 0.1729, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ui::text_list = CreateFontW(logicalHeight * 0.7, logicalWidth * 0.7, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
	}
	else {
		MessageBox(nullptr, L"请安装字体", L"错误", MB_ICONERROR);
		mywindows::errlogf << R"(SDK_SC_Web字体未安装)" << std::endl;
		system("files\\ttfs\\text.ttf");
		mywindows::reboot();
	}
	if (!config::getint(UNSUITFONT)) {
		isInstall = checkIsFontInstalled(L"Aa漆书");
		if (isInstall) {
			ui::text_big = CreateFontW(logicalHeight*1.3, logicalWidth*1.3, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Aa漆书");
		}
		else {
			MessageBox(nullptr, L"请安装字体", L"错误", MB_ICONERROR);
			mywindows::errlogf << R"(Aa漆书字体未安装)" << std::endl;
			system("files\\ttfs\\QS.ttf");
			mywindows::reboot();
		}
	}
	else {
		ui::text_big = CreateFontW(logicalHeight*1.3, logicalWidth*1.3, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
	}
	ReleaseDC(nullptr, hdc);
}
void init::resetPoint()
{
	RECT rect;
	GetClientRect(mywindows::main_hwnd, &rect);
	// 计算窗口的宽度和高度
	mywindows::windowTop = rect.left;
	mywindows::windowLeft = rect.top;
	int width = rect.right - rect.left;
	mywindows::WW = width;
	width = rect.bottom - rect.top;
	mywindows::WH = width;
	if(ui::ScreenModeChanged){
		ui::FS->resetSize();
		ui::SS->resetplace();
		ui::NS->resetPoint();
		DeleteObject(ui::NS->icon_star);
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
	std::thread([w1, w2, w3] { regWindow(w1, w2, w3); }).detach();
	SetConsoleOutputCP(65001); // 设置为UTF-8编码
	CHAR run[260] = {};
	GetModuleFileNameA(nullptr, run, MAX_PATH);
	const int len = MultiByteToWideChar(CP_ACP, 0, run, -1, nullptr, 0);
	wchar_t* run_ = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, run, -1, run_, len);
	Log::wrunpath = run_;
	mywindows::log("getting run path \n%s", run);
	mywindows::log(L"getting run path \n%s", run_);
	mywindows::removeFileNameFromPath(run);
	Log::wrunpath = run_;
	mywindows::removeFileNameFromPath(Log::wrunpath);
	Log::runpath = run;
	config::init();
	ui::FS = new FirstScreen();
	ui::NS = new NameScreen();
	ui::HS = new HistoryScreen();
	ui::SS = new set2();
	ui::MS = new MenuScreen();
	set2::offMusic = config::getint(OFFMUSIC);
	set2::FloatWindow = config::getint(FLOATWINDOW);
	config();
	DWORD threadId2;
	CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(Upgrade), nullptr, 0, &threadId2);
	mciSendString(L"open .\\files\\mp3\\backsound.mp3 alias bgm", nullptr, 0, nullptr);
	// 打开 MP3 文件并创建一个别名 'bgm'
}
void init::music()
{
	static bool once = true;
	if (once) {
		std::wstring str = L"open \"";
		str += Log::wrunpath;
		std::wstring cmd = str;
		str += L"\\files\\mp3\\backsound.mp3\" alias bgm";
		mywindows::log(L"load bgm,command is :%ws", str.c_str());
		mciSendString(str.c_str(), nullptr, 0, nullptr);

		once = false;
	}// 打开并播放背景音乐
	if (!set2::offMusic)
		mciSendString(L"play bgm repeat", nullptr, 0, nullptr); // 使用别名 'bgm' 播放音乐，并设置为循环播放
}

void init::config()
{
	set2::offVideo = config::getint(OFF_VIDEO);
	set2::offMusic= config::getint(OFFMUSIC);
	mywindows::debug = config::getint(DEBUG);
	ui::mode = config::getint(MODE);
	set2::FloatWindow= config::getint(FLOATWINDOW);
	set2::fullscreen= config::getint(INWINDOW);
}

void init::MainWindow()
{
	if (set2::fullscreen)mywindows::main_hwnd = CreateWindowW(L"main", config::get(WINDOW_TITLE).c_str(), WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_THICKFRAME, 0, 0, mywindows::WW, mywindows::WH, NULL, NULL, GetModuleHandle(NULL), NULL); 
	else {
		mywindows::WW = mywindows::screenWidth;
		mywindows::WH = mywindows::screenHeight;
		mywindows::main_hwnd = CreateWindowW(L"main", config::get(WINDOW_TITLE).c_str(), WS_POPUP | WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN, 0, 0, mywindows::WW, mywindows::WH, NULL, NULL, GetModuleHandle(NULL), NULL);
	}
}
void init::regWindow(const WNDPROC w1, const WNDPROC w2, const WNDPROC w3)
{
	w2_ = w2; w3_ = w3;
	WNDCLASS wndcls; //创建一个窗体类
	wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	wndcls.hbrBackground = HBRUSH(GetStockObject(NULL_BRUSH));//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	wndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//设置光标
	wndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));//设置窗体左上角的图标
	wndcls.hInstance = GetModuleHandle(nullptr);;//设置窗体所属的应用程序实例
	wndcls.lpfnWndProc = w1;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	wndcls.lpszClassName = L"main";//设置窗体的类名
	wndcls.lpszMenuName = nullptr;//设置窗体的菜单,没有，填NULL
	wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&wndcls);//向操作系统注册窗体
	DWORD threadId;
	CreateThread(nullptr, 0, PTHREAD_START_ROUTINE(RegWindows), nullptr, 0, &threadId);
}
DWORD WINAPI init::RegWindows() {
	WNDCLASS Fwndcls; //创建一个窗体类
	Fwndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	Fwndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	Fwndcls.hbrBackground = HBRUSH(GetStockObject(NULL_BRUSH));//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	Fwndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//设置光标
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));//设置窗体左上角的图标
	Fwndcls.hInstance = GetModuleHandle(nullptr);;//设置窗体所属的应用程序实例
	Fwndcls.lpfnWndProc = w2_;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	Fwndcls.lpszClassName = L"FLOAT";//设置窗体的类名
	Fwndcls.lpszMenuName = nullptr;//设置窗体的菜单,没有，填NULL
	Fwndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&Fwndcls);//向操作系统注册窗体
	Fwndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	Fwndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	Fwndcls.hbrBackground = HBRUSH(GetStockObject(WHITE_BRUSH));//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	Fwndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//设置光标
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));//设置窗体左上角的图标
	Fwndcls.hInstance = GetModuleHandle(nullptr);;//设置窗体所属的应用程序实例
	Fwndcls.lpfnWndProc = w3_;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	Fwndcls.lpszClassName = L"QUIT";//设置窗体的类名
	Fwndcls.lpszMenuName = nullptr;//设置窗体的菜单,没有，填NULL
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

		const std::string CURRENT_VERSION = "1.9.0"; // 假设当前版本号是 "1.2.3"

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
	ShellExecute(nullptr, L"open", L"upgrade.exe", nullptr, nullptr, SW_SHOWNORMAL);
	return 0;
}

bool init::checkIsFontInstalled(const std::wstring& fontName) {
	using namespace Gdiplus;
	// 要检查的字体名称
	bool isFontInstalled = false;
	const InstalledFontCollection installedFontCollection;
	INT          found = 0;
	WCHAR        familyName[LF_FACESIZE];  // enough space for one family name

	// How many font families are installed?
	const INT count = installedFontCollection.GetFamilyCount();

	// Allocate a buffer to hold the array of FontFamily
	// objects returned by GetFamilies.
	FontFamily* pFontFamily = new FontFamily[count];

	// Get the array of FontFamily objects.
	installedFontCollection.GetFamilies(count, pFontFamily, &found);

	// The loop below creates a large string that is a comma-separated
	// list of all font family names.
	// Allocate a buffer large enough to hold that string.
	const auto familyList = new WCHAR[count * (sizeof(familyName) + 3)];
	StringCchCopy(familyList, 1, L"");

	for (INT j = 0; j < count; ++j)
	{
		pFontFamily[j].GetFamilyName(familyName);
		if (fontName == familyName) {
			isFontInstalled = true;
		}
	}
	delete[] pFontFamily;
	delete[] familyList;

	return isFontInstalled;
}