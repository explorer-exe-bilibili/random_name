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
	static int seed, seed2;
	static int getattrib(const std::string& input);
	static std::string removeAfterDash(const std::string& input);
	static std::string RandomLineFromFile(const std::wstring& filename);
	static int random_star();
public:
	static struct Item
	{
		int star;
		int type;
		int elemant;
	}Item;
	static int star[4][256];
	static int type_[4][256];
	static bool fileerr;
	static LPCWSTR random(int m, int i);
	static int randomIntegerBetween(int min, int max);
};
