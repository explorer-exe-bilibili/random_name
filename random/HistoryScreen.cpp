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
		// ������ʽ������ƥ�����͡��Ǽ�������
		static std::wstring currentTimestamp; // ���ڴ洢��ǰʱ���
		static bool inBrackets = false; // ����Ƿ��ڴ�������
		// ������ʽ������ƥ��ʱ��������͡��Ǽ�������
		static std::wregex pattern(LR"(((\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\])?)\[(\d{1})\����\]\[(\d{1})\��\]\[(([^\])]+)\])?)");
		static std::wregex timestampPattern(LR"((\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\])\{)");
		std::wsmatch matches;
		// ƥ��ʱ������������
		if (std::regex_search(tmp_wstring, matches, timestampPattern)) {
			// ƥ�䵽ʱ������������
			currentTimestamp = matches[1];
			inBrackets = true;
			continue;
		}
		if (tmp_wstring.find(L"}") != std::wstring::npos) {
			// �����Ҵ����ţ������������ڵĴ���
			inBrackets = false;
			continue;
		}
		if (std::regex_search(tmp_wstring, matches, pattern)) {
			// ���ƥ���ʱ���������ڴ������ڣ���ʹ�ü�ס��ʱ����������͡��Ǽ�������
			std::wstring timestamp = inBrackets ? currentTimestamp : matches[1];
			tmp_item.time = timestamp;
		}
		else {
			// δƥ�䵽�κ����ݣ�����
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

	// ���ַ���ĩβ��ʼ���ҵ���һ���������ַ���λ��
	while (pos > 0 && iswdigit(input[pos - 1])) {
		--pos;
	}

	// �ָ��ַ���
	textPart = input.substr(0, pos);
	numberPart = input.substr(pos);
}

void HistoryScreen::show()
{
	using namespace std;
	p->Paint(0, 0, mywindows::WW, mywindows::WH, goldencardbg);
	p->DrawString(L"��ʷ��¼", ui::text_mid, 0.46 * mywindows::WW, 0.01 * mywindows::WH);
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
		const int back = MessageBox(nullptr, L"�����ļ��������⣬����(����Ǵ��ļ�)", L"����", MB_ICONERROR | MB_YESNO);
		if (back == IDYES)ShellExecute(nullptr, L"open", filename.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		ui::ScreenMode = FIRST_SCREEN;
	}
	if (history.size() == 0) {
		MessageBox(nullptr, L"û����ʷ��¼", L"��ʾ", MB_ICONINFORMATION);
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
			p->DrawString(L"δ֪", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
		case 0:
			p->DrawString(L"����", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 1:
			p->DrawString(L"���ֽ�", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 2:
			p->DrawString(L"����", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 3:
			p->DrawString(L"˫�ֽ�", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
			break;
		case 4:
			p->DrawString(L"��ǹ", ui::text_mid, xy[j].TypeX, xy[j].y, rgb);
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