#include "config.h"
#include <stdio.h>
#include<string>
#include"mywindows.h"
#include <codecvt>
#include"sth2sth.h"

// 定义链表头节点
config::Node* config::head = nullptr;
const wchar_t* config::LogString;
std::wstring config::configpath;
using namespace std;

//读取和补全配置项
void config::init() {
	mywindows::log("initing config start");
	configpath = Log::wrunpath;
	configpath += CONFIG_;
	FILE* file = _wfopen(configpath.c_str(), L"r");
	if (file == nullptr) {
		add(NAMES1, L"\\names1.txt");
		add(NAMES2, L"\\names2.txt");
		add(NAMES3, L"\\names3.txt");
		add(NAMES4, L"\\names4.txt");
		add(YUANSHI, L"10000");
		add(BLUE_BALL_COUNT, L"10000");
		add(PINK_BALL_COUNT, L"10000");
		add(FLOATX, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.8)));
		add(FLOATY, sth2sth::str2wstr(to_string(mywindows::screenHeight * 0.8)));
		add(FLOATW, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.05)));
		add(FLOATH, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.05)));
		add(OFF_VIDEO, L"0");
		add(SPECIAL, L"0");
		add(MODE, L"1");
		add(MU, L"0.9");
		add(TEXTR, L"0");
		add(TEXTG, L"125");
		add(TEXTB, L"125");
		add(FPS, L"60");
		add(UNSUITFONT, L"0");
		add(DEBUG, L"0");
		add(INWINDOW, L"0");
		add(OFFMUSIC, L"0");
		add(FLOATWINDOW, L"1");
		add(TYPICAL, L"0");
		add(WINDOW_TITEL, L"祈愿");
		add(OVER4, L"\\files\\imgs\\over4.jpg");
		add(OVER3,L"\\files\\imgs\\over3.jpg");
		add(OVER2, L"\\files\\imgs\\over2.jpg");
		add(OVER1, L"\\files\\imgs\\over1.jpg");
		add(FLOATPHOTO, L"\\files\\imgs\\float.jpg");
		add(SIGNALSTAR3, L"\\files\\video\\3star-single.mp4");
		add(SIGNALSTAR4, L"\\files\\video\\4star-single.mp4");
		add(SIGNALSTAR5, L"\\files\\video\\5star-single.mp4");
		add(GROUPSTAR4, L"\\files\\video\\4star-multi.mp4");
		add(GROUPSTAR5, L"\\files\\video\\5star-multi.mp4");
		add(POOL_COUNT, L"4");
		add(IMAGE_DIRECTORY, L"\\files\\imgs\\");
		saveFile();
		readFile();
		printAllConfigItems();
	}
	else
	{
		fclose(file);
		readFile();
		wstring LogString = get(MODE);
		if (LogString==L"err")add(MODE, L"1");
		LogString = get(OFF_VIDEO);
		if (LogString==L"err")add(OFF_VIDEO, L"0");
		LogString = get(WINDOW_TITEL);
		if (LogString==L"err")add(WINDOW_TITEL, L"原神");
		LogString = get(NAMES1);
		if (LogString==L"err")add(NAMES1, L"\\names1.txt");
		LogString = get(NAMES2);
		if (LogString==L"err")add(NAMES2, L"\\names2.txt");
		LogString = get(NAMES3);
		if (LogString==L"err")add(NAMES3, L"\\names3.txt");
		LogString = get(NAMES4);
		if (LogString==L"err")add(NAMES4, L"\\names4.txt");
		LogString = get(BLUE_BALL_COUNT);
		if (LogString==L"err")add(BLUE_BALL_COUNT, L"10000");
		LogString = get(PINK_BALL_COUNT);
		if (LogString==L"err")add(PINK_BALL_COUNT, L"10000");
		LogString = get(YUANSHI);
		if (LogString==L"err")add(YUANSHI, L"10000");
		LogString = get(SPECIAL);
		if (LogString==L"err")add(SPECIAL, L"0");
		LogString = get(OFFMUSIC);
		if (LogString==L"err")add(OFFMUSIC, L"0");
		LogString = get(INWINDOW);
		if (LogString==L"err")add(INWINDOW, L"0");
		LogString = get(FLOATWINDOW);
		if (LogString == L"err")add(FLOATWINDOW, L"1");
		LogString = get(UNSUITFONT);
		if (LogString == L"err")add(UNSUITFONT, L"0");
		LogString = get(DEBUG);
		if (LogString == L"err")add(DEBUG, L"0");
		LogString = get(TYPICAL);
		if (LogString == L"err")add(TYPICAL, L"0");
		LogString = get(MU);
		if (LogString == L"err")add(MU, L"0.9");
		LogString = get(TEXTR);
		if (LogString == L"err")add(TEXTR, L"0");
		LogString = get(TEXTG);
		if (LogString == L"err")add(TEXTG, L"125");
		LogString = get(TEXTB);
		if (LogString == L"err")add(TEXTB, L"125");
		LogString = get(FPS);
		if (LogString == L"err")add(FPS, L"60");
		LogString = get(FLOATX);
		if (LogString==L"err")add(FLOATX, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.8)));
		LogString = get(FLOATY);
		if (LogString==L"err")add(FLOATY, sth2sth::str2wstr(to_string(mywindows::screenHeight * 0.8)));
		LogString = get(FLOATW);
		if (LogString==L"err")add(FLOATW, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.05)));
		LogString = get(FLOATH);
		if (LogString==L"err")add(FLOATH, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.05)));
		LogString = get(OVER1);
		if (LogString==L"err")add(OVER1, L"\\files\\imgs\\over1.jpg");
		LogString = get(OVER2);
		if (LogString==L"err")add(OVER2, L"\\files\\imgs\\over2.jpg");
		LogString = get(OVER3);
		if (LogString==L"err")add(OVER3, L"\\files\\imgs\\over3.jpg");
		LogString = get(OVER4);
		if (LogString==L"err")add(OVER4, L"\\files\\imgs\\over4.jpg");
		LogString = get(FLOATPHOTO);
		if (LogString==L"err")add(FLOATPHOTO, L"\\files\\imgs\\float.jpg");
		LogString = get(SIGNALSTAR3);
		if (LogString==L"err")add(SIGNALSTAR3, L"\\files\\video\\3star-single.mp4");
		LogString = get(SIGNALSTAR4);
		if (LogString==L"err") add(SIGNALSTAR4, L"\\files\\video\\4star-single.mp4");
		LogString = get(SIGNALSTAR5);
		if (LogString==L"err")add(SIGNALSTAR5, L"\\files\\video\\5star-single.mp4");
		LogString = get(GROUPSTAR4);
		if (LogString==L"err")add(GROUPSTAR4, L"\\files\\video\\4star-multi.mp4");
		LogString = get(GROUPSTAR5);
		if (LogString==L"err")add(GROUPSTAR5, L"\\files\\video\\5star-multi.mp4");
		LogString = get(POOL_COUNT);
		if (LogString==L"err")add(POOL_COUNT, L"4");
		LogString = get(IMAGE_DIRECTORY);
		if (LogString==L"err")add(IMAGE_DIRECTORY, L"\\files\\imgs\\");
		saveFile();
		printAllConfigItems();
		mywindows::log(L"config init successfully");
	}
}
//获取配置项内容
std::wstring config::get(const std::wstring& name) {
	Node* current = head;

	// 遍历链表查找匹配的配置项
	while (current != nullptr) {
		if (current->item.name == name) {
			// 找到匹配的配置项,返回其参数值
			return current->item.value;
		}
		current = current->next;
	}

	// 如果没有找到匹配的配置项,返回L"err"
	return L"err";
}
std::wstring config::getpath(const std::wstring& name)
{
	wstring path = get(name);
	if (path.find_first_of(L"\\") == 0) {
		path = Log::wrunpath + path;
	}
	return path;
}
double config::getd(const std::wstring& name)
{
	Node* current = head;

	// 遍历链表查找匹配的配置项
	while (current != nullptr) {
		if (current->item.name == name) {
			// 找到匹配的配置项,返回其参数值
			try
			{
				return stod(current->item.value);
			}
			catch (const std::exception& e)
			{
				Log l("files\\log\\configERR.log");
				l << l.pt() << "[ERROR]Meet a error when get int config item: " << e.what();
				mywindows::errlog("Meet a error ,you can saw it in configERR.log");
				return 0;
			}
		}
		current = current->next;
	}

	// 如果没有找到匹配的配置项,返回L"err"
	return -1;
}
int config::getint(const std::wstring& name)
{
	Node* current = head;

	// 遍历链表查找匹配的配置项
	while (current != nullptr) {
		if (current->item.name == name) {
			// 找到匹配的配置项,返回其参数值
			try
			{
				return stoi(current->item.value);
			}
			catch (const std::exception& e)
			{
				Log l("files\\log\\configERR.log");
				l << l.pt() << "[ERROR]Meet a error when get int config item: " << e.what();
				mywindows::errlog("Meet a error ,you can saw it in configERR.log");
				return 0;
			}
		}
		current = current->next;
	}

	// 如果没有找到匹配的配置项,返回L"err"
	return -1;
}
bool endsWith(const std::wstring& str, const std::wstring& suffix) {
	if (suffix.length() > str.length()) {
		return false;
	}
	return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}
