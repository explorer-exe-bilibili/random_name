#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_NON_CONFORMING_WCSTOK
#pragma once
#include<string>
#define CONFIG_ L"\\files\\config.txt"

class config
{
private:
	// 定义配置项结构
	typedef struct ConfigItem {
		std::wstring name;
		std::wstring value;
	} ConfigItem;
	// 定义链表节点
	typedef struct Node {
	ConfigItem item;
	struct Node* next=nullptr;
	} Node;
	// 定义链表头节点
	static Node* head;
	static const wchar_t* LogString;
	static std::wstring configpath;
public:
	static int turnUpSideDown(const std::wstring& name);
	static void add(const std::wstring& name, const std::wstring& value);
	static void add(const std::wstring& name, const int value);
	static void readFile();
	static void saveFile();
	static void cleanup();
	static void deleteItem(const std::wstring& name);
	static void replace(const std::wstring& name, const std::wstring& value);
	static void replace(const std::wstring& name, const int value);
	static void printAllConfigItems();
	static int init();
	static std::wstring get(const std::wstring& name);
	static std::wstring getpath(const std::wstring& name);
	static double getd(const std::wstring& name);
	static int getint(const std::wstring& name);
	static long long getscreen(const std::wstring& name);
	static long long getwindow(const std::wstring& name);
};

