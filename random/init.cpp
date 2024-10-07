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
	// ��ȡ�豸�����ĵ� DPI
	const HDC hdc = GetDC(nullptr); // ��ȡ�����豸������
	const int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	// �����߼���λ�߶�
	const int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
	const int logicalWidth = logicalHeight * 0.77;
	bool isInstall = checkIsFontInstalled(L"genshin-icon");
	if (isInstall) {
		ui::NS->icon_star = CreateFontW(logicalHeight * 0.0862, logicalWidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon_mid = CreateFontW(logicalHeight * 0.16, logicalWidth * 0.22, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon = CreateFontW(logicalHeight * 0.2299, logicalWidth * 0.3008, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
	}
	else {
		MessageBox(nullptr, L"�밲װ����", L"����", MB_ICONERROR);
		mywindows::errlogf << R"("genshin - icon"����δ��װ)" << std::endl;
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
		MessageBox(nullptr, L"�밲װ����", L"����", MB_ICONERROR);
		mywindows::errlogf << R"(SDK_SC_Web����δ��װ)" << std::endl;
		system("files\\ttfs\\text.ttf");
		mywindows::reboot();
	}
	if (!config::getint(UNSUITFONT)) {
		isInstall = checkIsFontInstalled(L"Aa����");
		if (isInstall) {
			ui::text_big = CreateFontW(logicalHeight*1.3, logicalWidth*1.3, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Aa����");
		}
		else {
			MessageBox(nullptr, L"�밲װ����", L"����", MB_ICONERROR);
			mywindows::errlogf << R"(Aa��������δ��װ)" << std::endl;
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
	// ���㴰�ڵĿ�Ⱥ͸߶�
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
	SetConsoleOutputCP(65001); // ����ΪUTF-8����
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
	// �� MP3 �ļ�������һ������ 'bgm'
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
	}// �򿪲����ű�������
	if (!set2::offMusic)
		mciSendString(L"play bgm repeat", nullptr, 0, nullptr); // ʹ�ñ��� 'bgm' �������֣�������Ϊѭ������
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
	WNDCLASS wndcls; //����һ��������
	wndcls.cbClsExtra = 0;//��Ķ����ڴ棬Ĭ��Ϊ0����
	wndcls.cbWndExtra = 0;//���ڵĶ����ڴ棬Ĭ��Ϊ0����
	wndcls.hbrBackground = HBRUSH(GetStockObject(NULL_BRUSH));//��ȡ��ˢ����������ص�HGDIOBJ����ǿ������ת����
	wndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//���ù��
	wndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));//���ô������Ͻǵ�ͼ��
	wndcls.hInstance = GetModuleHandle(nullptr);;//���ô���������Ӧ�ó���ʵ��
	wndcls.lpfnWndProc = w1;//WinSunProc;//���ô���Ļص���������ʱûд��������ΪNULL�����油��
	wndcls.lpszClassName = L"main";//���ô��������
	wndcls.lpszMenuName = nullptr;//���ô���Ĳ˵�,û�У���NULL
	wndcls.style = CS_HREDRAW | CS_VREDRAW;//���ô�����Ϊˮƽ�ػ��ʹ�ֱ�ػ�
	RegisterClass(&wndcls);//�����ϵͳע�ᴰ��
	DWORD threadId;
	CreateThread(nullptr, 0, PTHREAD_START_ROUTINE(RegWindows), nullptr, 0, &threadId);
}
DWORD WINAPI init::RegWindows() {
	WNDCLASS Fwndcls; //����һ��������
	Fwndcls.cbClsExtra = 0;//��Ķ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.cbWndExtra = 0;//���ڵĶ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.hbrBackground = HBRUSH(GetStockObject(NULL_BRUSH));//��ȡ��ˢ����������ص�HGDIOBJ����ǿ������ת����
	Fwndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//���ù��
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));//���ô������Ͻǵ�ͼ��
	Fwndcls.hInstance = GetModuleHandle(nullptr);;//���ô���������Ӧ�ó���ʵ��
	Fwndcls.lpfnWndProc = w2_;//WinSunProc;//���ô���Ļص���������ʱûд��������ΪNULL�����油��
	Fwndcls.lpszClassName = L"FLOAT";//���ô��������
	Fwndcls.lpszMenuName = nullptr;//���ô���Ĳ˵�,û�У���NULL
	Fwndcls.style = CS_HREDRAW | CS_VREDRAW;//���ô�����Ϊˮƽ�ػ��ʹ�ֱ�ػ�
	RegisterClass(&Fwndcls);//�����ϵͳע�ᴰ��
	Fwndcls.cbClsExtra = 0;//��Ķ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.cbWndExtra = 0;//���ڵĶ����ڴ棬Ĭ��Ϊ0����
	Fwndcls.hbrBackground = HBRUSH(GetStockObject(WHITE_BRUSH));//��ȡ��ˢ����������ص�HGDIOBJ����ǿ������ת����
	Fwndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//���ù��
	Fwndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));//���ô������Ͻǵ�ͼ��
	Fwndcls.hInstance = GetModuleHandle(nullptr);;//���ô���������Ӧ�ó���ʵ��
	Fwndcls.lpfnWndProc = w3_;//WinSunProc;//���ô���Ļص���������ʱûд��������ΪNULL�����油��
	Fwndcls.lpszClassName = L"QUIT";//���ô��������
	Fwndcls.lpszMenuName = nullptr;//���ô���Ĳ˵�,û�У���NULL
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

		const std::string CURRENT_VERSION = "1.9.0"; // ���赱ǰ�汾���� "1.2.3"

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
	ShellExecute(nullptr, L"open", L"upgrade.exe", nullptr, nullptr, SW_SHOWNORMAL);
	return 0;
}

bool init::checkIsFontInstalled(const std::wstring& fontName) {
	using namespace Gdiplus;
	// Ҫ������������
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