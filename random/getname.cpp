﻿#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "getname.h"

#include <codecvt>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <random>

#include<uchardet/uchardet.h>

#include"mywindows.h"
#include"config.h"
#include"configitem.h"
#include"sth2sth.h"

using namespace std;

getname* getname::instance = nullptr;
std::string detectEncoding(const std::wstring& filename);

getname::getname():fileerr(false)
{
	for(int i=0;i<config::getint(POOL_COUNT);i++)
	{
		ReRandom(i);
	}
}
getname::~getname()
{
	for(auto& b:items)
	{
		for(auto& i:b)
		{
			i = {};
		}
	}
	if (instance)delete instance;
	instance = nullptr;
}

bool getname::random(const int m, const int i) {
	std::string tmp1;
	wstring path;
	if (m == 0) path = config::getpath(NAMES1);
	else if (m == 1) path = config::getpath(NAMES2);
	else if (m == 2) path = config::getpath(NAMES3);
	else if (m == 3) path = config::getpath(NAMES4);
	tmp1 = RandomLineFromFile(path);
	if (tmp1 == "FOF")return 0;
	int attrib = getattrib(tmp1);
	if(config::getint(TYPICAL)==(m+1))
	{
		items[m][i].star = random_star();
	}
	else {
		items[m][i].star = attrib % 10;
	}
	items[m][i].type = attrib % 100 / 10;
	items[m][i].name = sth2sth::UTF8To16(Make(tmp1).c_str());
	return 1;
}
int getname::getattrib(const std::string& input) {
	// 找到第一个 "-"
	std::cerr << input << "\n";
	size_t dashPos = input.find("-");
	std::cerr << input << "\n";
	if (dashPos != std::string::npos) {
		// 如果找到了 "-", 获取 "-" 后面的数字
		std::string numberStr = input.substr(dashPos + 1);
		// 将数字字符串转换为整数
		try
		{
			return std::stoi(numberStr);
		}
		catch (const std::exception& e)
		{
			Log err("files\\log\\RandomERR.log");
			err<<err.pt() <<"[ERROR]Meet a error when get star:" << e.what()<<"the string is :"<<numberStr << endl;
		}
	}
	// 如果未找到 "-" 或转换失败，返回默认值（可以根据需要修改）
	return 3;
}
std::string getname::Make(const std::string& input) {
	std::wstring trimmed = sth2sth::str2wstru(input);
	//如果存在，删去utf-8的bom
	if (trimmed[0] == 65279)
	{
		trimmed = trimmed.substr(1);
	}

	// 找到第一个 "-"
	size_t dashPos = trimmed.find(L"-");
	if (dashPos != std::string::npos && dashPos > 1) {
		// 如果找到了 "-", 裁切掉 "-" 后面的内容
		return sth2sth::wstr2stru(trimmed.substr(0, dashPos));
	}
	else {
		// 如果未找到 "-", 直接返回原字符串
		return sth2sth::wstr2stru(trimmed);
	}
}
//std::string getname::RandomLineFromFile(const std::wstring& filename)
//{
//	std::vector<std::string> lines;
//	std::ifstream file(filename);
//	if (!file)
//	{
//		mywindows::errlog("文件不存在");
//		return "FOF";
//	}
//	std::string line = "";
//	while (std::getline(file, line))
//	{
//		lines.push_back(line);
//	}
//	if (lines.empty())
//	{
//		mywindows::errlog("file is empty");
//		return "File is empty";
//	}
//	// 随机选择一行
//	int randomIndex;
//	do {
//		std::srand(static_cast<unsigned int>(std::time(nullptr)) * seed);
//		int temp = rand();
//		std::srand(((static_cast<unsigned int>(std::time(nullptr))) * seed + temp) * rand());
//		randomIndex = std::rand() % lines.size();
//		seed++;
//	} while (lines[randomIndex].empty());
//	return lines[randomIndex];
//}


