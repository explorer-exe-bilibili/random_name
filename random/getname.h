#pragma once
#include <windows.h>
#include<string>
class getname
{
private:
	static int seed, seed2;
public:
	static int star[4][256];
	static bool fileerr;
	static LPCWSTR random(int m, int i);
	static int getstar(const std::string& input);
	static std::string removeAfterDash(const std::string& input);
	static std::string RandomLineFromFile(const std::string& filename);
	static int randomIntegerBetween(int min, int max);
};