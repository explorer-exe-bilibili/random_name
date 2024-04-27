#include "set-json.h"
#include"nlohmann/json.hpp"
#include"sth2sth.h"
#include"config.h"
#include "mywindows.h"
#include "bitmaps.h"
#include "directshow.h"
#include "ui.h"

#define BETWEENCOUNT 100
#define S_WINDOWTITLE 200
#define ISFILE 300
#define ISBITMAP 400
#define NOLIMIT 0

using json = nlohmann::json;
using namespace std;
Log setlog("set-json.log", 0);
extern HBITMAP hbitmaps[BitmapCounts];
extern BITMAP overlay1Bm, bm, ball, overlay2Bm, overlay3Bm, overlay4Bm, cardbg, exitinfo, goldenbg, listbm, liststar, buttom;

set2::set2() {
	{
		bitmaps[background] = &bm;
		bitmaps[blue10b] = &ball;
		bitmaps[blue10i] = &ball;
		bitmaps[blue1b] = &ball;
		bitmaps[blue1i] = &ball;
		bitmaps[Buttom] = &buttom;
		bitmaps[cardbackground] = &cardbg;
		bitmaps[exitb] = &exitinfo;
		bitmaps[exiti] = &exitinfo;
		bitmaps[goldencardbg] = &goldenbg;
		bitmaps[list3] = &liststar;
		bitmaps[list4] = &liststar;
		bitmaps[list5] = &liststar;
		bitmaps[list6] = &liststar;
		bitmaps[listbg] = &listbm;
		bitmaps[over1] = &overlay1Bm;
		bitmaps[over2] = &overlay2Bm;
		bitmaps[over3] = &overlay3Bm;
		bitmaps[over4] = &overlay4Bm;
		bitmaps[pink10b] = &ball;
		bitmaps[pink10i] = &ball;
		bitmaps[pink1b] = &ball;
		bitmaps[pink1i] = &ball;
		bitmaps[SetBM] = &setbm;
		bitmaps[setbutton] = &setbu;
	}
	offmusic = config::getint(OFFMUSIC);
	fullscreen = config::getint(INWINDOW);
	// 读取JSON文件
	string jsonpath = "files\\setting.json";
	std::ifstream fileStream(jsonpath);
	if (!fileStream.is_open()) {
		setlog << "Failed to open file" << std::endl;
		if (!std::filesystem::exists(jsonpath))
			rollback(jsonpath);
		return;
	}
	// 将文件内容解析为JSON对象
	json data{};
	try {
		fileStream >> data;
	}
	catch (const std::exception& e) {
		setlog << "Failed to parse JSON: " << e.what() << std::endl;
		return;
	}
	int sp = 0;//settingpage
	int in = 0;//ItemNumber
	for (const auto& page : data["pages"]) {
		in = 0;
		spage pt;
		pt.Title = sth2sth::str2wstr(page.value("Title", ""));
		for (const auto& sItem : page["item"]) {
			sitem t;
			t.IsFile = sItem.value("IsFile", 0);
			t.IsEditBox = sItem.value("IsEditBox", 0);
			t.IsSwitch = sItem.value("IsSwitch", 0);
			t.Limit = sItem.value("Limit", NOLIMIT);
			t.Number = sItem.value("Number", in);
			t.BitmapNumber = sItem.value("BitmapNumber", 0);
			t.Name = sth2sth::str2wstr(sItem.value("Name", ""));
			t.ConfigName = sth2sth::str2wstr(sItem.value("ConfigName", ""));
			t.FileChooseWindowName = sth2sth::str2wstr(sItem.value("FileChooseWindowName", ""));
			{
				string tmp = sItem.value("FileType", "All");
				if (tmp == "All" AND t.IsFile)
					t.FileType = L"所有文件\0 *.*\0\0";
				else if (tmp == "bmp")
					t.FileType = L"bmp图片文件(*.bmp)\0 * .bmp\0\0";
				else if (tmp == "nameFile")
					t.FileType = L"姓名文件(*.txt)\0 * .txt\0\0";
				else if (tmp == "video")
					t.FileType = L"视频文件(*.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v)\0 *.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v\0\0";
				else if (tmp == "picture")
					t.FileType = L"图片文件(*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf)\0*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf\0\0";
				else t.FileType = L"";
			}
			if (t.Limit == BETWEENCOUNT) {
				t.max = sItem.value("max", 0);
				t.min = sItem.value("min", 0);
				t.OutOfLimitOutPut = sth2sth::str2wstr(sItem.value("OutOfLimit", ""));
			}
			pt.items.push_back(t);
			in++;
		}
		pt.itemcount = in;
		pages.push_back(pt);
		sp++;
	}
	return;
}
set2::set2(std::string& jsonfile) {
	{

		bitmaps[background] = &bm;
		bitmaps[blue10b] = &ball;
		bitmaps[blue10i] = &ball;
		bitmaps[blue1b] = &ball;
		bitmaps[blue1i] = &ball;
		bitmaps[Buttom] = &buttom;
		bitmaps[cardbackground] = &cardbg;
		bitmaps[exitb] = &exitinfo;
		bitmaps[exiti] = &exitinfo;
		bitmaps[goldencardbg] = &goldenbg;
		bitmaps[list3] = &liststar;
		bitmaps[list4] = &liststar;
		bitmaps[list5] = &liststar;
		bitmaps[list6] = &liststar;
		bitmaps[listbg] = &listbm;
		bitmaps[over1] = &overlay1Bm;
		bitmaps[over2] = &overlay2Bm;
		bitmaps[over3] = &overlay3Bm;
		bitmaps[over4] = &overlay4Bm;
		bitmaps[pink10b] = &ball;
		bitmaps[pink10i] = &ball;
		bitmaps[pink1b] = &ball;
		bitmaps[pink1i] = &ball;
		bitmaps[SetBM] = &setbm;
		bitmaps[setbutton] = &setbu;
	}
	offmusic = config::getint(OFFMUSIC);
	fullscreen = config::getint(INWINDOW);
	// 读取JSON文件
	std::ifstream fileStream(jsonfile);
	if (!fileStream.is_open()) {
		setlog << "Failed to open file" << std::endl;
		if (!std::filesystem::exists(jsonfile))
			rollback(jsonfile);
		return;
	}
	// 将文件内容解析为JSON对象
	json data{};
	try {
		fileStream >> data;
	}
	catch (const std::exception& e) {
		setlog << "Failed to parse JSON: " << e.what() << std::endl;
		return;
	}
	int sp = 0;//settingpage
	int in = 0;//ItemNumber
	for (const auto& page : data["pages"]) {
		in = 0;
		spage pt;
		pt.Title = sth2sth::str2wstr(page.value("Title", ""));
		for (const auto& sItem : page["item"]) {
			sitem t;
			t.IsFile = sItem.value("IsFile", 0);
			t.IsEditBox = sItem.value("IsEditBox", 0);
			t.IsSwitch = sItem.value("IsSwitch", 0);
			t.Limit = sItem.value("Limit", NOLIMIT);
			t.Number = sItem.value("Number", in);
			t.BitmapNumber = sItem.value("BitmapNumber", 0);
			t.Name = sth2sth::str2wstr(sItem.value("Name", ""));
			t.ConfigName = sth2sth::str2wstr(sItem.value("ConfigName", ""));
			t.FileChooseWindowName = sth2sth::str2wstr(sItem.value("FileChooseWindowName", ""));
			{
				string tmp = sItem.value("FileType", "All");
				if (tmp == "All" AND t.IsFile)
					t.FileType = L"所有文件\0 *.*\0\0";
				else if (tmp == "bmp")
					t.FileType = L"bmp图片文件(*.bmp)\0 * .bmp\0\0";
				else if (tmp == "nameFile")
					t.FileType = L"姓名文件(*.txt)\0 * .txt\0\0";
				else if (tmp == "video")
					t.FileType = L"视频文件(*.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v)\0 *.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v\0\0";
				else if (tmp == "picture")
					t.FileType = L"图片文件(*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf)\0*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf\0\0";
				else t.FileType = L"";
			}
			if (t.Limit == BETWEENCOUNT) {
				t.max = sItem.value("max", 0);
				t.min = sItem.value("min", 0);
				t.OutOfLimitOutPut = sth2sth::str2wstr(sItem.value("OutOfLimit", ""));
			}
			else if (t.Limit == ISFILE || t.Limit == ISBITMAP) {
				t.IsEditBox = 1;
				t.IsFile = 1;
			}
			pt.items.push_back(t);
			in++;
		}
		pt.itemcount = in;
		pages.push_back(pt);
		sp++;
	}
	return;
}
void set2::paint(HDC hdc, HDC hdcMem) {
	if (firstpaint) {
		SelectObject(hdcMem, hbitmaps[SetBM]);
		StretchBlt(hdc, 0, 0, mywindows::windowWidth, mywindows::windowHeight, hdcMem, 0, 0, setbm.bmWidth, setbm.bmHeight, SRCCOPY);
		firstpaint = 0;
	}
	SelectObject(hdc, ui::text_mid);
	wstring title = pages[settingpage].Title;
	int titlex, titley;
	int stringWidth = 0.0272 * mywindows::windowWidth * title.length();
	titlex= (mywindows::windowWidth - stringWidth) / 2;
	titley = mywindows::windowHeight * 0.09;
	TextOut_(hdc, titlex, titley, title.c_str());
	SelectObject(hdcMem, hbitmaps[exitb]);
	StretchBlt(hdc, ui::exitx, ui::exity, ui::exitxend - ui::exitx, ui::exityend - ui::exity, hdcMem, 0, 0, exitinfo.bmWidth, exitinfo.bmHeight, SRCAND);
	SelectObject(hdcMem, hbitmaps[exiti]);
	StretchBlt(hdc, ui::exitx, ui::exity, ui::exitxend - ui::exitx, ui::exityend - ui::exity, hdcMem, 0, 0, exitinfo.bmWidth, exitinfo.bmHeight, SRCPAINT);
	for (const auto& item : pages[settingpage].items) {
		showitem(item, hdc, hdcMem);
	}
	unsigned int totalp = static_cast<int>(pages.size());
	wstring t = to_wstring(settingpage) + L"/" + to_wstring(totalp);
	SelectObject(hdc, ui::icon_mid);
	TextOut_(hdc, nextbmx, nextbmy, L"b");
	TextOut_(hdc, lastbmx, lastbmy, L"c");
	SelectObject(hdc, ui::text_mid);
	TextOut_(hdc, mywindows::windowWidth * 0.765, mywindows::windowHeight * 0.91, t.c_str());

}
void set2::rollback(string jsonpath) {
	json data;
	data = {
		{"Title","图片",
		{"item",{
			"Name","卡池1图片",
			"ConfigName","over1",
			"FileChooseWindowName","选择卡池1图片",
			"FileType","bmp",
			"Limit",ISBITMAP,
			"Number",1,
			"BitmapNumber",0
			},{
			"Name","卡池2图片",
			"ConfigName","over2",
			"FileChooseWindowName","选择卡池2图片",
			"FileType","bmp",
			"Limit",ISBITMAP,
			"Number",2,
			"BitmapNumber",1
			},{
			"Name","卡池3图片",
			"ConfigName","over3",
			"FileChooseWindowName","选择卡池3图片",
			"FileType","bmp",
			"Limit",ISBITMAP,
			"Number",3,
			"BitmapNumber",2
			},{
			"Name","卡池4图片",
			"ConfigName","over4",
			"FileChooseWindowName","选择卡池4图片",
			"FileType","bmp",
			"Limit",ISBITMAP,
			"Number",4,
			"BitmapNumber",3
			},{
			"Name","关闭音乐",
			"ConfigName","off music",
			"IsSwitch",1,
			"Number",11
			},{
			"Name","关闭视频",
			"ConfigName","off video",
			"IsSwitch",1,
			"Number",12
			}
		}},
		{"Title","名单与卡池",
		{"item",{
			"Name","卡池1名单",
			"ConfigName","namesfile1",
			"FileChooseWindowName","选择卡池1名单",
			"FileType","nameFile",
			"Limit",ISFILE,
			"Number",1
			},{
			"Name","卡池2名单",
			"ConfigName","namesfile2",
			"FileChooseWindowName","选择卡池2名单",
			"FileType","nameFile",
			"Limit",ISFILE,
			"Number",2
			},{
			"Name","卡池3名单",
			"ConfigName","namesfile3",
			"FileChooseWindowName","选择卡池3名单",
			"FileType","nameFile",
			"Limit",ISFILE,
			"Number",3
			},{
			"Name","卡池4名单",
			"ConfigName","namesfile4",
			"FileChooseWindowName","选择卡池4名单",
			"FileType","nameFile",
			"Limit",ISFILE,
			"Number",4
			},{
			"Name","抽背卡池",
			"ConfigName","special",
			"IsEditBox",1,
			"Limit",BETWEENCOUNT,
			"max",4,
			"min",0,
			"OutOfLimitOutPut","输入一个0-4之间的数字（0表示禁用）",
			"Number",11
			},{
			"Name","关闭音乐",
			"ConfigName","off music",
			"IsSwitch",1,
			"Number",12
			},{
			"Name","关闭视频",
			"ConfigName","off video",
			"IsSwitch",1,
			"Number",13
			}
		}},
		{"Title","视频",
		"item",{{
			"Name","单发3星视频",
			"ConfigName","signal 3star video",
			"FileChooseWindowName","选择单发3星视频",
			"FileType","video",
			"Limit",ISFILE,
			"Number",1
			},{
			"Name","单发4星视频",
			"ConfigName","signal 4star video",
			"FileChooseWindowName","选择单发4星视频",
			"FileType","video",
			"Limit",ISFILE,
			"Number",2
			},{
			"Name","单发5星视频",
			"ConfigName","signal 5star video",
			"FileChooseWindowName","选择单发5星视频",
			"FileType","video",
			"Limit",ISFILE,
			"Number",3
			},{
			"Name","十发4星视频",
			"ConfigName","group 4star video",
			"FileChooseWindowName","选择十发4星视频",
			"FileType","video",
			"Limit",ISFILE,
			"Number",5
			},{
			"Name","十发5星视频",
			"ConfigName","group 5star video",
			"FileChooseWindowName","选择十发5星视频",
			"FileType","video",
			"Limit",ISFILE,
			"Number",6
			},{
			"Name","关闭音乐",
			"ConfigName","off music",
			"IsSwitch",1,
			"Number",11
			},{
			"Name","关闭视频",
			"ConfigName","off video",
			"IsSwitch",1,
			"Number",12
			}
		}},
		{"Title","杂项",
		{"item",{{
			"Name","窗口模式",
			"ConfigName","window mode(not full screen)",
			"IsSwitch",1,
			"Number",1,
			},{
			"Name","标题",
			"ConfigName","title name",
			"IsEditBox",1,
			"Number",2
			},{
			"Name","关闭音乐",
			"ConfigName","off music",
			"IsSwitch",1,
			"Number",11
			},{
			"Name","关闭视频",
			"ConfigName","off video",
			"IsSwitch",1,
			"Number",12
			}
		}}
}
	};
	// 写入JSON文件
	std::ofstream file("files\\setting.json");
	if (file.is_open()) {
		file << data.dump(4); // 将JSON数据写入文件，4表示缩进4个空格
		file.close();
		setlog<<setlog.pt()<<"[INFO]" << "JSON data written to 'files\\setting.json'" << std::endl;
	}
	else {
		setlog << setlog.pt() << "[ERROR]" << "Failed to open file for writing" << std::endl;
	}
}
void set2::clicked(int x, int y)
{
	int totalp = pages.size();
	int number = -1;
	if (x >= ui::exitx AND x <= ui::exitxend AND y >= ui::exity AND y <= ui::exityend)quit();
	else if (x >= nextbmx AND x <= nextxend AND y >= nextbmy AND y <= nextyend) {
		number = 0;
		settingpage--;
		if (settingpage < 1) {
			settingpage = totalp;
		}
		changepage();
	}
	else if (x >= lastbmx AND x <= lastxend AND y >= lastbmy AND y <= lastyend) {
		number = 0;
		settingpage++;
		if (settingpage > totalp) {
			settingpage = 1;
		}
		changepage();
	}
	else {
		sNode* current = shead;
		while (current != NULL) {
			if (x >= current->x AND x <= current->xend AND y >= current->y AND y <= current->yend) {
				number = current->number;
				break;
			}
			current = current->next;
		}
	}
	if (number != -1) {
		directshow::music(CLICK);
		if (number > 0) {
			for (const auto& item : pages[settingpage - 1].items) {
				if (number = item.Number) {
					if (item.IsSwitch)
						config::turnUpSideDown(item.ConfigName);
					else if (item.IsFile) {
						ChooseFile(mywindows::hWnd, item);
						if (item.Limit == ISBITMAP) {
							reloadbmp(item);
						}
					}
				}
			}
		}
	}
}
void set2::reloadbmp(sitem item)
{
	DeleteObject(hbitmaps[item.BitmapNumber]);
	hbitmaps[item.BitmapNumber] =
		(HBITMAP)LoadImage(NULL, config::get(item.ConfigName).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hbitmaps[item.BitmapNumber], sizeof(BITMAP), bitmaps[item.BitmapNumber]);
}
void set2::rereadconfig() {
	offmusic = config::getint(OFFMUSIC);
	fullscreen = !config::getint(INWINDOW);
}
void set2::changepage()
{
	char n = 0;
	while (textboxhwnd[n] != 0) {
		DestroyWindow(textboxhwnd[n]);
		n++;
	}
	for (n = 0; n < 40; n++)isused[n] = 0;
	firstpaint = 1;
}
void set2::showitem(sitem item, HDC hdc, HDC hdcMem) {
	if (item.IsSwitch) {
		switchbm(item, hdc, hdcMem);
	}
	else if (item.IsEditBox) {
		textbox(item, hdc, hdcMem);
	}
}
void set2::seteditbox(LPARAM lParam, WPARAM wParam)
{
	// 获取文本框的句柄,确保它是有效的
	HWND editBoxHwnd = (HWND)(lParam);
	int numberoftextbox = LOWORD(wParam);
	if (editBoxHwnd != NULL) {
		// 分配缓冲区大小,这里假设文本框中的文本不会超过256个字符
		wchar_t sz[256];
		Edit_GetText(editBoxHwnd, sz, 256);
		// 显示文本框中的文本
		if (wcslen(sz) != 0) {
			std::wstring tmp(sz);
			for (const auto& item : pages[settingpage].items) {
				if (item.Number == numberoftextbox) editboxeditor(item,tmp);
				else continue;
			}
		}
	}
}
void set2::textbox(sitem item, HDC hdc, HDC hdcMem)
{
	SelectObject(hdc, ui::text_mid);
	SetBkColor(hdc, RGB(236, 229, 216));
	SelectObject(hdcMem, hbitmaps[setbutton]);
	TextOut_(hdc, sxy[item.Number].x, sxy[item.Number].y + mywindows::windowHeight * 0.01, item.Name.c_str());
	int number = item.Number;
	std::wstring wst = config::get(item.ConfigName).c_str();
	if (isused[number] == 0) {
		textboxhwnd[number] = CreateEditBox(mywindows::hWnd, number, sxy[number].bmx, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, wst.c_str());
	}
	if (item.IsFile) {
		if (!isused[number + 20]) {
			StretchBlt(hdc, sxy[number].bmxend, sxy[number].bmy, sxy[number].bmw, sxy[number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
			TextOut_(hdc, sxy[number].bmxend + mywindows::windowWidth * 0.02, sxy[number].y + mywindows::windowHeight * 0.01, L"选择文件");

			int x = sxy[number].bmxend, y = sxy[number].bmy, xend = sxy[number].bmxend + sxy[number].bmw, yend = sxy[number].bmh + sxy[number].bmy;
			if (!isused[number]) {
				sNode* newnode = new sNode;
				if (newnode == nullptr) {
					mywindows::errlog(L"Memory allocation error(textbox)");
				}
				newnode->x = x;
				newnode->xend = xend;
				newnode->y = y;
				newnode->yend = yend;
				newnode->next = shead;
				newnode->number = number;
				shead = newnode;
			}
		}
	}
	isused[number] = 1;
}
void set2::editboxeditor(sitem item, wstring tmp)
{
	switch (item.Limit)
	{
	case S_WINDOWTITLE:
		SetWindowTextW(mywindows::hWnd, tmp.c_str());
		break;
	case BETWEENCOUNT: {
		int value = std::stoi(tmp);
		if (value < item.min || value > item.max) {
			MessageBoxW(NULL, item.OutOfLimitOutPut.c_str(), L"错误", MB_ICONERROR);
			return;
		}
	}break;
	case ISFILE:
	case ISBITMAP:
		if (!std::filesystem::exists(tmp))return;
		break;
	default:
		break;
	}
	config::replace(item.ConfigName, tmp);
	return;
}
set2::~set2() {
	return;
}
void set2::switchbm(sitem item, HDC hdc, HDC hdcMem) {
	SelectObject(hdc, ui::text_mid);
	SetBkColor(hdc, RGB(236, 229, 216));
	SelectObject(hdcMem, hbitmaps[setbutton]);
	StretchBlt(hdc, sxy[item.Number].bmx, sxy[item.Number].bmy, sxy[item.Number].bmw, sxy[item.Number].bmh, hdcMem, 0, 0, setbu.bmWidth, setbu.bmHeight, SRCCOPY);
	TextOut_(hdc, sxy[item.Number].x, sxy[item.Number].y + mywindows::windowHeight * 0.01, item.Name.c_str());
	if (std::stoi(config::get(item.ConfigName)) == 1)
		TextOut_(hdc, sxy[item.Number].bmx + mywindows::windowWidth * 0.04, sxy[item.Number].bmy + mywindows::windowHeight * 0.01, L"开");
	else
		TextOut_(hdc, sxy[item.Number].bmx + mywindows::windowWidth * 0.04, sxy[item.Number].bmy + mywindows::windowHeight * 0.01, L"关");
	int x = sxy[item.Number].bmx, y = sxy[item.Number].bmy, xend = sxy[item.Number].bmxend, yend = sxy[item.Number].bmh + sxy[item.Number].bmy;
	sNode* newnode = new sNode;
	if (newnode == NULL) {
		mywindows::errlog("Memory allocation error(textbox)");
	};
	newnode->x = x;
	newnode->xend = xend;
	newnode->y = y;
	newnode->yend = yend;
	newnode->next = shead;
	newnode->number = item.Number;
	shead = newnode;
}
void set2::resetplace() {
	mywindows::log(L"Free Setting List complete");
	for (char i = 1; i <= 10; i++) {
		sxy[i].x = mywindows::windowWidth * 0.05;
		sxy[i].y = mywindows::windowHeight * (i) * 0.09;
		sxy[i].bmx = mywindows::windowWidth * 0.25;
		sxy[i].bmxend = mywindows::windowWidth * 0.35;
		sxy[i].bmy = sxy[i].y;
		sxy[i].bmyend = sxy[i].y + mywindows::windowHeight * 0.07;
		sxy[i].bmw = mywindows::windowWidth * 0.1;
		sxy[i].bmh = mywindows::windowWidth * 0.03;
	}
	for (char i = 11; i <= 19; i++) {
		sxy[i].x = mywindows::windowWidth * 0.55;
		sxy[i].y = mywindows::windowHeight * (i - 9) * 0.09;
		sxy[i].bmx = mywindows::windowWidth * 0.75;
		sxy[i].bmxend = mywindows::windowWidth * 0.85;
		sxy[i].bmy = sxy[i].y;
		sxy[i].bmyend = sxy[i].y + mywindows::windowHeight * 0.07;
		sxy[i].bmw = mywindows::windowWidth * 0.1;
		sxy[i].bmh = mywindows::windowWidth * 0.03;
	}
}
void set2::ChooseFile(HWND hwnd, sitem item)
{
loop:
	OPENFILENAMEW ofn = { 0 };
	wchar_t strFilename[MAX_PATH] = { 0 }; // 用于接收文件名

	ofn.lStructSize = sizeof(OPENFILENAMEW); // 结构体大小
	ofn.hwndOwner = mywindows::hWnd; // 拥有者窗口句柄,为NULL表示对话框是非模态的,实际应用中一般都要有这个句柄
	ofn.lpstrFilter = item.FileType.c_str(); // 设置过滤
	ofn.nFilterIndex = 1; // 过滤器索引
	ofn.lpstrFile = strFilename; // 接收返回的文件名,注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFilename); // 缓冲区长度
	ofn.lpstrInitialDir = NULL; // 初始目录为默认
	ofn.lpstrTitle = item.FileChooseWindowName.c_str(); // 使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // 文件、目录必须存在,隐藏只读选项

	if (GetOpenFileNameW(&ofn))
	{
		std::wstring filename(strFilename);
		config::replace(item.ConfigName, filename);
		Edit_SetText(textboxhwnd[item.Number], filename.c_str());
	}
	else
	{
		MessageBoxW(NULL, L"请选择一个文件", NULL, MB_ICONERROR);
		goto loop;
	}
}
void set2::quit() {
	directshow::music(ENTER);
	char n = 0;
	while (n <= 0) {
		if (textboxhwnd[n] != NULL)
			DestroyWindow(textboxhwnd[n]);
		n++;
	}
	ui::screenmode = FIRST_MENU;
	ui::ScreenModeChanged = 1;
	for (n = 0; n < 40; n++)isused[n] = 0;
	firstpaint = 1;
}
void set2::repaint()
{
	firstpaint = 1;
	InvalidateRect(mywindows::hWnd, NULL, FALSE);
}
HWND set2::CreateEditBox(HWND hWndParent, int NUMBER, int x, int y, int w, int h, const wchar_t* words) {
	// 创建EDIT控件的样式
	DWORD editStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL;

	// 创建EDIT控件的附加样式（可选）
	DWORD editExStyle = WS_EX_CLIENTEDGE;

	// 创建文本框
	HWND hEdit = CreateWindowExW(editExStyle, L"EDIT", words, editStyle, x, y, w, h, hWndParent, (HMENU)NUMBER, NULL, NULL);

	// 返回文本框句柄
	return hEdit;
}