std::string getname::RandomLineFromFile(const std::wstring& filename)
{
	std::vector<std::string> lines;
	std::wifstream file(filename);
	if (!file)
	{
		mywindows::errlog("文件不存在");
		return "FOF";
	}

	// 自动检测文件编码
	std::string encoding;
	try {
		encoding = detectEncoding(filename);
	}
	catch (const std::exception& e) {
		mywindows::errlog(e.what());
		return "FOF";
	}

	// 设置文件流的编码
	if (encoding == "UTF-8")
	{
		file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));
	}
	else if (encoding == "GBK" || encoding == "GB2312" || encoding == "GB18030")
	{
		file.imbue(locale("zh_cn"));
	}
	else if (encoding == "windows-1252" || encoding == "ISO-8859-1")
	{
		file.imbue(std::locale("en_us"));
	}
	else if (encoding == "UTF-16")
	{
		file.imbue(std::locale(file.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));
	}
	// 你可以根据需要添加更多的编码支持

	std::wstring line;
	while (std::getline(file, line))
	{
		// 将 wstring 转换为 UTF-8 编码的 string
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		lines.push_back(conv.to_bytes(line));
	}
	if (lines.empty())
	{
		mywindows::errlog("file is empty");
		return "File is empty";
	}

	// 随机选择一行
	int randomIndex;
	do {
		std::srand(static_cast<unsigned int>(std::time(nullptr)) * seed);
		int temp = rand();
		std::srand(((static_cast<unsigned int>(std::time(nullptr))) * seed + temp) * rand());
		randomIndex = std::rand() % lines.size();
		seed++;
	} while (lines[randomIndex].empty());
	return lines[randomIndex];
}

int getname::randomIntegerBetween(const int min, const int max) {
	// 初始化随机数生成器
	srand(static_cast<unsigned int>(time(nullptr)) + seed2);
	// 计算随机数的范围，并生成一个在这个范围内的随机数
	int range = max - min + 1;
	int randomNum = rand();
	srand(static_cast<unsigned int>(time(nullptr)) + seed2 + randomNum);
	randomNum = min + rand() % range;
	seed2 += randomNum;
	return randomNum;
}

void getname::ReRandom(int number)
{
	thread ([this, number]() {
		unsigned int t = 0;
		while (t<=255)
		{
			if (!random(number, t))
			{
				wstring basic_string = L"选取的名字文件有问题，对应的卡池序号为" + to_wstring(number + 1);
				MessageBox(NULL,basic_string.c_str() , L"错误", MB_ICONQUESTION);
				break;
			}
			t++;
		}
	}).detach();
}

getname* getname::getInstance()
{
	if(!instance)
	{
		instance = new getname();
	}
	return instance;
}

long long randi() {
	// 创建随机设备
	std::random_device rd;
	// 使用随机设备作为种子，创建一个随机数引擎
	std::mt19937_64 eng(rd());
	// 定义随机数分布范围
	std::uniform_int_distribution<long long> distr;

	// 生成并返回随机数
	return distr(eng);
}
int counter = 0;
int fourcounter = 0;
int getname::random_star()
{

	// 基础概率
	double fiveStarRate = 0.6; // 五星物品基础概率	
	const double fourStarRate = 5.1; // 四星物品基础概率

	// 检查是否达到提升概率的抽数
	if (counter >= 73) {
		// 从第74抽开始，每次提升6%，直到第90抽时为100%
		fiveStarRate += (counter - 73) * 6;
		if (fiveStarRate > 100) fiveStarRate = 100; // 保证概率不超过100%
	}

	// 生成随机数
	double randNum;
	randNum = (randi() / 100)% 100;
	counter++; // 增加计数器
	fourcounter++;
	// 检查是否达到五星保底
	if (counter >= 90) {
		counter = 0; // 重置计数器
		return 5;
	}
	if (fourcounter >= 10){
		fourcounter = 0;
		return 4;
	}
	// 根据概率抽取物品
	if (randNum <= fiveStarRate) {
		counter = 0; // 重置计数器
		return 5;
	}
	else if (randNum <= fourStarRate) {
		return 4;
	}
	else {
		return 3;
	}
}


std::string detectEncoding(const std::wstring& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		throw std::runtime_error("文件不存在");
	}

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	uchardet_t ud = uchardet_new();
	if (uchardet_handle_data(ud, buffer.data(), buffer.size()) != 0) {
		uchardet_delete(ud);
		throw std::runtime_error("无法检测文件编码");
	}
	uchardet_data_end(ud);
	const char* encoding = uchardet_get_charset(ud);
	std::string result(encoding);
	uchardet_delete(ud);
	return result;
}