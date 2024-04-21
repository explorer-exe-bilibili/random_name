#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<string>
#define CONFIG_ "\\files\\config.txt"
#define YUANSHI "Diamond"
#define NAMES "namesfile"
#define NAMES1 "namesfile1"
#define NAMES2 "namesfile2"
#define NAMES3 "namesfile3"
#define NAMES4 "namesfile4"
#define BLUE_BALL_COUNT "buleball"
#define PINK_BALL_COUNT "redball"
#define OFF_VIDEO "off video"
#define WINDOW_TITEL "title name"
#define MODE "first mode"
#define INWINDOW "window mode(not full screen)"
#define OVER4 "over4"
#define OVER3 "over3"
#define OVER2 "over2"
#define OVER1 "over1"
#define SPECIAL "special"
#define OFFMUSIC "off music"
// 定义配置项结构
typedef struct ConfigItem;
// 定义链表节点
typedef struct Node;
class config
{
private:
	// 定义链表头节点
	static Node* head;
	static const char* LogString;
	static std::string configpath;
	static const char* CONFIG;
public:
	static void add(const char* name, const char* value);
	static void readFile();
	static void saveFile();
	static void cleanup();
	static void deleteItem(const char* name);
	static void replace(const char* option, const char* value);
	static void printAllConfigItems();
	static void init();
	static const char* get(const char* name);
};

