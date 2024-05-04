#pragma once
#include <string>
#include <vector>
#include"Log.h"
class local
{
private:
	static std::string localversion;
	static std::string findMaxVersion(const std::vector<std::string>& versions);
	static std::string removeAfterpoint(const std::string& input);
	static void getlocalversion();
public:
	static bool isupdated(std::string online);
};

