#include "set-json.h"
#include"sth2sth.h"
#include"config.h"
#include "mywindows.h"
#include "bitmaps.h"
#include "directshow.h"
#include "ui.h"
#include<nlohmann\json.hpp>
#include<thread>


#define BETWEENCOUNT 100
#define S_WINDOWTITLE 200
#define ISFILE 300
#define ISBITMAP 400
#define NOLIMIT 0
#define NAME "Name"
#define CONFIGNAME "ConfigName"
#define ISSWITCH "IsSwitch"
#define ISEDIT "IsEditBox"
#define NUMBER "Number"
#define FILETYPE "FileType"
#define FILECHOOSE "FileChooseWindowName"
#define OUTOFLIMIT "OutOfLimitOutPut"
#define LIMIT "Limit"
#define TITLE "Title"
#define BITMAPC "BitmapNumber"
#define BEFORE 1
#define AFTER 2

using json = nlohmann::json;
using namespace std;
Log slog("files\\log\\set-json.log", 1);
extern HBITMAP hbitmaps[BitmapCounts];
extern BITMAP overlay1Bm, bm, ball, overlay2Bm, overlay3Bm, overlay4Bm, cardbg, exitinfo, goldenbg, listbm, liststar, buttom;


void set2::Load(string jsonpath) {
	init();
	Log slog("files\\log\\set-json.log", 0);
	// 读取JSON文件
openjsonfile:
	std::ifstream fileStream(jsonpath);
	if (!fileStream.is_open()) {
		slog << "Failed to open file" << std::endl;
		if (!std::filesystem::exists(jsonpath))
			rollback(jsonpath);
		goto openjsonfile;
	}
	// 将文件内容解析为JSON对象
	json data{};
	try {
		fileStream >> data;
	}
	catch (const std::exception& e) {
		slog << "[ERROR]Failed to parse JSON: " << e.what() << std::endl;
		rollback(jsonpath);
		return;
	}
	int sp = 0;//setting page
	int in = 0;//ItemNumber
	for (const auto& page : data["pages"]) {
		in = 0;
		spage pt;
		pt.Title = sth2sth::str2wstr(U2G(page.value("Title", "")));
		for (const auto& sItem : page["item"]) {
			sitem t;
			try
			{
				t.IsFile = sItem.value("IsFile", 0);
				t.IsEditBox = sItem.value("IsEditBox", 0);
				t.IsSwitch = sItem.value("IsSwitch", 0);
				t.Limit = sItem.value("Limit", NOLIMIT);
				t.Number = sItem.value("Number", in);
				t.BitmapNumber = sItem.value("BitmapNumber", 0);
				t.Name = sth2sth::str2wstr(U2G(sItem.value("Name", "")));
				t.ConfigName = sth2sth::str2wstr(sItem.value("ConfigName", ""));
				t.FileChooseWindowName = sth2sth::str2wstr(U2G(sItem.value("FileChooseWindowName", "")));
				t.FileType = sItem.value("FileType", "All");
				if (t.Limit == BETWEENCOUNT) {
					t.max = sItem.value("max", 0);
					t.min = sItem.value("min", 0);
					t.OutOfLimitOutPut = sItem.value("OutOfLimitOutPut", "");
				}
				else if (t.Limit == ISFILE || t.Limit == ISBITMAP) {
					t.IsEditBox = 1;
					t.IsFile = 1;
				}
			}
			catch (const std::exception& e)
			{
				slog << slog.pt() << "[ERROR]FAiled with reading json" << e.what() << endl;
				t.IsFile = 0;
				t.IsEditBox = 0;
				t.IsSwitch = 0;
				t.Limit = NOLIMIT;
				t.Number = in;
				t.ConfigName = L"unknow";
				t.BitmapNumber = 0;
				t.FileChooseWindowName = L"";
				t.FileType = "All";
				t.max = 0;
				t.min = 0;
				t.OutOfLimitOutPut = "";
			}
			pt.items.push_back(t);
			in++;
		}
		pt.itemcount = in;
		pages.push_back(pt);
		sp++;
	}
	settingpage = 1;
	return;
}
// UTF8字符串转成GBK字符串
std::string set2::U2G(const std::string& utf8)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);
	
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}
// GBK字符串转成json识别的UTF8字符串
std::string set2::G2U(const std::string& gbk)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), gbk.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
set2::set2() {
	string jsonpath = "files\\setting.json";
	std::thread loading(std::bind(&set2::Load, this, jsonpath));
	loading.detach();
}
set2::set2(std::string& jsonfile) {
	std::thread loading(std::bind(&set2::Load, this, jsonfile));
	loading.detach();
}
void set2::paint(HDC hdc, HDC hdcMem) {
	if (firstpaint) {
		SelectObject(hdcMem, hbitmaps[SetBM]);
		StretchBlt(hdc, 0, 0, mywindows::windowWidth, mywindows::windowHeight, hdcMem, 0, 0, setbm.bmWidth, setbm.bmHeight, SRCCOPY);
		firstpaint = 0;
	}
	SelectObject(hdc, ui::text_mid);
	wstring title = pages[settingpage-1].Title;
	int titlex, titley;
	int stringWidth = 0.0272 * mywindows::windowWidth * title.length();
	titlex= (mywindows::windowWidth - stringWidth) / 2;
	titley = mywindows::windowHeight * 0.05;
	TextOut_(hdc, titlex, titley, title.c_str());
	SelectObject(hdcMem, hbitmaps[exitb]);
	StretchBlt(hdc, ui::exitx, ui::exity, ui::exitxend - ui::exitx, ui::exityend - ui::exity, hdcMem, 0, 0, exitinfo.bmWidth, exitinfo.bmHeight, SRCAND);
	SelectObject(hdcMem, hbitmaps[exiti]);
	StretchBlt(hdc, ui::exitx, ui::exity, ui::exitxend - ui::exitx, ui::exityend - ui::exity, hdcMem, 0, 0, exitinfo.bmWidth, exitinfo.bmHeight, SRCPAINT);
	for (const auto& item : pages[settingpage-1].items) {
		showitem(item, hdc, hdcMem);
	}
	unsigned int totalp = static_cast<int>(pages.size());
	wstring t = to_wstring(settingpage) + L"/" + to_wstring(totalp);
	SelectObject(hdc, ui::icon_mid);
	TextOut_(hdc, nextbmx, nextbmy, L"b");
	TextOut_(hdc, lastbmx, lastbmy, L"c");
	SelectObject(hdc, ui::text_mid);
	TextOut_(hdc, mywindows::windowWidth * 0.765, mywindows::windowHeight * 0.91, t.c_str());

}
void set2::rollback(string jsonpath) {
	Log slog("files\\log\\set-json.log", 0);
	slog << "try to rollback setting page" << endl;
	// 创建JSON数据
	json j;
	json p;
	json i;
	p[TITLE] = G2U("图片");
	i[NAME] = G2U("卡池1图片"); i[CONFIGNAME] = "over1"; i[FILECHOOSE] = G2U("选择卡池1图片"); i[FILETYPE] = "bmp"; i[LIMIT] = ISBITMAP; i[NUMBER] = 1; i[BITMAPC] = 0;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池2图片"); i[CONFIGNAME] = "over2"; i[FILECHOOSE] = G2U("选择卡池2图片"); i[FILETYPE] = "bmp"; i[LIMIT] = ISBITMAP; i[NUMBER] = 2; i[BITMAPC] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池3图片"); i[CONFIGNAME] = "over3"; i[FILECHOOSE] = G2U("选择卡池3图片"); i[FILETYPE] = "bmp"; i[LIMIT] = ISBITMAP; i[NUMBER] = 3; i[BITMAPC] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池4图片"); i[CONFIGNAME] = "over4"; i[FILECHOOSE] = G2U("选择卡池4图片"); i[FILETYPE] = "bmp"; i[LIMIT] = ISBITMAP; i[NUMBER] = 4; i[BITMAPC] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("关闭音乐"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("名单与卡池");
	i[NAME] = G2U("卡池1名单"); i[CONFIGNAME] = "namesfile1"; i[FILECHOOSE] = G2U("选择卡池1名单"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池2名单"); i[CONFIGNAME] = "namesfile2"; i[FILECHOOSE] = G2U("选择卡池2名单"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池3名单"); i[CONFIGNAME] = "namesfile3"; i[FILECHOOSE] = G2U("选择卡池3名单"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池4名单"); i[CONFIGNAME] = "namesfile4"; i[FILECHOOSE] = G2U("选择卡池4名单"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("抽背卡池"); i[CONFIGNAME] = "special"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i["max"] = 4; i["min"] = 0;
	i[OUTOFLIMIT] = G2U("输入一个0-4之间的数字（0表示禁用）"); i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("关闭音乐"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("视频");
	i[NAME] = G2U("单发3星视频"); i[CONFIGNAME] = "signal 3star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("选择单发3星视频"); i[FILETYPE] = "video"; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("单发4星视频"); i[CONFIGNAME] = "signal 4star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("选择单发4星视频"); i[FILETYPE] = "video"; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("单发5星视频"); i[CONFIGNAME] = "signal 5star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("选择单发5星视频"); i[FILETYPE] = "video"; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("十发4星视频"); i[CONFIGNAME] = "group 4star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("选择十发4星视频"); i[FILETYPE] = "video"; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("十发5星视频"); i[CONFIGNAME] = "group 5star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("选择十发5星视频"); i[FILETYPE] = "video"; i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("关闭视频"); i[CONFIGNAME] = "off video"; i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("关闭音乐"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("杂项");
	i[NAME] = G2U("窗口模式"); i[CONFIGNAME] = "window mode(not full screen)"; i[ISSWITCH] = 1; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("标题"); i[CONFIGNAME] = "title name"; i[ISEDIT] = 1; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("悬浮窗"); i[CONFIGNAME] = "open float window"; i[ISSWITCH] = 1; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("关闭音乐"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	// 写入JSON文件
	std::ofstream file(jsonpath);
	if (file.is_open()) {
		try
		{
			slog << slog.pt() << "[INFO]begin to write json items";
			file << j.dump(4); // 将JSON数据写入文件，4表示缩进4个空格
			file.close();
			slog << "[INFO]JSON data written to" << jsonpath << endl;
		}
		catch (const std::exception& e)
		{
			slog << slog.pt() << "[ERROR]meet an error when writing json item (rollback):" << e.what() << endl;
			return;
		}
	}
	else {
		slog << "[ERROR]Failed to open file for writing" << std::endl;
	}
}
void set2::clicked(int x, int y)
{
	int totalp = pages.size();
	int number = -1;
	int half = 0;
	if (x >= ui::exitx AND x <= ui::exitxend AND y >= ui::exity AND y <= ui::exityend)quit();
	else if (x >= nextbmx AND x <= nextxend AND y >= nextbmy AND y <= nextyend) {
		number = 0;
		settingpage--;
		if (settingpage < 1) {
			settingpage = totalp;
		}
		changepage();
	}
	else if (x >= lastbmx AND x <= lastxend AND y >= lastbmy AND y <= lastyend) {
		number = 0;
		settingpage++;
		if (settingpage > totalp) {
			settingpage = 1;
		}
		changepage();
	}
	else {
		sNode* current = shead;
		while (current != NULL) {
			if (x >= current->x AND x <= current->xend AND y >= current->y AND y <= current->yend) {
				if (x >= current->x AND x <= current->x + (current->xend - current->x) / 2)
					half = BEFORE;
				else half = AFTER;
				number = current->number;
				break;
			}
			current = current->next;
		}
	}
	if (number != -1) {
		directshow::music(CLICK);
		if (number > 0) {
			for (const auto& item : pages[settingpage-1].items) {
				if (number == item.Number) {
					if (item.IsSwitch) {
						config::turnUpSideDown(item.ConfigName);
						rereadconfig();
					}
					else if (item.IsFile) {
						if (half == BEFORE)
							ChooseFile(item);
						else {
							OpenFile(item);
						}
						if (item.Limit == ISBITMAP) {
							reloadbmp(item);
						}
					}
				}
			}
		}
	}
}
void set2::reloadbmp(sitem item)
{
	DeleteObject(hbitmaps[item.BitmapNumber]);
	hbitmaps[item.BitmapNumber] =
		(HBITMAP)LoadImage(NULL, config::getpath(item.ConfigName).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hbitmaps[item.BitmapNumber], sizeof(BITMAP), bitmaps[item.BitmapNumber]);
}
void set2::rereadconfig() {
	offmusic = config::getint(OFFMUSIC);
	fullscreen = !config::getint(INWINDOW);
	offvideo = config::getint(OFF_VIDEO);
	FloatWindow = config::getint(FLOATWINDOW);
	if (offmusic)
		mciSendString(L"stop bgm", NULL, 0, NULL);
	else
		mciSendString(L"play bgm repeat", NULL, 0, NULL);
}
void set2::release()
{
	for (int i = 0; i < BitmapCounts; i++) {
		memset(bitmaps[i], 0, sizeof(BITMAP));
		bitmaps[i] = nullptr;
	}
}
void set2::reinit()
{
	init();
}
void set2::changepage()
{
	char n = 0;
	while (n <= 20) {
		if (textboxhwnd[n] != 0) {
			DestroyWindow(textboxhwnd[n]);
		}
		n++;
	}
	for (n = 0; n < 40; n++)isused[n] = 0;
	firstpaint = 1;
}
void set2::showitem(sitem item, HDC hdc, HDC hdcMem) {
	if (item.IsSwitch) {
		switchbm(item, hdc, hdcMem);
	}
	else if (item.IsEditBox) {
		textbox(item, hdc, hdcMem);
	}
}
void set2::seteditbox(LPARAM lParam, WPARAM wParam)
{
	// 获取文本框的句柄,确保它是有效的
	HWND editBoxHwnd = (HWND)(lParam);
	int numberoftextbox = LOWORD(wParam);
	if (editBoxHwnd != NULL) {
		// 分配缓冲区大小,这里假设文本框中的文本不会超过256个字符
		wchar_t sz[256];
		Edit_GetText(editBoxHwnd, sz, 256);
		// 显示文本框中的文本
		if (wcslen(sz) != 0) {
			std::wstring tmp(sz);
			for (const auto& item : pages[settingpage-1].items) {
				if (item.Number == numberoftextbox) EditBoxEditor(item,tmp);
			}
		}
	}
}
void set2::textbox(sitem item, HDC hdc, HDC hdcMem)
{
	SelectObject(hdc, ui::text_mid);
	SetBkColor(hdc, RGB(236, 229, 216));
	SelectObject(hdcMem, hbitmaps[setbutton]);
	TextOut_(hdc, sxy[item.Number].x, sxy[item.Number].y + mywindows::windowHeight * 0.01, item.Name.c_str());
	int number = item.Number;

	std::wstring wst;
	if (item.IsFile) {
		wst = config::getpath(item.ConfigName);
	}
	else {
		wst = config::get(item.ConfigName);
	}
	if (isused[number] == 0) {
		textboxhwnd[number] = CreateEditBox(mywindows::main_hwnd, number, sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, wst.c_str());
	}
	if (item.IsFile) {
		if (!isused[number + 20]) {
			StretchBlt(hdc, sxy[number].bmxend, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
			TextOut_(hdc, sxy[number].bmxend+sxy[number].bmw/2 + mywindows::windowWidth * 0.01, sxy[number].y + mywindows::windowHeight * 0.01, L"打开");
			TextOut_(hdc, sxy[number].bmxend+mywindows::windowWidth*0.01, sxy[number].y + mywindows::windowHeight * 0.01, L"选择");

			int x = sxy[number].bmxend, y = sxy[number].bmy, xend = sxy[number].bmxend + sxy[number].bmw, yend = sxy[number].bmh + sxy[number].bmy;
			if (!isused[number]) {
				sNode* newnode = new sNode;
				if (newnode == nullptr) {
					mywindows::errlog(L"Memory allocation error(textbox)");
				}
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
	isused[number] = 1;
}
void set2::OpenFile(sitem item)
{
	wstring path = config::getpath(item.ConfigName);
	ShellExecute(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
void set2::init()
{
	bitmaps[BackGround] = &bm;
	bitmaps[blue10b] = &ball;
	bitmaps[blue10i] = &ball;
	bitmaps[blue1b] = &ball;
	bitmaps[blue1i] = &ball;
	bitmaps[Buttom] = &buttom;
	bitmaps[cardbackground] = &cardbg;
	bitmaps[exitb] = &exitinfo;
	bitmaps[exiti] = &exitinfo;
	bitmaps[goldencardbg] = &goldenbg;
	bitmaps[list3] = &liststar;
	bitmaps[list4] = &liststar;
	bitmaps[list5] = &liststar;
	bitmaps[list6] = &liststar;
	bitmaps[listbg] = &listbm;
	bitmaps[over1] = &overlay1Bm;
	bitmaps[over2] = &overlay2Bm;
	bitmaps[over3] = &overlay3Bm;
	bitmaps[over4] = &overlay4Bm;
	bitmaps[pink10b] = &ball;
	bitmaps[pink10i] = &ball;
	bitmaps[pink1b] = &ball;
	bitmaps[pink1i] = &ball;
	bitmaps[SetBM] = &setbm;
	bitmaps[setbutton] = &setbu;
	rereadconfig();
}
void set2::EditBoxEditor(sitem item, wstring tmp)
{
	switch (item.Limit)
	{
	case S_WINDOWTITLE:
		SetWindowTextW(mywindows::main_hwnd, tmp.c_str());
		break;
	case BETWEENCOUNT: {
		int value;
		try
		{
			value = std::stoi(tmp);
		}
		catch (const std::exception&e)
		{
			slog << "[ERROR]meet a error :" << e.what() << endl;
			MessageBox(NULL, L"请输入数字", L"错误", MB_ICONERROR);
			return;
		}
		if (value < item.min || value > item.max) {
			MessageBoxA(NULL, U2G(item.OutOfLimitOutPut).c_str(), "错误", MB_ICONERROR);
			return;
		}
	}break;
	case ISFILE:
	case ISBITMAP:
		if (!std::filesystem::exists(tmp))return;
		if (tmp == Log::wrunpath + config::get(item.ConfigName))return;
		break;
	default:
		break;
	}
	config::replace(item.ConfigName, tmp);
}
set2::~set2() {
}
void set2::switchbm(sitem item, HDC hdc, HDC hdcMem) {
	SelectObject(hdc, ui::text_mid);
	SetBkColor(hdc, RGB(236, 229, 216));
	SelectObject(hdcMem, hbitmaps[setbutton]);
	StretchBlt(hdc, sxy[item.Number].bmx, sxy[item.Number].bmy, sxy[item.Number].bmw, sxy[item.Number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
	TextOut_(hdc, sxy[item.Number].x, sxy[item.Number].y + mywindows::windowHeight * 0.01, item.Name.c_str());
	if (config::getint(item.ConfigName) == 1)
		TextOut_(hdc, sxy[item.Number].bmx + mywindows::windowWidth * 0.04, sxy[item.Number].bmy + mywindows::windowHeight * 0.01, L"开");
	else
		TextOut_(hdc, sxy[item.Number].bmx + mywindows::windowWidth * 0.04, sxy[item.Number].bmy + mywindows::windowHeight * 0.01, L"关");
	int x = sxy[item.Number].bmx, y = sxy[item.Number].bmy, xend = sxy[item.Number].bmxend, yend = sxy[item.Number].bmh + sxy[item.Number].bmy;
	sNode* newnode = new sNode;
	if (newnode == NULL) {
		mywindows::errlog("Memory allocation error(textbox)");
	};
	newnode->x = x;
	newnode->xend = xend;
	newnode->y = y;
	newnode->yend = yend;
	newnode->next = shead;
	newnode->number = item.Number;
	shead = newnode;
}
void set2::resetplace() {
	mywindows::log(L"Free Setting List complete");
	for (char i = 1; i <= 10; i++) {
		sxy[i].x = mywindows::windowWidth * 0.05;
		sxy[i].y = mywindows::windowHeight * (i) * 0.09;
		sxy[i].bmx = mywindows::windowWidth * 0.25;
		sxy[i].bmxend = mywindows::windowWidth * 0.35;
		sxy[i].bmy = sxy[i].y;
		sxy[i].bmyend = sxy[i].y + mywindows::windowHeight * 0.07;
		sxy[i].bmw = mywindows::windowWidth * 0.1;
		sxy[i].bmh = mywindows::windowWidth * 0.03;
	}
	for (char i = 11; i <= 19; i++) {
		sxy[i].x = mywindows::windowWidth * 0.55;
		sxy[i].y = mywindows::windowHeight * (i - 9) * 0.09;
		sxy[i].bmx = mywindows::windowWidth * 0.75;
		sxy[i].bmxend = mywindows::windowWidth * 0.85;
		sxy[i].bmy = sxy[i].y;
		sxy[i].bmyend = sxy[i].y + mywindows::windowHeight * 0.07;
		sxy[i].bmw = mywindows::windowWidth * 0.1;
		sxy[i].bmh = mywindows::windowWidth * 0.03;
	}
	nextbmx = mywindows::windowWidth * 0.73;
	nextbmy = mywindows::windowHeight * 0.91;
	nextxend = mywindows::windowWidth * 0.752;
	nextyend = mywindows::windowHeight * 0.95;
	lastbmx = mywindows::windowWidth * 0.8;
	lastbmy = mywindows::windowHeight * 0.91;
	lastxend = mywindows::windowWidth * 0.822;
	lastyend = mywindows::windowHeight * 0.95;
}
void set2::ChooseFile(sitem item)
{
loop:
	OPENFILENAMEW ofn = { 0 };
	wchar_t strFilename[MAX_PATH] = { 0 }; // 用于接收文件名

	ofn.lStructSize = sizeof(OPENFILENAMEW); // 结构体大小
	ofn.hwndOwner = mywindows::main_hwnd; // 拥有者窗口句柄,为NULL表示对话框是非模态的,实际应用中一般都要有这个句柄
	if (item.FileType == "All")
		ofn.lpstrFilter = L"所有文件\0 *.*\0\0";
	else if (item.FileType == "bmp")
		ofn.lpstrFilter = L"bmp图片文件(*.bmp)\0 * .bmp\0\0";
	else if (item.FileType == "nameFile")
		ofn.lpstrFilter = L"姓名文件(*.txt)\0 * .txt\0\0";
	else if (item.FileType == "video")
		ofn.lpstrFilter = L"视频文件(*.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v)\0 *.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v\0\0";
	else if (item.FileType == "picture")
		ofn.lpstrFilter = L"图片文件(*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf)\0*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf\0\0";
	ofn.nFilterIndex = 1; // 过滤器索引
	ofn.lpstrFile = strFilename; // 接收返回的文件名,注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFilename); // 缓冲区长度
	ofn.lpstrInitialDir = NULL; // 初始目录为默认
	ofn.lpstrTitle = item.FileChooseWindowName.c_str(); // 使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // 文件、目录必须存在,隐藏只读选项

	if (GetOpenFileNameW(&ofn))
	{
		std::wstring filename(strFilename);
		config::replace(item.ConfigName, filename);
		Edit_SetText(textboxhwnd[item.Number], filename.c_str());
	}
	else
	{
		MessageBoxW(NULL, L"请选择一个文件", NULL, MB_ICONERROR);
		goto loop;
	}
}
void set2::quit() {
	directshow::music(ENTER);
	char n = 0;
	while (n <= 20) {
		if (textboxhwnd[n] != NULL)
			DestroyWindow(textboxhwnd[n]);
		n++;
	}
	ui::screenmode = FIRST_MENU;
	ui::ScreenModeChanged = 1;
	for (n = 0; n < 40; n++)isused[n] = 0;
	firstpaint = 1;
}
void set2::repaint()
{
	firstpaint = 1;
	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
}
HWND set2::CreateEditBox(HWND hWndParent, int number, int x, int y, int w, int h, const wchar_t* words) {
	// 创建EDIT控件的样式
	DWORD editStyle = ES_AUTOHSCROLL | (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER);

	// 创建EDIT控件的附加样式（可选）
	DWORD editExStyle = WS_EX_CLIENTEDGE;

	// 创建文本框
	HWND hEdit = CreateWindowExW(editExStyle, L"EDIT", words, editStyle, x, y, w, h, hWndParent, (HMENU)number, NULL, NULL);

	// 返回文本框句柄
	return hEdit;
}