long long config::getscreen(const std::wstring& name)
{
	wstring strv = get(name);
	long double douv = stold(strv);
	if (douv> 1) {
		long long llv = douv;
		return llv;
	}
	else {
		if (name.back()=='x' || endsWith(name,L"width")) {
			long long llv = douv * mywindows::screenWidth;
			return llv;
		}
		else if (name.back()=='y' || endsWith(name,L"height")) {
			long long llv = douv * mywindows::screenHeight;
			return llv;
		}
	}
}
long long config::getwindow(const std::wstring& name)
{
	wstring strv = get(name);
	long double douv = stold(strv);
	if (douv> 1) {
		long long llv = douv;
		return llv;
	}
	else {
		if (name.back()=='x' || endsWith(name,L"width")) {
			long long llv = douv * mywindows::WW;
			return llv;
		}
		else if (name.back()=='y' || endsWith(name,L"height")) {
			long long llv = douv * mywindows::WH;
			return llv;
		}
	}
}
int config::turnUpSideDown(const std::wstring& name)
{
	bool t = !getint(name);
	replace(name, to_wstring(t));
	return t;
}
// 添加配置项到链表
void config::add(const std::wstring& name, const std::wstring& value) {
	Node* current = head;
	Node* prev = nullptr;

	while (current != nullptr) {
		if (current->item.name == name) {
			current->item.value = value;
			return;
		}
		prev = current;
		current = current->next;
	}

	ConfigItem newItem;
	newItem.name = name;
	newItem.value = value;

	Node* newNode = new Node;
	if (newNode == nullptr) {
		mywindows::errlog(L"Memory allocation error(add)");
		return;
	}

	newNode->item = newItem;
	newNode->next = head;
	head = newNode;
}
void config::add(const std::wstring& name, const int value)
{
	Node* current = head;
	Node* prev = nullptr;

	while (current != nullptr) {
		if (current->item.name == name) {
			current->item.value = to_wstring(value);
			return;
		}
		prev = current;
		current = current->next;
	}

	ConfigItem newItem;
	newItem.name = name;
	newItem.value = value;

	Node* newNode = new Node;
	if (newNode == nullptr) {
		mywindows::errlog(L"Memory allocation error(add)");
		return;
	}

	newNode->item = newItem;
	newNode->next = head;
	head = newNode;
}

