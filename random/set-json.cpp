// ReSharper disable CppClangTidyCppcoreguidelinesNarrowingConversions
// ReSharper disable CppClangTidyClangDiagnosticImplicitIntFloatConversion
#include "set-json.h"
#include"sth2sth.h"
#include"configitem.h"
#include "mywindows.h"
#include "bitmaps.h"
#include "ui.h"
#include <nlohmann/json.hpp>
#include <thread>

#include "Button.h"
#include "Gp.h"
#include "SetButton.h"

bool set2::offVideo = false;
bool set2::reran = false;
bool set2::offMusic = false;
bool set2::fullscreen = false;
bool set2::FloatWindow = false;

Log slog("files\\log\\set-json.log", true);
using namespace std;

set2::set2() {
	string jsonpath = "files\\setting.json";
	std::thread loading([this, jsonpath] { Load(jsonpath); });
	loading.detach();
}
set2::~set2() = default;
void set2::quit() const
{
	if(SetButton::needReboot==1)
	{
		const int choose = MessageBox(nullptr, L"部分设置重启生效，是否重启？", L"提示", MB_ICONQUESTION | MB_YESNO);
		if (choose == IDYES)
			mywindows::reboot();
		SetButton::needReboot = false;
	}
	ui::ScreenMode = FIRST_SCREEN;
	ui::ScreenModeChanged = true;
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
	rereadConfig();
}
void set2::reinit()
{
	init();
}

void set2::changePage() const
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
		for(const auto&b:p.buttons)
		{
			b->refresh();
		}
	}
	for(auto & b:buttons)
	{
		b.refresh();
	}
}
void set2::rereadConfig() {
	offMusic = config::getint(OFFMUSIC);
	fullscreen = !config::getint(INWINDOW);
	offVideo = config::getint(OFF_VIDEO);
	FloatWindow = config::getint(FLOATWINDOW);
	if (offMusic)
		mciSendString(L"stop bgm", nullptr, 0, nullptr);
	else
		mciSendString(L"play bgm repeat", nullptr, 0, nullptr);
}
void set2::paint() const
{
	p->Paint(0, 0, mywindows::WW, mywindows::WH, SetBM);
	const unsigned int totalp = static_cast<int>(pages.size());
	const wstring title = pages[page - 1].Title;
	const wstring t = to_wstring(page) + L"/" + to_wstring(totalp);
	const int stringWidth = 0.0272 * mywindows::WW * title.length();
	const int titlex = (mywindows::WW - stringWidth) / 2;
	const int titley = mywindows::WH / 20;
	p->DrawString(title, ui::text_mid, titlex, titley);
	p->DrawString(t, ui::text_mid, mywindows::WW * 0.755, mywindows::WH * 0.91, 236, 229, 216);
	for(auto&b:pages[page-1].buttons)
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
		for(const auto&b:a.buttons)
			b->setGp(p);
}
void set2::enter()
{
	ui::ScreenMode = SETTING;
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}
void set2::Load(const string& jsonpath) {
	init();
	Log log("files\\log\\set-json.log", false);
	// 读取JSON文件
	nlohmann::json data{};
	if (!config::getint(NOSETTINGFILE)) {
	openjsonfile:
		std::ifstream fileStream(jsonpath);
		if (!fileStream.is_open()) {
			log << "Failed to open file" << '\n';
			if (!std::filesystem::exists(jsonpath))
				rollback(jsonpath);
			goto openjsonfile;  // NOLINT(cppcoreguidelines-avoid-goto, hicpp-avoid-goto)
		}
		// 将文件内容解析为JSON对象
		try {
			fileStream >> data;
		}
		catch (const std::exception& e) {
			log << "[ERROR]Failed to parse JSON: " << e.what() << '\n';
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
				t.IsDir = sItem.value("IsDir", 0);
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
					t.IsEditBox = true;
					t.IsFile = true;
				}
			}
			catch (const std::exception& e)
			{
				log << log.pt() << "[ERROR]FAiled with reading json" << e.what() << '\n';
				t.IsFile = false;
				t.IsEditBox = false;
				t.IsSwitch = false;
				t.IsDir = false;
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
		pt.ItemCount = in;
		pages.push_back(pt);
		sp++;
	}
	page = 1;
	regButton();
	return;
}
set2::set2(std::string& jsonfile) {
	std::thread loading([this, jsonfile] { Load(jsonfile); });
	loading.detach();
}

void set2::clicked(int x, int y) const
{
	for(auto& i:pages[page-1].buttons)
	{
		i->click(x, y);
	}
	for(auto&i:buttons)
	{
		i.click(CLICK,x, y);
	}
}
void set2::showItem(const sitem& item) const
{
	for(auto&i:pages[page-1].buttons)
	{
		i->show();
	}
}
void set2::setEditBox(WPARAM wParam) const
{
	const int number = LOWORD(wParam);
	for(auto&i:pages[page-1].buttons)
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
			i.buttons[a]->reConnect();
			a++;
		}
	}
	Button exit, next, last;
	exit.setBmapC(exitBu);
	exit.setxy2WWWH(0.9, 0.045, 0.93, 0.105);
	exit.setDisableStr(true);
	exit.setMusic(ENTER);
	exit.bind([this]()
		{
			this->quit();
		});
	next.setText(L"c");
	next.setDisableBmap(true);
	next.setxy2WWWH(0.8, 0.91, 0.822, 0.95);
	next.setTextColor(236, 229, 216);
	next.setMusic(CLICK_MUSIC);
	next.setFont(&ui::icon_mid);
	next.bind([this]()
		{
			page++;
			if (page > pages.size()) {
				page = 1;
			}
			changePage();
		});
	last = next;
	last.setText(L"b");
	last.setxy2WWWH(0.73, 0.91, 0.752, 0.95);
	last.bind([this]()
		{
			page--;
			if (page < 1) {
				page = pages.size();
			}
			changePage();
		});
	buttons.push_back(exit);
	buttons.push_back(next);
	buttons.push_back(last);
	for(auto&b:buttons)
	{
		b.reConnect();
	}
}
