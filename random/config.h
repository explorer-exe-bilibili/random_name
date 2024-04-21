#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_NON_CONFORMING_WCSTOK
#pragma once
#include<string>
#define CONFIG_ L"\\files\\config.txt"
#define YUANSHI L"Diamond"
#define NAMES1 L"namesfile1"
#define NAMES2 L"namesfile2"
#define NAMES3 L"namesfile3"
#define NAMES4 L"namesfile4"
#define BLUE_BALL_COUNT L"buleball"
#define PINK_BALL_COUNT L"redball"
#define OFF_VIDEO L"off video"
#define WINDOW_TITEL L"title name"
#define MODE L"firstmode"
#define INWINDOW L"window mode(not full screen)"
#define OVER4 L"over4"
#define OVER3 L"over3"
#define OVER2 L"over2"
#define OVER1 L"over1"
#define SPECIAL L"special"
#define OFFMUSIC L"off music"
#define SIGNALSTAR3 L"signal 3star video"
#define SIGNALSTAR4 L"signal 4star video"
#define SIGNALSTAR5 L"signal 5star video"
#define GROUPSTAR4 L"group 4star video"
#define GROUPSTAR5 L"group 5star video"


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
	struct Node* next;
	} Node;
	// 定义链表头节点
	static Node* head;
	static const wchar_t* LogString;
	static std::wstring configpath;
public:
	static void add(const std::wstring& name, const std::wstring& value);
	  static void readFile();
	  static void saveFile();
	  static void cleanup();
	  static void deleteItem(const std::wstring& name);
	  static void replace(const std::wstring& option, const std::wstring& value);
	  static void printAllConfigItems();
	  static void init();
	  static std::wstring get(const std::wstring& name);
};

