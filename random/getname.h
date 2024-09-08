#pragma once
#include <windows.h>
#include<string>

#define BOW 0
#define SWORD 1
#define MAGIC 2
#define CLAYMORE 3
#define POLEARM 4

class getname
{
private:
	int seed=12341, seed2=23332;
	int getattrib(const std::string& input);
	std::string removeAfterDash(const std::string& input);
	std::string RandomLineFromFile(const std::wstring& filename);
	int random_star();
	static getname* instance;
public:
	struct Item
	{
		std::wstring name = L"";
		int star = 3;
		int type = 0;
		int elemant = 0;
	}Item;
	getname();
	~getname();
	struct Item items[4][256];
	bool fileerr;
	bool random(int m, int i);
	int randomIntegerBetween(int min, int max);
	void ReRandom(int number);
	static getname* getInstance();
};
