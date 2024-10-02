#pragma once
#include "Gp.h"
#include <string>
#include <vector>

#include "Screen.h"
class Button;

class HistoryScreen :public Screen
{
	bool ReadHistory();
	static void getnum(const std::string& input, std::string& textPart, std::string& numberPart);
	bool hasRead=false;
	struct item
	{
		int star;
		int type;
		std::wstring name;
		std::wstring time;
	};
	typedef struct Hxy {
		int x, y, starX, TypeX,TimeX;
	}Hxy;

	int lastbmx = 0, lastbmy = 0, lastxend = 0, lastyend = 0;
	int nextbmx = 0, nextbmy = 0, nextxend = 0, nextyend = 0;
	int page=0;
	std::vector<std::vector<item>> history;
	std::vector<Hxy> xy;
	std::vector<Button> buttons;
	void setxy();
	static void quit();
	void changePage();
	void show();
public:
	std::wstring filename;
	Gp *p=nullptr;
	HistoryScreen(Gp *pt, const std::wstring& file_name);
	HistoryScreen();
	~HistoryScreen() override;
	void setFile(const std::wstring& file_name);
	void paint();
	static void enter();
};

