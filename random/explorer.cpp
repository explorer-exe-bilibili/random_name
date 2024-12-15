#include "explorer.h"

#include <stdexcept>
#include <filesystem>
#include <regex>
#include <vector>
#include <algorithm>
#include <memory>

#include "ConfigItem.h"
#include "mywindows.h"
#include "sth2sth.h"
#include "EasyGL/Bitmap.h"

explorer* explorer::instance = nullptr;

explorer::explorer()
{
	Load();
}

explorer::~explorer()
{
	mciSendString(L"close star3", nullptr, 0, nullptr);
	mciSendString(L"close star4", nullptr, 0, nullptr);
	mciSendString(L"close star5", nullptr, 0, nullptr);
	mciSendString(L"close starfull", nullptr, 0, nullptr);
	mciSendString(L"close enter", nullptr, 0, nullptr);
	mciSendString(L"close click", nullptr, 0, nullptr);
	mciSendString(L"close bgm", nullptr, 0, nullptr);
	mciSendString(L"close list", nullptr, 0, nullptr);

}

explorer* explorer::getInstance()
{
	if (!instance) {
		instance = new explorer();
	}
	return instance;
}


const Bitmap* explorer::getBitmap(const int number)const
{
	if (number >= Bitmaps.size() || number < 0)
		throw std::out_of_range("number out of range");
	else
		return &Bitmaps[number];
}

void explorer::PlayMusic(const std::string& alias)
{
	if (config::getint(OFFMUSIC))return;
	const std::string cmd = "play " + alias + " from 0";
	mciSendStringA(cmd.c_str(),nullptr,0,nullptr);
}

void explorer::PlayMusic(const std::wstring& alias)
{
	if (config::getint(OFFMUSIC))return;
	const std::wstring cmd = L"play " + alias + L" from 0";
	mciSendStringW(cmd.c_str(), nullptr, 0, nullptr);
}

void explorer::PlayVideo(const std::wstring& alias) const
{
	if(config::getint(MEM))
	{
		video->load(alias,config::getpath(alias));
	}
	video->play(alias);
	if (config::getint(MEM))
	{
		video->unload(alias);
	}
}

void explorer::stopMusic()
{
	mciSendString(L"stop starfull", nullptr, 0, nullptr);
	mciSendString(L"stop star5", nullptr, 0, nullptr);
	mciSendString(L"stop star3", nullptr, 0, nullptr);
	mciSendString(L"stop star4", nullptr, 0, nullptr);
	mciSendString(L"stop enter", nullptr, 0, nullptr);
	mciSendString(L"stop click", nullptr, 0, nullptr);
	mciSendString(L"stop bgm", nullptr, 0, nullptr);
	mciSendString(L"stop list", nullptr, 0, nullptr);
}

double presses = 0;
std::string presses_str;


void explorer::Load()
{
	presses = 0;
	presses_str = "正在加载视频";
	video = VideoPlayer::getInstance();
	if (!config::getint(MEM)) {
		video->load(GROUPSTAR4, config::getpath(GROUPSTAR4));
		presses = 10;
		video->load(GROUPSTAR5, config::getpath(GROUPSTAR5));
		presses = 20;
		video->load(SIGNALSTAR3, config::getpath(SIGNALSTAR3));
		presses = 30;
		video->load(SIGNALSTAR4, config::getpath(SIGNALSTAR4));
		presses = 40;
		video->load(SIGNALSTAR5, config::getpath(SIGNALSTAR5));
	}
	presses = 50;
	presses_str = "加载图片中";
	for (char t = 1; t <= config::getint(POOL_COUNT); t++)
	{
		std::wstring configName = L"over" + std::to_wstring(t);
		Bitmaps.emplace_back(sth2sth::wstr2str(config::getpath(configName)));
		presses+=0.5;
	}
	std::wstring path = config::getpath(FLOATPHOTO);
	Bitmaps.emplace_back(sth2sth::wstr2str(path));
	std::wstring directory = config::getpath(IMAGE_DIRECTORY);
	std::wregex regex(LR"((\d+)\.\w+)");
	std::vector<std::pair<int, std::wstring>> files;

	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.is_regular_file())
		{
			std::wsmatch match;
			std::wstring filename = entry.path().filename().wstring();
			if (std::regex_match(filename, match, regex))
			{
				int number = std::stoi(match[1].str());
				files.emplace_back(number, entry.path().wstring());
			}
		}
	}

	std::ranges::sort(files, [](const auto& a, const auto& b) {
		return a.first < b.first;
		});
	presses++;
	for (const auto& file : files)
	{
		Bitmaps.emplace_back(sth2sth::wstr2str(file.second));
		presses+=0.5;
	}
	presses = 75;
	presses_str = "加载其它音乐和音效";
	mciSendString(L"open .\\files\\mp3\\reveal-3star.mp3 alias star3", nullptr, 0, nullptr);
	presses = 78;
	mciSendString(L"open .\\files\\mp3\\reveal-4star.mp3 alias star4", nullptr, 0, nullptr);
	presses = 81;
	mciSendString(L"open .\\files\\mp3\\reveal-5star.mp3 alias star5", nullptr, 0, nullptr);
	presses = 84;
	mciSendString(L"open .\\files\\mp3\\reveal-fullstar.mp3 alias starfull", nullptr, 0, nullptr);
	presses = 88;
	mciSendString(L"open .\\files\\mp3\\enter.mp3 alias enter", nullptr, 0, nullptr);
	presses = 94;
	mciSendString(L"open .\\files\\mp3\\click.mp3 alias click", nullptr, 0, nullptr);
	presses = 98;
	mciSendString(L"open .\\files\\mp3\\result-list.mp3 alias list", nullptr, 0, nullptr);
	presses = 100;
	ShowWindow(mywindows::main_hwnd, SW_SHOW);
}

void explorer::reloadBitmap(int number)
{
	if (number < 0 || number >= Bitmaps.size())return;
	std::wstring path;
	if (number <= 3)
	{
		std::wstring configName = L"over" + std::to_wstring(number + 1);
		path = config::getpath(configName);
	}
	else if (number > 3)
	{
		path = config::getpath(IMAGE_DIRECTORY);
		std::wregex regex(LR"((\d+)\.\w+)");
		std::vector<std::pair<int, std::wstring>> files;

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_regular_file())
			{
				std::wsmatch match;
				std::wstring filename = entry.path().filename().wstring();
				if (std::regex_match(filename, match, regex))
				{
					int number = std::stoi(match[1].str());
					files.emplace_back(number, entry.path().wstring());
				}
			}
		}

		std::ranges::sort(files, [](const auto& a, const auto& b) {
			return a.first < b.first;
			});
		path += files[number - 4].second;
	}
	Bitmaps[number] = Bitmap(sth2sth::wstr2str(path));
}

void explorer::reloadVideo(std::wstring alias) const
{
	if (config::getint(MEM))return;
	video->unload(alias);
	video->load(alias,config::getpath(alias));
}
