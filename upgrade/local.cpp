#include "local.h"
#include <algorithm>
Log llog("upgrade.log", 1);

using namespace std;

string local::localversion = "";

bool local::isupdated(string online)
{
	getlocalversion();
	std::vector<std::string> versions;
	versions.push_back(localversion);
	versions.push_back(online);
	string heighest= findMaxVersion(versions);
	if (heighest == localversion) {
		return true;
	}
	return false;
}

std::string local::findMaxVersion(const std::vector<std::string>& versions) {
	std::vector<std::string> numericParts;

	// 提取所有版本号的数字部分
	for (const auto& version : versions) {
		std::string numericPart;
		for (char c : version) {
			if (std::isdigit(c)) {
				numericPart += c;
			}
			else if (!numericPart.empty() && c == '.') {
				numericPart += '.';
			}
		}
		numericParts.push_back(numericPart);
	}

	// 找到最大的数字部分
	std::sort(numericParts.begin(), numericParts.end(), std::greater<>());
	std::string maxNumericPart = removeAfterpoint(numericParts.front());

	// 在原始版本号中查找含有最大数字部分的版本号
	for (const auto& version : versions) {
		if (version.find(maxNumericPart) != std::string::npos) {
			return version;
		}
	}
	return ""; // 未找到匹配的版本号
}

std::string local::removeAfterpoint(const std::string& input) {
	// 找到第一个 "."
	size_t dashPos;
	dashPos = input.find_last_of(".");
	if (dashPos != std::string::npos && dashPos < input.length()) {
		// 如果找到了 ".", 裁切掉 "." 后面的内容
		std::string m = input.substr(0, dashPos);
		dashPos = m.find("-");
		if (dashPos != std::string::npos) {
			m = m.substr(0, dashPos);
			return m;
		}
		else return m;
	}
	else {
		// 如果未找到 ".", 直接返回原字符串
		return input;
	}
}

void local::getlocalversion()
{
	ifstream file("version");
	if (!file) {
		localversion = "0.0.0";
		return;
	}
	string versionline;
	getline(file, versionline);
	localversion = versionline;
}
