#include "ConfigItem.h"

#include "mywindows.h"
#include "sth2sth.h"

#include<string>
#include <filesystem>

using namespace std;
#define ARGB(a,r,g,b)	((uint32_t)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)|(((DWORD)(BYTE)(a))<<24)))
extern wstring configPath;

//读取和补全配置项
int config::init() {
	mywindows::log("initing config start");
	configPath = Log::wrunpath;
	configPath += CONFIG_;
	if (!std::filesystem::exists(configPath)) {
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
		//add(TEXTR, L"0");
		//add(TEXTG, L"125");
		//add(TEXTB, L"125");
		add(FPS, L"60");
		add(UNSUITFONT, L"0");
		add(DEBUG, L"0");
		add(INWINDOW, L"0");
		add(OFFMUSIC, L"0");
		add(FLOATWINDOW, L"1");
		add(NOSETTINGFILE, L"1");
		add(TYPICAL, L"0");
		add(MEM, L"0");
		add(NOSMOOTHUI, L"0");
		add(SPEED_FIRST_SCREEN, L"50");
		add(NAME_COLOR_6_STAR, ARGB(255, 0, 125, 125));
		add(NAME_COLOR, ARGB(255, 0, 125, 125));
		add(WINDOW_TITLE, L"祈愿");
		add(OVER4, L"\\files\\imgs\\over4.jpg");
		add(OVER3, L"\\files\\imgs\\over3.jpg");
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
		readFile();
		wstring LogString = get(MODE);
		if (LogString == L"err")add(MODE, L"1");
		LogString = get(OFF_VIDEO);
		if (LogString == L"err")add(OFF_VIDEO, L"0");
		LogString = get(WINDOW_TITLE);
		if (LogString == L"err")add(WINDOW_TITLE,L"原神");
		LogString = get(NAMES1);
		if (LogString == L"err")add(NAMES1, L"\\names1.txt");
		LogString = get(NAMES2);
		if (LogString == L"err")add(NAMES2, L"\\names2.txt");
		LogString = get(NAMES3);
		if (LogString == L"err")add(NAMES3, L"\\names3.txt");
		LogString = get(NAMES4);
		if (LogString == L"err")add(NAMES4, L"\\names4.txt");
		LogString = get(BLUE_BALL_COUNT);
		if (LogString == L"err")add(BLUE_BALL_COUNT, L"10000");
		LogString = get(PINK_BALL_COUNT);
		if (LogString == L"err")add(PINK_BALL_COUNT, L"10000");
		LogString = get(YUANSHI);
		if (LogString == L"err")add(YUANSHI, L"10000");
		LogString = get(SPECIAL);
		if (LogString == L"err")add(SPECIAL, L"0");
		LogString = get(OFFMUSIC);
		if (LogString == L"err")add(OFFMUSIC, L"0");
		LogString = get(INWINDOW);
		if (LogString == L"err")add(INWINDOW, L"0");
		LogString = get(FLOATWINDOW);
		if (LogString == L"err")add(FLOATWINDOW, L"1");
		LogString = get(NOSETTINGFILE);
		if (LogString == L"err")add(NOSETTINGFILE, L"1");
		LogString = get(UNSUITFONT);
		if (LogString == L"err")add(UNSUITFONT, L"0");
		LogString = get(DEBUG);
		if (LogString == L"err")add(DEBUG, L"0");
		LogString = get(TYPICAL);
		if (LogString == L"err")add(TYPICAL, L"0");
		LogString = get(MU);
		if (LogString == L"err")add(MU, L"0.9");/*
		LogString = get(TEXTR);
		if (LogString == L"err")add(TEXTR, L"0");
		LogString = get(TEXTG);
		if (LogString == L"err")add(TEXTG, L"125");
		LogString = get(TEXTB);
		if (LogString == L"err")add(TEXTB, L"125");*/
		LogString = get(FPS);
		if (LogString == L"err")add(FPS, L"60");
		LogString = get(MEM);
		if (LogString == L"err")add(MEM, L"0");
		LogString = get(NOSMOOTHUI);
		if (LogString == L"err")add(NOSMOOTHUI, L"0");
		LogString = get(SPEED_FIRST_SCREEN);
		if (LogString == L"err")add(SPEED_FIRST_SCREEN, L"50");
		LogString = get(FLOATX);
		if (LogString == L"err")add(FLOATX, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.8)));
		LogString = get(FLOATY);
		if (LogString == L"err")add(FLOATY, sth2sth::str2wstr(to_string(mywindows::screenHeight * 0.8)));
		LogString = get(FLOATW);
		if (LogString == L"err")add(FLOATW, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.05)));
		LogString = get(FLOATH);
		if (LogString == L"err")add(FLOATH, sth2sth::str2wstr(to_string(mywindows::screenWidth * 0.05)));
		LogString = get(OVER1);
		if (LogString == L"err")add(OVER1, L"\\files\\imgs\\over1.jpg");
		LogString = get(OVER2);
		if (LogString == L"err")add(OVER2, L"\\files\\imgs\\over2.jpg");
		LogString = get(OVER3);
		if (LogString == L"err")add(OVER3, L"\\files\\imgs\\over3.jpg");
		LogString = get(OVER4);
		if (LogString == L"err")add(OVER4, L"\\files\\imgs\\over4.jpg");
		LogString = get(FLOATPHOTO);
		if (LogString == L"err")add(FLOATPHOTO, L"\\files\\imgs\\float.jpg");
		LogString = get(SIGNALSTAR3);
		if (LogString == L"err")add(SIGNALSTAR3, L"\\files\\video\\3star-single.mp4");
		LogString = get(SIGNALSTAR4);
		if (LogString == L"err") add(SIGNALSTAR4, L"\\files\\video\\4star-single.mp4");
		LogString = get(SIGNALSTAR5);
		if (LogString == L"err")add(SIGNALSTAR5, L"\\files\\video\\5star-single.mp4");
		LogString = get(GROUPSTAR4);
		if (LogString == L"err")add(GROUPSTAR4, L"\\files\\video\\4star-multi.mp4");
		LogString = get(GROUPSTAR5);
		if (LogString == L"err")add(GROUPSTAR5, L"\\files\\video\\5star-multi.mp4");
		LogString = get(POOL_COUNT);
		if (LogString == L"err")add(POOL_COUNT, L"4");
		LogString = get(IMAGE_DIRECTORY);
		if (LogString == L"err")add(IMAGE_DIRECTORY, L"\\files\\imgs\\");
		saveFile();
		printAllConfigItems();
		mywindows::log(L"config init successfully");
	}
	return 0;
}