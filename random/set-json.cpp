#include "set-json.h"
#include"sth2sth.h"
#include"config.h"
#include "mywindows.h"
#include "bitmaps.h"
#include "directshow.h"
#include "ui.h"
#include<nlohmann\json.hpp>
#include<thread>


#define NOLIMIT 0
#define AFTER 2
#define BEFORE 1
#define BETWEENCOUNT 100
#define RESTART 10
#define S_WINDOWTITLE 200
#define ISFILE 300
#define ISBITMAP 400
#define FLOATPIC 5000
#define MAXWINDOWSIZE 1145141919
#define BITMAPC "BitmapNumber"
#define CONFIGNAME "ConfigName"
#define FILECHOOSE "FileChooseWindowName"
#define FILETYPE "FileType"
#define ISEDIT "IsEditBox"
#define ISSWITCH "IsSwitch"
#define LIMIT "Limit"
#define NAME "Name"
#define NUMBER "Number"
#define OUTOFLIMIT "OutOfLimitOutPut"
#define TITLE "Title"
#define MAX "max"
#define MIN "min"

Log slog("files\\log\\set-json.log", 1);
using json = nlohmann::json;
using namespace std;

set2::set2() {
	string jsonpath = "files\\setting.json";
	std::thread loading(std::bind(&set2::Load, this, jsonpath));
	loading.detach();
	PictureNeedReload.BitmapNumber = 0;
	PictureNeedReload.ConfigName = L"";
	PictureNeedReload.FileChooseWindowName = L"";
	PictureNeedReload.FileType = "";
	PictureNeedReload.IsEditBox = 0;
	PictureNeedReload.IsFile = 0;
	PictureNeedReload.IsSwitch = 0;
	PictureNeedReload.Limit = 0;
	PictureNeedReload.max = 0;
	PictureNeedReload.min = 0;
	PictureNeedReload.Name = L"";
	PictureNeedReload.neetreload = 0;
	PictureNeedReload.Number = 0;
	PictureNeedReload.OutOfLimitOutPut = "";
}
set2::~set2() {
}
void set2::quit() {
	directshow::music(ENTER);
	char n = 0;
	while (n <= 20) {
		if (textboxhwnd[n] != NULL)
			DestroyWindow(textboxhwnd[n]);
		n++;
	}
	ui::screenmode = FIRST_SCREEN;
	ui::ScreenModeChanged = 1;
	for (n = 0; n < 40; n++)isused[n] = 0;
	firstpaint = 1;
}
void set2::init()
{
	rereadconfig();
}
void set2::reinit()
{
	init();
}
void set2::repaint()
{
	firstpaint = 1;
	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
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
void set2::resetplace() {
	mywindows::log(L"Free Setting List complete");
	for (char i = 1; i <= 10; i++) {
		sxy[i].x = mywindows::WW * 0.05;
		sxy[i].y = mywindows::WH * (i) * 0.09;
		sxy[i].bmx = mywindows::WW * 0.25;
		sxy[i].bmxend = mywindows::WW * 0.35;
		sxy[i].bmy = sxy[i].y;
		sxy[i].bmyend = sxy[i].y + mywindows::WH * 0.07;
		sxy[i].bmw = mywindows::WW * 0.1;
		sxy[i].bmh = mywindows::WW * 0.03;
	}
	for (char i = 11; i <= 19; i++) {
		sxy[i].x = mywindows::WW * 0.55;
		sxy[i].y = mywindows::WH * (i - 9) * 0.09;
		sxy[i].bmx = mywindows::WW * 0.75;
		sxy[i].bmxend = mywindows::WW * 0.85;
		sxy[i].bmy = sxy[i].y;
		sxy[i].bmyend = sxy[i].y + mywindows::WH * 0.07;
		sxy[i].bmw = mywindows::WW * 0.1;
		sxy[i].bmh = mywindows::WW * 0.03;
	}
	nextbmx = mywindows::WW * 0.73;
	nextbmy = mywindows::WH * 0.91;
	nextxend = mywindows::WW * 0.752;
	nextyend = mywindows::WH * 0.95;
	lastbmx = mywindows::WW * 0.8;
	lastbmy = mywindows::WH * 0.91;
	lastxend = mywindows::WW * 0.822;
	lastyend = mywindows::WH * 0.95;
	firstpaint = 1;
	for(auto & bs:buttons)
	{
		for(auto& b:bs)
		{
			b.refreash();
		}
	}
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
void set2::paint() {
	for(const auto&b:buttons[settingpage-1])
		b.paint();
	if (firstpaint) {
		p->Paint(0, 0, mywindows::WW, mywindows::WH, SetBM);
		firstpaint = 0;
	}
	p->Paint(ui::exitx, ui::exity, ui::exitxend - ui::exitx, ui::exityend - ui::exity, exiti);
	wstring title = pages[settingpage - 1].Title;
	int titlex, titley;
	int stringWidth = 0.0272 * mywindows::WW * title.length();
	titlex = (mywindows::WW - stringWidth) / 2;
	titley = mywindows::WH * 0.05;
	for (const auto& item : pages[settingpage - 1].items) {
		showitem(item);
	}
	unsigned int totalp = static_cast<int>(pages.size());
	wstring t = to_wstring(settingpage) + L"/" + to_wstring(totalp);
	HDC hdc = p->GetDC();
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(236, 229, 216));
	SelectObject(hdc, ui::text_mid);
	TextOut_(hdc, titlex, titley, title.c_str());
	SelectObject(hdc, ui::icon_mid);
	TextOut_(hdc, nextbmx, nextbmy, L"b");
	TextOut_(hdc, lastbmx, lastbmy, L"c");
	SelectObject(hdc, ui::text_mid);
	TextOut_(hdc, mywindows::WW * 0.765, mywindows::WH * 0.91, t.c_str());
	p->ReleaseDC(hdc);
	if (PictureNeedReload.neetreload AND PictureNeedReload.BitmapNumber <= p->bitmaps.size()) {
		Gdiplus::Bitmap* newBitmap = new Gdiplus::Bitmap(config::getpath(PictureNeedReload.ConfigName).c_str());
		if (newBitmap->GetLastStatus() == Gdiplus::Ok) {
			p->bitmaps[PictureNeedReload.BitmapNumber].reset(newBitmap);
		}
		else {
			slog << slog.pt() << "[ERROR]Fail to reste bitmap" << PictureNeedReload.BitmapNumber << endl;
		}
	}
	PictureNeedReload.neetreload = 0;
}
void set2::setGp(Gp *p)
{
	this->p = p;
	for(auto&bs:buttons)
		for (auto&b : bs) {
			b.setGp(p);
		}
}
void set2::enter()
{
	directshow::music(ENTER);
	ui::screenmode = SETTING;
	firstpaint = 1;
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}
void set2::OpenFile(sitem item)
{
	wstring path = config::getpath(item.ConfigName);
	ShellExecute(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
void set2::Load(string jsonpath) {
	init();
	Log slog("files\\log\\set-json.log", 0);
	// 读取JSON文件
	json data{};
	if (!config::getint(NOSETTINGFILE)) {
	openjsonfile:
		std::ifstream fileStream(jsonpath);
		if (!fileStream.is_open()) {
			slog << "Failed to open file" << std::endl;
			if (!std::filesystem::exists(jsonpath))
				rollback(jsonpath);
			goto openjsonfile;
		}
		// 将文件内容解析为JSON对象
		try {
			fileStream >> data;
		}
		catch (const std::exception& e) {
			slog << "[ERROR]Failed to parse JSON: " << e.what() << std::endl;
			rollback(jsonpath);
			return;
		}
	}
	else {
		data = rollback(jsonpath);
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
					if (t.max == MAXWINDOWSIZE) {
						t.max = mywindows::screenWidth;
					}
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
	regButton();
	return;
}



set2::set2(std::string& jsonfile) {
	std::thread loading(std::bind(&set2::Load, this, jsonfile));
	loading.detach();
	PictureNeedReload.BitmapNumber = 0;
	PictureNeedReload.ConfigName = L"";
	PictureNeedReload.FileChooseWindowName = L"";
	PictureNeedReload.FileType = "";
	PictureNeedReload.IsEditBox = 0;
	PictureNeedReload.IsFile = 0;
	PictureNeedReload.IsSwitch = 0;
	PictureNeedReload.Limit = 0;
	PictureNeedReload.max = 0;
	PictureNeedReload.min = 0;
	PictureNeedReload.Name = L"";
	PictureNeedReload.neetreload = 0;
	PictureNeedReload.Number = 0;
	PictureNeedReload.OutOfLimitOutPut = "";
}
void set2::reloadbmp(sitem item)
{
	if (item.BitmapNumber != FLOATPIC) {
		PictureNeedReload = item;
		PictureNeedReload.neetreload = 1;
	}
}
json set2::rollback(string jsonpath) {
	Log slog("files\\log\\set-json.log", 0);
	slog << "try to rollback setting page" << endl;
	// 创建JSON数据
	json j;
	json p;
	json i;
	p[TITLE] = G2U("图片");
	i[NAME] = G2U("卡池1图片"); i[CONFIGNAME] = "over1"; i[FILECHOOSE] = G2U("选择卡池1图片");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP; i[NUMBER] = 1; i[BITMAPC] = 0;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池2图片"); i[CONFIGNAME] = "over2"; i[FILECHOOSE] = G2U("选择卡池2图片");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP; i[NUMBER] = 2; i[BITMAPC] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池3图片"); i[CONFIGNAME] = "over3"; i[FILECHOOSE] = G2U("选择卡池3图片");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP; i[NUMBER] = 3; i[BITMAPC] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("卡池4图片"); i[CONFIGNAME] = "over4"; i[FILECHOOSE] = G2U("选择卡池4图片");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP; i[NUMBER] = 4; i[BITMAPC] = 3;
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
	i[NAME] = G2U("抽卡时名字的R值(RGB)"); i[CONFIGNAME] = "text red"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 255; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("输入一个0-255之间的数字"); i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("抽卡时名字的G值(RGB)"); i[CONFIGNAME] = "text green"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 255; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("输入一个0-255之间的数字"); i[NUMBER] = 6;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("抽卡时名字的B值(RGB)"); i[CONFIGNAME] = "text blue"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 255; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("输入一个0-255之间的数字"); i[NUMBER] = 7;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("抽背卡池"); i[CONFIGNAME] = "special"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 4; i[MIN] = 0;
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
	p[TITLE] = G2U("悬浮窗(重启生效)");
	i[NAME] = G2U("悬浮窗"); i[CONFIGNAME] = "open float window"; i[ISSWITCH] = 1; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("初始x坐标"); i[CONFIGNAME] = "float window x"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT + RESTART; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("大小不能大于屏幕"); i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("初始y坐标"); i[CONFIGNAME] = "float window y"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT + RESTART; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("大小不能大于屏幕"); i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("宽度"); i[CONFIGNAME] = "float window width"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT + RESTART; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("大小不能大于屏幕"); i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("高度"); i[CONFIGNAME] = "float window height"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT + RESTART; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("大小不能大于屏幕"); i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("滑动系数"); i[CONFIGNAME] = "float window Mu"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT + RESTART; i[MAX] = 1; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("大小不能超过1或小于0"); i[NUMBER] = 6;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("悬浮窗图片"); i[CONFIGNAME] = "float window picture"; i[FILECHOOSE] = G2U("选择悬浮窗图片");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP + RESTART; i[NUMBER] = 11; i[BITMAPC] = FLOATPIC;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("关闭音乐"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("杂项");
	i[NAME] = G2U("窗口模式"); i[CONFIGNAME] = "window mode(not full screen)"; i[ISSWITCH] = 1; i[NUMBER] = 1; i[LIMIT] = RESTART;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("标题"); i[CONFIGNAME] = "title name"; i[ISEDIT] = 1; i[LIMIT] = S_WINDOWTITLE; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("设置页面json文件开关"); i[CONFIGNAME] = "cancel setting json file"; i[ISSWITCH] = 1; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("字体兼容模式(出现字体错误再开)"); i[CONFIGNAME] = "the new font is unsuit"; i[ISSWITCH] = 1; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("关闭音乐"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("调试模式"); i[ISSWITCH] = 1; i[NUMBER] = 5; i[CONFIGNAME] = "debug mode"; i[LIMIT] = RESTART;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	// 写入JSON文件
	if (!config::getint(NOSETTINGFILE)) {
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
				return j;
			}
		}
		else {
			slog << "[ERROR]Failed to open file for writing" << std::endl;
		}
	}
	return j;
}
void set2::clicked(int x, int y)
{
	int totalp = pages.size();
	int number = -1;
	int half = 0;
	sNode* current = shead;
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
		directshow::music(CLICK_MUSIC);
		if (number > 0) {
			for (const auto& item : pages[settingpage - 1].items) {
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
			RECT rect = { current->x,current->y,current->xend,current->yend };
			InvalidateRect(mywindows::main_hwnd,&rect, FALSE);
		}
	}
}
void set2::showitem(sitem item) {
	if (item.IsSwitch) {
		switchbm(item);
	}
	else if (item.IsEditBox) {
		textbox(item);
	}
}
void set2::textbox(sitem item)
{
	int number = item.Number;
	std::wstring wst;
	if (item.IsFile) {
		wst = config::getpath(item.ConfigName);
	}
	else {
		wst = config::get(item.ConfigName);
	}
	HDC hdc = p->GetDC();
	SelectObject(hdc, ui::text_mid);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(236, 229, 216));
	TextOut_(hdc, sxy[item.Number].x, sxy[item.Number].y + mywindows::WH * 0.01, item.Name.c_str());
	p->ReleaseDC(hdc);
	if (isused[number] == 0) {
		textboxhwnd[number] = CreateEditBox(mywindows::main_hwnd, number, sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, wst.c_str());
	}
	else
	{
		MoveWindow(textboxhwnd[number], sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, TRUE);
	}
	if (item.IsFile) {
		if (!isused[number]) {
			//p->Paint(sxy[number].bmxend, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, setbutton);
			//HDC hdc = p->GetDC();
			//SelectObject(hdc, ui::text_mid);
			//SetBkMode(hdc, TRANSPARENT);
			//TextOut_(hdc, sxy[number].bmxend + sxy[number].bmw / 2 + mywindows::WW * 0.01, sxy[number].y + mywindows::WH * 0.01, L"打开");
			//TextOut_(hdc, sxy[number].bmxend + mywindows::WW * 0.01, sxy[number].y + mywindows::WH * 0.01, L"选择");
			//p->ReleaseDC(hdc);
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
void set2::switchbm(sitem item) {
	p->Paint(sxy[item.Number].bmx, sxy[item.Number].bmy, sxy[item.Number].bmw, sxy[item.Number].bmh, setbutton);
	HDC hdc = p->GetDC();
	SelectObject(hdc, ui::text_mid);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(236, 229, 216));
	TextOut_(hdc, sxy[item.Number].x, sxy[item.Number].y + mywindows::WH * 0.01, item.Name.c_str());
	SetBkMode(hdc, OPAQUE);
	SetBkColor(hdc, RGB(236, 229, 216));
	SetTextColor(hdc, RGB(0, 0, 0));
	if (config::getint(item.ConfigName) == 1)
		TextOut_(hdc, sxy[item.Number].bmx + mywindows::WW * 0.04, sxy[item.Number].bmy + mywindows::WH * 0.01, L"开");
	else
		TextOut_(hdc, sxy[item.Number].bmx + mywindows::WW * 0.04, sxy[item.Number].bmy + mywindows::WH * 0.01, L"关");
	p->ReleaseDC(hdc);
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
	if (item.FileType == "nameFile") {
		reran = 1;
	}
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
		catch (const std::exception& e)
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
			for (const auto& item : pages[settingpage - 1].items) {
				if (item.Number == numberoftextbox) EditBoxEditor(item, tmp);
			}
		}
	}
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

void set2::regButton()
{
	for(const auto& i:pages)
	{
		std::vector<Button> bs;
		for (const auto& t : i.items)
		{
			if (t.IsSwitch)
			{
				Button b;
				double x, y, xend, yend;
				if (t.Number <= 10)
				{
					x = 0.25;
					y = t.Number * 0.09;
					xend = 0.35;
					yend = y + 0.07;
				}
				else if (t.Number <= 19)
				{
					x = 0.75;
					y = (t.Number - 9) * 0.09;
					xend = 0.85;
					yend = y + 0.07;
				}
				else
				{
					x = 0; y = 0; xend = 0; yend = 0;
				}
				b.setxy2WWWH(x, y, xend, yend);
				b.bind(
					[&] {
						config::turnUpSideDown(t.ConfigName);
						rereadconfig();
						if (config::getint(t.ConfigName) == 1)
							b.setText(L"开");
						else
							b.setText(L"关");
					}
				);
				b.setBmapC(setbutton);
				if (config::getint(t.ConfigName) == 1)
					b.setText(L"开");
				else
					b.setText(L"关");
				bs.push_back(b);
			}
			else if (t.IsFile)
			{
				Button b1,b2;
				double x, y, xend1,xend2, yend;
				if(t.Number<=10)
				{
					x = 0.35;
					y= t.Number * 0.09;
					xend1= 0.4;
					xend2 = 0.45;
					yend= y + 0.07;
				}
				else if(t.Number<=19)
				{
					x = 0.75;
					y= (t.Number - 9) * 0.09;
					xend1= 0.8;
					xend2 = 0.85;
					yend= y + 0.07;
				}
				else
				{
					x = 0; y = 0; xend1 = 0; xend2 = 0; yend = 0;
				}
				b1.setxy2WWWH(x, y, xend1, yend);
				b2.setxy2WWWH(xend1, y, xend2, yend);
				b1.setBmapC(setbutton);
				b2.setBmapC(setbutton);
				b1.setText(L"选择");
				b2.setText(L"打开");
				b1.setTextColor(0, 0, 0);
				b2.setTextColor(0, 0, 0);
				b1.setFont(&ui::text_mid);
				b2.setFont(&ui::text_mid);
				b1.bind(
					[&] {
						ChooseFile(t);
						if (t.Limit == ISBITMAP)
						{
							reloadbmp(t);
						}
					}
				);
				b2.bind(
					[&] {
						OpenFile(t);
					}
				);
				bs.push_back(b1);
				bs.push_back(b2);
			}
		}
		buttons.push_back(bs);
	}
}