// 读取配置文件并保存配置项到链表
void config::readFile() {
	FILE* file = _wfopen(configpath.c_str(), L"r, ccs=UNICODE");
	if (file == nullptr) {
		mywindows::errlog(L"Error opening file for reading");
		return;
	}

	wchar_t line[256];
	while (fgetws(line, sizeof(line) / sizeof(wchar_t), file) != nullptr) {
		wchar_t* trimLine = wcstok(line, L"\n"); // 移除换行符

		wchar_t* currentOption = wcstok(trimLine, L"=");
		if (currentOption != nullptr) {
				wchar_t* value = wcstok(nullptr, L"=");
				if (value != NULL) {
					// 添加配置项到链表
					add(std::wstring(currentOption), std::wstring(value));
				}
				else
				{
					wstring err = L"读取配置时遇到错误，配置项的名称是： "+wstring(currentOption)+L"请按是检查配置文件，或者按否删除配置文件以恢复默认设置修复错误，或者按取消关闭程序";
					int i=MessageBox(NULL, err.c_str(), L"严重错误", MB_ICONERROR | MB_YESNOCANCEL);
					switch(i)
					{
					case IDYES:
						ShellExecute(NULL, L"open", configpath.c_str(), NULL, NULL, SW_SHOWNORMAL);
						exit(-1);
						break;
					case IDNO:
						fclose(file);
						_wremove(configpath.c_str());
						mywindows::reboot();
						break;
					case IDCANCEL:
						exit(-1);
						break;
					}
				}
		}
	}

	fclose(file);
}



