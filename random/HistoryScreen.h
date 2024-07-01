#pragma once
#include "Gp.h"
#include <string>
#include <vector>
class HistoryScreen
{
	bool ReadHistory();
	void getnum(const std::wstring& input, std::wstring& textPart, std::wstring& numberPart);
	bool hasReaded=0;
	struct item
	{
		int star;
		int type;
		std::wstring name;
	};
	std::vector<std::vector<item>> history;
public:
	std::wstring filename;
	Gp *p;
	HistoryScreen(Gp *pt,std::wstring file_name);
	HistoryScreen();
	~HistoryScreen();
	bool show();
	bool changeGp(Gp *pt);
	bool click();
	bool setFile(std::wstring file_name);
	bool quit();
};

