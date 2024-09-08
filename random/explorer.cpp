#include "explorer.h"

#include <stdexcept>
#include <filesystem>
#include <regex>
#include <vector>
#include <algorithm>
#include <memory>

#include "config.h"
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
		i = 0;
	}
	for(auto&i:Bitmap)
	{
		i = {};
	}
	HBitmap.clear();
	Bitmap.clear();
	GdiImage.clear();
	mciSendString(L"close star3", 0, 0, 0);
	mciSendString(L"close star4", 0, 0, 0);
	mciSendString(L"close star5", 0, 0, 0);
	mciSendString(L"close starfull", 0, 0, 0);
	mciSendString(L"close enter", 0, 0, 0);
	mciSendString(L"close click", 0, 0, 0);

}

explorer* explorer::getInstance()
{
	if (!instance) {
		instance = new explorer();
		instance->Load();
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

void explorer::PlayMusic(const std::string& alias)const
{
	if (config::getint(OFFMUSIC))return;
	std::string cmd = "play " + alias + " from 0";
	mciSendStringA(cmd.c_str(),0,0,0);
}

void explorer::PlayMusic(const std::wstring& alias)const
{
	if (config::getint(OFFMUSIC))return;
	std::wstring cmd = L"play " + alias + L" from 0";
	mciSendStringW(cmd.c_str(), 0, 0, 0);
}

void explorer::Load()
{
	for (char t = 1; t <= config::getint(POOL_COUNT); t++)
	{
		std::wstring configName = L"over" + std::to_wstring(t);
		GdiImage.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(configName).c_str()));

	}
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

	std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
		});

	for (const auto& file : files)
	{
		GdiImage.push_back(std::make_shared<Gdiplus::Bitmap>(file.second.c_str()));
	}
	std::wstring path = config::getpath(FLOATPHOTO);
	GdiImage.push_back(std::make_shared<Gdiplus::Bitmap>(path.c_str()));
	for (auto& i : GdiImage)
	{
		if(i->GetLastStatus()!=Gdiplus::Status::Ok)
		{
			mywindows::errlog(L"Í¼Æ¬¼ÓÔØÊ§°Ü");
		}
		HBITMAP temp;
		i->GetHBITMAP(Gdiplus::Color(0, 0, 0), &temp);
		HBitmap.push_back(temp);
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
	}
	mciSendString(L"open .\\files\\mp3\\reveal-3star.mp3 alias star3", 0, 0, 0);
	mciSendString(L"open .\\files\\mp3\\reveal-4star.mp3 alias star4", 0, 0, 0);
	mciSendString(L"open .\\files\\mp3\\reveal-5star.mp3 alias star5", 0, 0, 0);
	mciSendString(L"open .\\files\\mp3\\reveal-fullstar.mp3 alias starfull", 0, 0, 0);
	mciSendString(L"open .\\files\\mp3\\enter.mp3 alias enter", 0, 0, 0);
	mciSendString(L"open .\\files\\mp3\\click.mp3 alias click", 0, 0, 0);
}

void explorer::reloadBitmap(int number)
{
	if (number < 0 || number >= GdiImage.size())return;
	std::wstring path;
	if (number <= 3)
	{
		std::wstring configName = L"over" + std::to_wstring(number + 1);
		path = config::getpath(configName).c_str();
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

		std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
			return a.first < b.first;
			});
		path += files[number - 4].second;
	}
	GdiImage[number] = std::make_shared<Gdiplus::Bitmap>(path.c_str());
	Gdiplus::Bitmap* t_gb = new Gdiplus::Bitmap(path.c_str());
	if (t_gb->GetLastStatus() != Gdiplus::Status::Ok)
	{
		std::wstring message = L"¼ÓÔØÍ¼Æ¬³ö´í£¬Çë¼ì²éÎÄ¼þÂ·¾¶£¬±¨´íÂëÎª£¨gdiplus£©£º" + t_gb->GetLastStatus();
		MessageBox(NULL, message.c_str(), L"´íÎó", MB_ICONERROR);
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