// 保存配置项到配置文件
void config::saveFile() {
	FILE* file = _wfopen(configpath.c_str(), L"w, ccs=UNICODE");
	if (file == nullptr) {
		mywindows::errlog(L"Error opening file for writing");
		return;
	}

	Node* current = head;
	while (current != nullptr) {
		fwprintf(file, L"%s=%s\n", current->item.name.c_str(), current->item.value.c_str());
		current = current->next;
	}

	fclose(file);
}
// 释放链表节点的内存
void config::cleanup() {
	Node* current = head;
	Node* next;

	while (current != nullptr) {
		next = current->next;
		// 不需要手动释放 std::wstring 对象的内存
		delete current;
		current = next;
	}

	head = nullptr;
	mywindows::log(L"Free ConfigList complete");
}
// 删除配置项
void config::deleteItem(const std::wstring& name) {
	Node* prev = nullptr;
	Node* current = head;

	while (current != nullptr) {
		if (current->item.name == name) {
			// 找到匹配的配置项,删除它
			if (prev != nullptr) {
				prev->next = current->next;
			}
			else {
				head = current->next;
			}

			delete current;
			mywindows::log(L"del %s success", name.c_str());
			return;
		}

		prev = current;
		current = current->next;
	}

	mywindows::log(L"del %s failed, not found", name.c_str());
}
//替换配置项
void config::replace(const std::wstring& name, const std::wstring& value) {
	Node* prev = nullptr;
	Node* current = head;

	while (current != nullptr) {
		if (current->item.name == name) {
			current->item.value = value;
			saveFile();
			return;
		}

		prev = current;
		current = current->next;
	}

	// 如果没有找到匹配的配置项,则添加新的配置项
	add(name, value);
	saveFile();
}
void config::replace(const std::wstring& name, const int value)
{
	Node* prev = nullptr;
	Node* current = head;

	while (current != nullptr) {
		if (current->item.name == name) {
			current->item.value = sth2sth::str2wstr(to_string(value));
			saveFile();
			return;
		}

		prev = current;
		current = current->next;
	}

	// 如果没有找到匹配的配置项,则添加新的配置项
	add(name, value);
	saveFile();
}
//打印配置
void config::printAllConfigItems() {
	mywindows::log(L" Configuration:");
	Node* current = head;
	while (current != nullptr) {
		mywindows::log(L"%s: %s", current->item.name.c_str(), current->item.value.c_str());
		current = current->next;
	}
}