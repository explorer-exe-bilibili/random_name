#include "config.h"
#include <stdio.h>
#include<string>
#include"mywindows.h"
#include <codecvt>

// 定义链表头节点
config::Node* config::head = NULL;
const wchar_t* config::LogString;
std::wstring config::configpath;
using namespace std;

//读取和补全配置项
void config::init() {
	mywindows::log("initing config start");
	configpath = Log::wrunpath;
	configpath += CONFIG_;
	FILE* file = _wfopen(configpath.c_str(), L"r");
	if (file == NULL) {
		add(NAMES1, L"\\names1.txt");
		add(NAMES2, L"\\names2.txt");
		add(NAMES3, L"\\names3.txt");
		add(NAMES4, L"\\names4.txt");
		add(YUANSHI, L"10000");
		add(BLUE_BALL_COUNT, L"10000");
		add(PINK_BALL_COUNT, L"10000");
		add(OFF_VIDEO, L"0");
		add(SPECIAL, L"0");
		add(MODE, L"1");
		add(INWINDOW, L"0");
		add(OFFMUSIC, L"0");
		add(FLOATWINDOW, L"1");
		add(WINDOW_TITEL, L"祈愿");
		add(OVER4, L"\\files\\imgs\\over4.bmp");
		add(OVER3,L"\\files\\imgs\\over3.bmp");
		add(OVER2, L"\\files\\imgs\\over2.bmp");
		add(OVER1, L"\\files\\imgs\\over1.bmp");
		add(SIGNALSTAR3, L"\\files\\video\\3star-single.mp4");
		add(SIGNALSTAR4, L"\\files\\video\\4star-single.mp4");
		add(SIGNALSTAR5, L"\\files\\video\\5star-single.mp4");
		add(GROUPSTAR4, L"\\files\\video\\4star-multi.mp4");
		add(GROUPSTAR5, L"\\files\\video\\5star-multi.mp4");
		saveFile();
		readFile();
		printAllConfigItems();
	}
	else
	{
		fclose(file);
		readFile();
		wstring LogString = get(MODE);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(MODE, L"1");
		LogString = get(OFF_VIDEO);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(OFF_VIDEO, L"0");
		LogString = get(WINDOW_TITEL);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(WINDOW_TITEL, L"原神");
		LogString = get(NAMES1);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(NAMES1, L"\\names1.txt");
		LogString = get(NAMES2);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(NAMES2, L"\\names2.txt");
		LogString = get(NAMES3);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(NAMES3, L"\\names3.txt");
		LogString = get(NAMES4);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(NAMES4, L"\\names4.txt");
		LogString = get(BLUE_BALL_COUNT);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(BLUE_BALL_COUNT, L"10000");
		LogString = get(PINK_BALL_COUNT);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(PINK_BALL_COUNT, L"10000");
		LogString = get(YUANSHI);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(YUANSHI, L"10000");
		LogString = get(SPECIAL);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(SPECIAL, L"0");
		LogString = get(OFFMUSIC);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(OFFMUSIC, L"0");
		LogString = get(INWINDOW);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(INWINDOW, L"0");
		LogString = get(FLOATWINDOW);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(FLOATWINDOW, L"1");
		LogString = get(OVER1);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(OVER1, L"\\files\\imgs\\over1.bmp");
		LogString = get(OVER2);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(OVER2, L"\\files\\imgs\\over2.bmp");
		LogString = get(OVER3);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(OVER3, L"\\files\\imgs\\over3.bmp");
		LogString = get(OVER4);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(OVER4, L"\\files\\imgs\\over4.bmp");
		LogString = get(SIGNALSTAR3);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(SIGNALSTAR3, L"\\files\\video\\3star-single.mp4");
		LogString = get(SIGNALSTAR4);
		if (wcscmp(LogString.c_str(), L"err") == 0) add(SIGNALSTAR4, L"\\files\\video\\4star-single.mp4");
		LogString = get(SIGNALSTAR5);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(SIGNALSTAR5, L"\\files\\video\\5star-single.mp4");
		LogString = get(GROUPSTAR4);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(GROUPSTAR4, L"\\files\\video\\4star-multi.mp4");
		LogString = get(GROUPSTAR5);
		if (wcscmp(LogString.c_str(), L"err") == 0)add(GROUPSTAR5, L"\\files\\video\\5star-multi.mp4");
		saveFile();
		printAllConfigItems();
		mywindows::log(L"config init successfully");
	}
}
//获取配置项内容
std::wstring config::get(const std::wstring& name) {
	Node* current = head;

	// 遍历链表查找匹配的配置项
	while (current != NULL) {
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
int config::getint(const std::wstring& name)
{
	Node* current = head;

	// 遍历链表查找匹配的配置项
	while (current != NULL) {
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
// 读取配置文件并保存配置项到链表
void config::readFile() {
	FILE* file = _wfopen(configpath.c_str(), L"r, ccs=UNICODE");
	if (file == NULL) {
		mywindows::errlog(L"Error opening file for reading");
		return;
	}

	wchar_t line[256];
	while (fgetws(line, sizeof(line) / sizeof(wchar_t), file) != NULL) {
		wchar_t* trimLine = wcstok(line, L"\n"); // 移除换行符

		wchar_t* currentOption = wcstok(trimLine, L"=");
		if (currentOption != NULL) {
			wchar_t* value = wcstok(NULL, L"=");
			// 添加配置项到链表
			add(std::wstring(currentOption), std::wstring(value));
			mywindows::log(trimLine);
		}
	}

	fclose(file);
}
// 保存配置项到配置文件
void config::saveFile() {
	FILE* file = _wfopen(configpath.c_str(), L"w, ccs=UNICODE");
	if (file == NULL) {
		mywindows::errlog(L"Error opening file for writing");
		return;
	}

	Node* current = head;
	while (current != NULL) {
		fwprintf(file, L"%s=%s\n", current->item.name.c_str(), current->item.value.c_str());
		current = current->next;
	}

	fclose(file);
}
// 释放链表节点的内存
void config::cleanup() {
	Node* current = head;
	Node* next;

	while (current != NULL) {
		next = current->next;
		// 不需要手动释放 std::wstring 对象的内存
		delete current;
		current = next;
	}

	head = NULL;
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
//打印配置
void config::printAllConfigItems() {
	mywindows::log(L" Configuration:");
	Node* current = head;
	while (current != NULL) {
		mywindows::log(L"%s: %s", current->item.name.c_str(), current->item.value.c_str());
		current = current->next;
	}
}