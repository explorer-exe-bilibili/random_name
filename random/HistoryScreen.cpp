#include "HistoryScreen.h"

#include <fstream>

#include "bitmaps.h"
#include "mywindows.h"
#include "ui.h"
#include <regex>

#include "sth2sth.h"


HistoryScreen::HistoryScreen(Gp* pt, const std::wstring& file_name)
{
	p = pt;
	filename = file_name;
}

HistoryScreen::HistoryScreen()
{
	p = nullptr;
}

HistoryScreen::~HistoryScreen()
{
	xy.clear();
}
void HistoryScreen::setFile(const std::wstring& file_name)
{
	filename = file_name;
	hasRead = false;
}

void HistoryScreen::paint()
{
	setxy();
	show();
}

void HistoryScreen::enter()
{
	ui::ScreenMode = HISTORY;
	explorer::getInstance()->PlayMusic(ENTER);
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}

bool HistoryScreen::ReadHistory()
{
	history.clear();
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}
	std::vector<item> temp;
	char c = 1;
	std::string tmp_string;
	while (std::getline(file, tmp_string) && c <= 20)
	{
		std::wstring tmp_wstring = sth2sth::str2wstru(tmp_string);
		item tmp_item;
		mywindows::logf << tmp_wstring << std::endl;
		if (tmp_string.empty())continue;
		// 正则表达式，用于匹配类型、星级和名字
		static std::wstring currentTimestamp; // 用于存储当前时间戳
		static bool inBrackets = false; // 标记是否在大括号内
		// 正则表达式，用于匹配时间戳、类型、星级和名字
		static std::wregex pattern(LR"(((\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\])?)\[(\d{1})\类型\]\[(\d{1})\星\]\[(([^\])]+)\])?)");
		static std::wregex timestampPattern(LR"((\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\])\{)");
		std::wsmatch matches;
		// 匹配时间戳和左大括号
		if (std::regex_search(tmp_wstring, matches, timestampPattern)) {
			// 匹配到时间戳和左大括号
			currentTimestamp = matches[1];
			inBrackets = true;
			continue;
		}
		if (tmp_wstring.find(L"}") != std::wstring::npos) {
			// 遇到右大括号，结束大括号内的处理
			inBrackets = false;
			continue;
		}
		if (std::regex_search(tmp_wstring, matches, pattern)) {
			// 输出匹配的时间戳（如果在大括号内，则使用记住的时间戳）、类型、星级和名字
			std::wstring timestamp = inBrackets ? currentTimestamp : matches[1];
			tmp_item.time = timestamp;
		}
		else {
			// 未匹配到任何内容，跳过
			continue;
		}
		if (!matches[7].str().empty())
			tmp_item.name = matches[7];
		else
			tmp_item.name = L"unknow";
		try
		{
			tmp_item.star = std::stoi(matches[5]);
			tmp_item.type = std::stoi(matches[4]);
		}
		catch (std::exception& e)
		{
			mywindows::errlogf << "meet a error when reading stars" << e.what() << std::endl;
			tmp_item.star = 0;
			tmp_item.type = -1;
		}
		temp.push_back(tmp_item);
		c++;
	}
	history.push_back(temp);
	file.close();
	hasRead = true;
	return true;
}

void HistoryScreen::getnum(const std::string& input,
	std::string& textPart, std::string& numberPart) {
	size_t pos = input.size();

	// 从字符串末尾开始，找到第一个非数字字符的位置
	while (pos > 0 && iswdigit(input[pos - 1])) {
		--pos;
	}

	// 分割字符串
	textPart = input.substr(0, pos);
	numberPart = input.substr(pos);
}

void HistoryScreen::show()
{
	using namespace std;
	p->Paint(0, 0, mywindows::WW, mywindows::WH, goldencardbg);
	p->DrawString(L"历史记录", ui::text_mid, 0.46 * mywindows::WW, 0.01 * mywindows::WH);
	int totalp = history.size() / 20;
	if (history.size() % 20 != 0)totalp++;
	const wstring t = to_wstring(page + 1) + L"/" + to_wstring(totalp);
	const HDC hdc = p->GetDC();
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(236, 229, 216));
	SelectObject(hdc, ui::icon_mid);
	TextOut_(hdc, nextbmx, nextbmy, L"b");
	TextOut_(hdc, lastbmx, lastbmy, L"c");
	SelectObject(hdc, ui::text_mid);
	TextOut_(hdc, mywindows::WW * 0.765, mywindows::WH * 0.91, t.c_str());
	p->ReleaseDC(hdc);
	if (!hasRead)if (!ReadHistory()) {
		const int back = MessageBox(nullptr, L"姓名文件出现问题，请检查(点击是打开文件)", L"错误", MB_ICONERROR | MB_YESNO);
		if (back == IDYES)ShellExecute(nullptr, L"open", filename.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		ui::ScreenMode = FIRST_SCREEN;
	}
	if (history.size() == 0) {
		MessageBox(nullptr, L"没有历史记录", L"提示", MB_ICONINFORMATION);
		ui::ScreenMode = FIRST_SCREEN;
	}
	for (int j = 0; j < history[page].size(); j++)
	{
#define rgb 255,255,255
		p->DrawString(history[page][j].time, ui::text_mid, xy[j].TimeX, xy[j].y, rgb);
		p->DrawString(history[page][j].name, ui::text_mid, xy[j].x, xy[j].y, rgb);
		p->DrawString(std::to_wstring(history[page][j].star), ui::text_mid, xy[j].starX, xy[j].y, rgb);
		switch (history[page][j].type)
		{
		case -1:
			p->DrawString(L"未知", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
		case 0:
			p->DrawString(L"弓箭", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 1:
			p->DrawString(L"单手剑", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 2:
			p->DrawString(L"法器", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 3:
			p->DrawString(L"双手剑", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 4:
			p->DrawString(L"长枪", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		default: ;
		}
	}
}

void HistoryScreen::changePage() {
	show();
	explorer::getInstance()->PlayMusic(ENTER);
}

void HistoryScreen::quit()
{
	explorer::getInstance()->PlayMusic(ENTER);
	ui::ScreenMode = FIRST_SCREEN;
	ui::ScreenModeChanged = true;
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}


void HistoryScreen::setxy() {
	xy.clear();
	for (char i = 1; i <= 20; i++) {
		Hxy txy;
		txy.x = mywindows::WW * (0.25 + 0.5 * (int)(i / 10));
		txy.y = mywindows::WH * (i % 10) * 0.09;
		if (txy.y == 0) {
			txy.y = mywindows::WH * 0.9;
			txy.x = mywindows::WW * 0.25 + 0.5 * ((int)(i / 10) - 1);
		}
		txy.starX = txy.x + mywindows::WW * 0.05;
		txy.TypeX = txy.x + mywindows::WW * 0.075;
		txy.TimeX = txy.x - mywindows::WW * 0.25;
		xy.push_back(txy);
	}
	nextbmx = mywindows::WW * 0.73;
	nextbmy = mywindows::WH * 0.91;
	nextxend = mywindows::WW * 0.752;
	nextyend = mywindows::WH * 0.95;
	lastbmx = mywindows::WW * 0.8;
	lastbmy = mywindows::WH * 0.91;
	lastxend = mywindows::WW * 0.822;
	lastyend = mywindows::WH * 0.95;
	mywindows::log(L"Set History List complete");
}