#pragma once
#include <string>

#define NOLIMIT 0
#define AFTER 2
#define BEFORE 1


enum limits
{
	BETWEENCOUNT = 1 << 0,
	REBOOT = 1 << 1,
	S_WINDOWTITLE = 1 << 2,
	ISFILE = 1 << 3,
	ISBITMAP = 1 << 4,
	FLOATPIC = 1 << 5
};
#define MAXWINDOWSIZE 1145141919
#define BITMAPC "BitmapNumber"
#define CONFIGNAME "ConfigName"
#define FILECHOOSE "FileChooseWindowName"
#define FILETYPE "FileType"
#define ISEDIT "IsEditBox"
#define ISSWITCH "IsSwitch"
#define ISDIR "IsDir"
#define LIMIT "Limit"
#define NAME "Name"
#define NUMBER "Number"
#define OUTOFLIMIT "OutOfLimitOutPut"
#define TITLE "Title"
#define MAX "max"
#define MIN "min"

struct sitem {
	bool IsEditBox = false;
	bool IsFile = false;
	bool IsSwitch = false;
	bool IsDir = false;
	int Number = 0;
	int max = 0;
	int min = 0;
	int Limit = 0;
	int BitmapNumber = 0;
	std::wstring Name;
	std::wstring ConfigName;
	std::wstring FileChooseWindowName;
	std::string FileType;
	std::string OutOfLimitOutPut;
	bool needReload = false;
};
