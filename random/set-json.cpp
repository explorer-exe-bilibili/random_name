#include "set-json.h"
#include"sth2sth.h"
#include"config.h"
#include "mywindows.h"
#include "bitmaps.h"
#include "directshow.h"
#include "ui.h"
#include <nlohmann/json.hpp>
#include <thread>

bool set2::offvideo = 0;
bool set2::reran = 0;
bool set2::offmusic = 0;
bool set2::fullscreen = 0;
bool set2::FloatWindow = 0;

Log slog("files\\log\\set-json.log", 1);
using namespace std;

set2::set2() {
	string jsonpath = "files\\setting.json";
	std::thread loading(std::bind(&set2::Load, this, jsonpath));
	loading.detach();
}
set2::~set2() = default;
void set2::quit() {
	if(SetButton::needReboot==1)
	{
		int choose = MessageBox(NULL, L"部分设置重启生效，是否重启？", L"提示", MB_ICONQUESTION | MB_YESNO);
		if (choose == IDYES)
			mywindows::reboot();
		SetButton::needReboot = 0;
	}
	ui::screenmode = FIRST_SCREEN;
	ui::ScreenModeChanged = 1;
	for (auto& i : pages)
	{
		for (auto& b : i.buttons)
		{
			b->unshow();
		}
	}
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
	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
}

void set2::changepage()
{
	for(auto & i:pages)
	{
		for(auto&b:i.buttons)
		{
			b->unshow();
		}
	}
}
void set2::resetplace() {
	mywindows::log(L"Free Setting List complete");
	for(auto&p:pages)
	{
		for(auto&b:p.buttons)
		{
			b->refresh();
		}
	}
	for(auto & b:buttons)
	{
		b.refresh();
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
	p->Paint(0, 0, mywindows::WW, mywindows::WH, SetBM);
	unsigned int totalp = static_cast<int>(pages.size());
	wstring title = pages[settingpage - 1].Title;
	wstring t = to_wstring(settingpage) + L"/" + to_wstring(totalp);
	int titlex, titley;
	int stringWidth = 0.0272 * mywindows::WW * title.length();
	titlex = (mywindows::WW - stringWidth) / 2;
	titley = mywindows::WH * 0.05;
	p->DrawString(title, ui::text_mid, titlex, titley);
	p->DrawString(t, ui::text_mid, mywindows::WW * 0.755, mywindows::WH * 0.91, 236, 229, 216);
	for(auto&b:pages[settingpage-1].buttons)
	{
		b->show();
	}
	for(auto&b:buttons)
	{
		b.paint();
	}
}
void set2::setGp(Gp *p)
{
	this->p = p;
	for(auto&b:buttons)
			b.setGp(p);
	for(auto&a:pages)
		for(auto&b:a.buttons)
			b->setGp(p);
}
void set2::enter()
{
	ui::screenmode = SETTING;
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}
void set2::Load(string jsonpath) {
	init();
	Log slog("files\\log\\set-json.log", 0);
	// 读取JSON文件
	nlohmann::json data{};
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
				if (t.Limit & BETWEENCOUNT) {
					t.max = sItem.value("max", 0);
					t.min = sItem.value("min", 0);
					t.OutOfLimitOutPut = sItem.value("OutOfLimitOutPut", "");
					if (t.max == MAXWINDOWSIZE) {
						t.max = mywindows::screenWidth;
					}
				}
				else if (t.Limit & ISFILE || t.Limit & ISBITMAP) {
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
}

void set2::clicked(int x, int y)
{
	for(auto& i:pages[settingpage-1].buttons)
	{
		i->click(x, y);
	}
	for(auto&i:buttons)
	{
		i.click(CLICK,x, y);
	}
}
void set2::showitem(sitem item) {
	for(auto&i:pages[settingpage-1].buttons)
	{
		i->show();
	}
}
void set2::seteditbox(WPARAM wParam)
{
	int number = LOWORD(wParam);
	for(auto&i:pages[settingpage-1].buttons)
	{
		i->EditBoxUpgrade(number);
	}
}
void set2::regButton()
{
	for (auto& i : pages)
	{
		int a = 0;
		for (auto& c : i.items) {
			i.buttons.push_back(std::make_shared<SetButton>(c));
			i.buttons[a].get()->reConnect();
			a++;
		}
	}
	Button exit, next, last;
	exit.setBmapC(exitBu);
	exit.setxy2WWWH(0.9, 0.045, 0.93, 0.105);
	exit.setDisableStr(1);
	exit.setMusic(ENTER);
	exit.bind([this]()
		{
			this->quit();
		});
	next.setText(L"c");
	next.setDisableBmap(1);
	next.setxy2WWWH(0.8, 0.91, 0.822, 0.95);
	next.setTextColor(236, 229, 216);
	next.setMusic(CLICK_MUSIC);
	next.setFont(&ui::icon_mid);
	next.bind([this]()
		{
			settingpage++;
			if (settingpage > pages.size()) {
				settingpage = 1;
			}
			changepage();
		});
	last = next;
	last.setText(L"b");
	last.setxy2WWWH(0.73, 0.91, 0.752, 0.95);
	last.bind([this]()
		{
			settingpage--;
			if (settingpage < 1) {
				settingpage = pages.size();
			}
			changepage();
		});
	buttons.push_back(exit);
	buttons.push_back(next);
	buttons.push_back(last);
	for(auto&b:buttons)
	{
		b.reConnect();
	}
}
