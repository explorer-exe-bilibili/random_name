#include "explorer.h"

#include <stdexcept>
#include <filesystem>
#include <regex>
#include <vector>
#include <algorithm>
#include <memory>

#include "ConfigItem.h"
#include "mywindows.h"

explorer* explorer::instance = nullptr;

explorer::explorer()
{
	Load();
}

explorer::~explorer()
{
	for(auto& i:HBitmap)
	{
		if(i)
			DeleteObject(&i);
		i = nullptr;
	}
	for(auto&i:Bitmap)
	{
		i = {};
	}
	HBitmap.clear();
	Bitmap.clear();
	GdiImage.clear();
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

HBITMAP explorer::getHBitmap(const int number)const
{
	if (number >= HBitmap.size() || number < 0)
		throw std::out_of_range("number out of range");
	else if(!HBitmap[number])
		throw std::runtime_error("HBitmap is nullptr");
	else
		return HBitmap[number];
}

const BITMAP* explorer::getBitmap(const int number) const
{
	if(number>=Bitmap.size()||number<0)
		throw std::out_of_range("number out of range");
	else
		return &Bitmap[number];
}

const Gdiplus::Image* explorer::getGdiImage(const int number)const
{
	if (number >= GdiImage.size() || number < 0)
		throw std::out_of_range("number out of range");
	else
		return GdiImage[number].get();
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
		GdiImage.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(configName).c_str()));
		presses+=0.5;
	}
	std::wstring path = config::getpath(FLOATPHOTO);
	GdiImage.push_back(std::make_shared<Gdiplus::Bitmap>(path.c_str()));
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
		GdiImage.push_back(std::make_shared<Gdiplus::Bitmap>(file.second.c_str()));
		presses+=0.5;
	}
	for (auto& i : GdiImage)
	{
		if(i->GetLastStatus()!=Gdiplus::Status::Ok)
		{
			mywindows::errlog(L"图片加载失败");
		}
		HBITMAP temp;
		i->GetHBITMAP(Gdiplus::Color(0, 0, 0), &temp);
		HBitmap.push_back(temp);
		presses += 0.25;
	}
	for (auto& i : HBitmap)
	{
		if (!i)
			throw std::runtime_error("HBitmap is nullptr");
		else
		{
			BITMAP temp;
			GetObject(i, sizeof(BITMAP), &temp);
			Bitmap.push_back(temp);
		}
		presses += 0.1;
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
	if (number < 0 || number >= GdiImage.size())return;
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
	GdiImage[number] = std::make_shared<Gdiplus::Bitmap>(path.c_str());
	Gdiplus::Bitmap* t_gb = new Gdiplus::Bitmap(path.c_str());
	if (t_gb->GetLastStatus() != Gdiplus::Status::Ok)
	{
		std::wstring message = L"加载图片出错，请检查文件路径，报错码为（gdiplus）：" + t_gb->GetLastStatus();
		MessageBox(nullptr, message.c_str(), L"错误", MB_ICONERROR);
		mywindows::errlog(message.c_str());
		return;
	}
	GdiImage[number].reset(t_gb);
	DeleteObject(HBitmap[number]);
	HBITMAP t;
	GdiImage[number]->GetHBITMAP(Gdiplus::Color(0, 0, 0), &t);
	HBitmap[number] = t;
	BITMAP t_B;
	GetObject(t, sizeof(BITMAP), &t_B);
	Bitmap[number] = t_B;
}

void explorer::reloadVideo(std::wstring alias) const
{
	if (config::getint(MEM))return;
	video->unload(alias);
	video->load(alias,config::getpath(alias));
}
