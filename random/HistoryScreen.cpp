#include "HistoryScreen.h"

#include <fstream>

#include "bitmaps.h"
#include "mywindows.h"
#include "ui.h"
#include <regex>

#include "sth2sth.h"


HistoryScreen::HistoryScreen(Gp *pt,std::wstring file_name)
{
	p=pt;
	filename=file_name;
}

HistoryScreen::HistoryScreen()
{
	p=NULL;
}

HistoryScreen::~HistoryScreen()
{
	xy.clear();
}

void HistoryScreen::changeGp(Gp* pt)
{
	p=pt;
}

void HistoryScreen::setFile(std::wstring file_name)
{
	filename=file_name;
	hasReaded = 0;
}

void HistoryScreen::paint()
{
	show();
}

bool HistoryScreen::ReadHistory()
{
	history.clear();
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	std::vector<item> temp;
	while (!file.eof())
	{
		item tmp_item;
		std::string tmp_string;
		std::getline(file, tmp_string);
		if (tmp_string.size() == 0)continue;
		// 正则表达式，用于匹配类型、星级和名字
		std::string line;
		std::string currentTimestamp; // 用于存储当前时间戳
		bool inBrackets = false; // 标记是否在大括号内

		// 正则表达式，用于匹配时间戳、类型、星级和名字
		std::regex pattern(R"(\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\])?(\{)?\[?(\d+)类型\]\[(\d+)星\]\[([^\]]+)\])?")");
		std::regex timestampPattern(R"(\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\]\{)");
		std::smatch matches;
		if (std::regex_search(line, matches, timestampPattern)) {
			// 匹配到时间戳和左大括号
			currentTimestamp = matches[1];
			inBrackets = true;
		}
		else if (line == "}") {
			// 遇到右大括号，结束大括号内的处理
			inBrackets = false;
		}
		else if (std::regex_search(line, matches, pattern)) {
			// 输出匹配的时间戳（如果在大括号内，则使用记住的时间戳）、类型、星级和名字
			std::string timestamp = inBrackets ? currentTimestamp : matches[1];
			tmp_item.time = sth2sth::str2wstr(timestamp);
		}

		tmp_item.name = sth2sth::str2wstr(matches[5]);
		try
		{
			tmp_item.star = std::stoi(matches[4]);
			tmp_item.type = std::stoi(matches[3]);
		}
		catch (std::exception& e)
		{
			mywindows::errlogf << "meet a error when reading stars" << e.what() << std::endl;
			tmp_item.star = 0;
			tmp_item.type = -1;
		}
		temp.push_back(tmp_item);
	}
	history.push_back(temp);
	file.close();
	hasReaded = 1;
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
	p->Paint(0, 0, mywindows::windowWidth, mywindows::windowHeight, goldencardbg);
	if (!hasReaded)if (!ReadHistory()) {
		int back = MessageBox(NULL, L"姓名文件出现问题，请检查(点击是打开文件)", L"错误", MB_ICONERROR | MB_YESNO);
		if (back == IDYES)ShellExecute(NULL, L"open", filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
		ui::screenmode = FIRST_MENU;
	}
	if (history.size() == 0) {
		MessageBox(NULL, L"没有历史记录", L"提示", MB_ICONINFORMATION);
		ui::screenmode = FIRST_MENU;
	}
	for (int j = 0; j < history[page].size(); j++)
	{
		p->DrawString(history[page][j].name, ui::text, xy[j].x, xy[j].y);
		p->DrawString(std::to_wstring(history[page][j].star), ui::text, xy[j].starX, xy[j].y);
		p->DrawString(std::to_wstring(history[page][j].type), ui::text, xy[j].TypeX, xy[j].y);
	}
}

void HistoryScreen::changePage() {
	show();
}

void HistoryScreen::click(int x, int y) {
	int totalp= history.size()/40;
	if(history.size() % 40 != 0)totalp++;
	if (x >= ui::exitx AND x <= ui::exitxend AND y >= ui::exity AND y <= ui::exityend)quit();
	else if (x >= nextbmx AND x <= nextxend AND y >= nextbmy AND y <= nextyend) {
		page--;
		if (page < 1) {
			page = totalp;
		}
		changePage();
	}
	else if (x >= lastbmx AND x <= lastxend AND y >= lastbmy AND y <= lastyend) {
		page++;
		if (page > totalp) {
			page = 1;
		}
		changePage();
	}
}

void HistoryScreen::quit()
{
	ui::screenmode = FIRST_MENU;
	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
}


void HistoryScreen::setxy() {
	xy.clear();
	for (char i = 1; i <= 80; i++) {
		Hxy txy;
		txy.x = mywindows::windowWidth * (0.05+0.1*i/10);
		txy.y = mywindows::windowHeight * (i%10) * 0.09;
		if(txy.y==0)txy.y=mywindows::windowHeight*0.9;
		txy.starX = txy.x+mywindows::windowWidth * 0.05;
		txy.TypeX = txy.x+mywindows::windowWidth * 0.075;
		xy.push_back(txy);
	}
	nextbmx = mywindows::windowWidth * 0.73;
	nextbmy = mywindows::windowHeight * 0.91;
	nextxend = mywindows::windowWidth * 0.752;
	nextyend = mywindows::windowHeight * 0.95;
	lastbmx = mywindows::windowWidth * 0.8;
	lastbmy = mywindows::windowHeight * 0.91;
	lastxend = mywindows::windowWidth * 0.822;
	lastyend = mywindows::windowHeight * 0.95;
	mywindows::log(L"Set History List complete");
}