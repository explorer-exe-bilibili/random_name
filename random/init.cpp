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
	// ��ȡ�豸�����ĵ� DPI
	HDC hdc = GetDC(nullptr); // ��ȡ�����豸������
	int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	// �����߼���λ�߶�
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
		MessageBox(nullptr, L"�밲װ����", L"����", MB_ICONERROR);
		mywindows::errlogf << "\"genshin - icon\"����δ��װ" << std::endl;
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
		MessageBox(nullptr, L"�밲װ����", L"����", MB_ICONERROR);
		mywindows::errlogf << "SDK_SC_Web����δ��װ" << std::endl;
		system("files\\ttfs\\text.ttf");
		mywindows::reboot();
	}
	if (!config::getint(UNSUITFONT)) {
		isInstall = cheakIsFontInstalled(L"Aa����");
		if (isInstall) {
			ui::text_big = CreateFontW(logicalHeight, logicalweidth, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Aa����");
		}
		else {
			MessageBox(nullptr, L"�밲װ����", L"����", MB_ICONERROR);
			mywindows::errlogf << "Aa��������δ��װ" << std::endl;
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
	// ���㴰�ڵĿ�Ⱥ͸߶�
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
	SetConsoleOutputCP(65001); // ����ΪUTF-8����
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
	// �� MP3 �ļ�������һ������ 'bgm'
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
	}// �򿪲����ű�������
	if (!ui::SS.offmusic)
		mciSendString(L"play bgm repeat", NULL, 0, NULL); // ʹ�ñ��� 'bgm' �������֣�������Ϊѭ������
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
	WNDCLASS wndcls{}; //����һ��������
	wndcls.cbClsExtra = 0;//��Ķ����ڴ棬Ĭ��Ϊ0����
	wndcls.cbWndExtra = 0;//���ڵĶ����ڴ棬Ĭ��Ϊ0����
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//��ȡ��ˢ����������ص�HGDIOBJ����ǿ������ת����
	wndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//���ù��
	wndcls.hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//���ô������Ͻǵ�ͼ��
	wndcls.hInstance = GetModuleHandle(NULL);;//���ô���������Ӧ�ó���ʵ��
	wndcls.lpfnWndProc = w1;//WinSunProc;//���ô���Ļص���������ʱûд��������ΪNULL�����油��
	wndcls.lpszClassName = L"main";//���ô��������
	wndcls.lpszMenuName = NULL;//���ô���Ĳ˵�,û�У���NULL
	wndcls.style = CS_HREDRAW | CS_VREDRAW;//���ô�����Ϊˮƽ�ػ��ʹ�ֱ�ػ�
	RegisterClass(&wndcls);//�����ϵͳע�ᴰ��
	DWORD threadId;
	HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Regwindows, NULL, 0, &threadId);
}
DWORD WINAPI init::Regwindows() {
	WNDCLASS Fwndcls{}; //����һ��������
	Fwndcls.cbClsExtra = 0;//��Ķ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.cbWndExtra = 0;//���ڵĶ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//��ȡ��ˢ����������ص�HGDIOBJ����ǿ������ת����
	Fwndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//���ù��
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//���ô������Ͻǵ�ͼ��
	Fwndcls.hInstance = GetModuleHandle(NULL);;//���ô���������Ӧ�ó���ʵ��
	Fwndcls.lpfnWndProc = w2_;//WinSunProc;//���ô���Ļص���������ʱûд��������ΪNULL�����油��
	Fwndcls.lpszClassName = L"FLOAT";//���ô��������
	Fwndcls.lpszMenuName = NULL;//���ô���Ĳ˵�,û�У���NULL
	Fwndcls.style = CS_HREDRAW | CS_VREDRAW;//���ô�����Ϊˮƽ�ػ��ʹ�ֱ�ػ�
	RegisterClass(&Fwndcls);//�����ϵͳע�ᴰ��
	WNDCLASS Qwndcls{}; //����һ��������
	Fwndcls.cbClsExtra = 0;//��Ķ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.cbWndExtra = 0;//���ڵĶ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//��ȡ��ˢ����������ص�HGDIOBJ����ǿ������ת����
	Fwndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//���ù��
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//���ô������Ͻǵ�ͼ��
	Fwndcls.hInstance = GetModuleHandle(NULL);;//���ô���������Ӧ�ó���ʵ��
	Fwndcls.lpfnWndProc = w3_;//WinSunProc;//���ô���Ļص���������ʱûд��������ΪNULL�����油��
	Fwndcls.lpszClassName = L"QUIT";//���ô��������
	Fwndcls.lpszMenuName = NULL;//���ô���Ĳ˵�,û�У���NULL
	Fwndcls.style = CS_HREDRAW | CS_VREDRAW;//���ô�����Ϊˮƽ�ػ��ʹ�ֱ�ػ�
	RegisterClass(&Fwndcls);//�����ϵͳע�ᴰ��
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

		const std::string CURRENT_VERSION = "1.5.0"; // ���赱ǰ�汾���� "1.2.3"

		if (file_version.empty() || file_version != CURRENT_VERSION) {
			mywindows::logf << "�汾�Ų�ͬ" << file_version << "->" << CURRENT_VERSION << std::endl;
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
	// Ҫ������������
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