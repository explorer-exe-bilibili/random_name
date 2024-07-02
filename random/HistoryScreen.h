#pragma once
#include "Gp.h"
#include <string>
#include <vector>
class HistoryScreen
{
	bool ReadHistory();
	void getnum(const std::string& input, std::string& textPart, std::string& numberPart);
	bool hasReaded=0;
	struct item
	{
		int star;
		int type;
		std::wstring name;
		std::wstring time;
	};
	typedef struct Hxy {
		int x, y, starX, TypeX;
	}Hxy;

	int lastbmx = 0, lastbmy = 0, lastxend = 0, lastyend = 0;
	int nextbmx = 0, nextbmy = 0, nextxend = 0, nextyend = 0;
	int page=0;
	std::vector<std::vector<item>> history;
	std::vector<Hxy> xy;
	void setxy();
	void quit();
	void changePage();
	void show();
public:
	std::wstring filename;
	Gp *p=nullptr;
	HistoryScreen(Gp *pt,std::wstring file_name);
	HistoryScreen();
	~HistoryScreen();
	void changeGp(Gp* pt);
	void click(int x, int y);
	void setFile(std::wstring file_name);
	void paint();
};

