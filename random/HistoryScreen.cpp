#include "HistoryScreen.h"

#include <fstream>

#include "mywindows.h"
#include "ui.h"


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

}

bool HistoryScreen::changeGp(Gp *pt)
{
	p=pt;
}

bool HistoryScreen::setFile(std::wstring file_name)
{
	filename=file_name;
	hasReaded = 0;
}

bool HistoryScreen::ReadHistory()
{
	std::wifstream file(filename);
	if(!file.is_open())
	{
		return false;
	}
	std::vector<item> temp;
	while(!file.eof())
	{
		item tmp_item;
		std::wstring tmp_wstring,name_part,star_part;
		int star,type;
		std::getline(file,tmp_wstring);
		if(tmp_wstring.size()==0)break;
		getnum(tmp_wstring,name_part,star_part);
		tmp_item.name=name_part;
		try
		{
			star=std::stoi(star_part);
			tmp_item.star=star%10;
			tmp_item.type=star/10;
			temp.push_back(tmp_item);
		}
		catch (std::exception& e)
		{
			mywindows::errlogf<<"meet a error when reading stars"<<e.what()<<std::endl;
			return false;
		}
	}
	history.push_back(temp);
	file.close();
	hasReaded = 1;
	return true;
}

void HistoryScreen::getnum(const std::wstring& input,
	std::wstring& textPart, std::wstring& numberPart) {
	size_t pos = input.size();

	// 从字符串末尾开始，找到第一个非数字字符的位置
	while (pos > 0 && iswdigit(input[pos - 1])) {
		--pos;
	}

	// 分割字符串
	textPart = input.substr(0, pos);
	numberPart = input.substr(pos);
}

bool HistoryScreen::show()
{
	if (!hasReaded)if (!ReadHistory()) {
		int back = MessageBox(NULL, L"姓名文件出现问题，请检查(点击是打开文件)", L"错误", MB_ICONERROR | MB_YESNO);
		if(back==IDYES)ShellExecute(NULL, L"open", filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
		ui::screenmode = FIRST_MENU;
	}
	
}