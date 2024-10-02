#pragma once
#include<string>

#define BOW 0
#define SWORD 1
#define MAGIC 2
#define CLAYMORE 3
#define POLEARM 4

struct NameItem
{
	std::wstring name = L"";
	int star = 3;
	int type = 0;
	int elemant = 0;
};

class getname
{
private:
	int seed=12341, seed2=23332;
	int getattrib(const std::string& input);
	std::string Make(const std::string& input);
	std::string RandomLineFromFile(const std::wstring& filename);
	int random_star();
	static getname* instance;
public:
	getname();
	~getname();
	NameItem items[4][256];
	bool fileerr;
	bool random(int m, int i);
	int randomIntegerBetween(int min, int max);
	void ReRandom(int number);
	static getname* getInstance();
};
