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

	// ��ȡ���а汾�ŵ����ֲ���
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

	// �ҵ��������ֲ���
	std::sort(numericParts.begin(), numericParts.end(), std::greater<>());
	std::string maxNumericPart = removeAfterpoint(numericParts.front());

	// ��ԭʼ�汾���в��Һ���������ֲ��ֵİ汾��
	for (const auto& version : versions) {
		if (version.find(maxNumericPart) != std::string::npos) {
			return version;
		}
	}
	return ""; // δ�ҵ�ƥ��İ汾��
}

std::string local::removeAfterpoint(const std::string& input) {
	// �ҵ���һ�� "."
	size_t dashPos;
	dashPos = input.find_last_of(".");
	if (dashPos != std::string::npos && dashPos < input.length()) {
		// ����ҵ��� ".", ���е� "." ���������
		std::string m = input.substr(0, dashPos);
		dashPos = m.find("-");
		if (dashPos != std::string::npos) {
			m = m.substr(0, dashPos);
			return m;
		}
		else return m;
	}
	else {
		// ���δ�ҵ� ".", ֱ�ӷ���ԭ�ַ���
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
