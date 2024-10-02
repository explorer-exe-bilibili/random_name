#pragma once
#include<string>
#define CONFIG_ L"\\files\\config.txt"

namespace config
{
	// 定义配置项结构
	typedef struct ConfigItem {
		std::wstring name;
		std::wstring value;
	} ConfigItem;
	// 定义链表节点
	typedef struct Node {
	ConfigItem item;
	Node* next=nullptr;
	} Node;
	int turnUpSideDown(const std::wstring& name);
	void add(const std::wstring& name, const std::wstring& value);
	void add(const std::wstring& name, int value);
	void readFile();
	void saveFile();
	void cleanup();
	void deleteItem(const std::wstring& name);
	void replace(const std::wstring& name, const std::wstring& value);
	void replace(const std::wstring& name, int value);
	void printAllConfigItems();
	int init();
	std::wstring get(const std::wstring& name);
	std::wstring getpath(const std::wstring& name);
	double getd(const std::wstring& name);
	int getint(const std::wstring& name);
	long long getScreen(const std::wstring& name);
	long long getWindow(const std::wstring& name);
};

