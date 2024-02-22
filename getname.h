#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <stdbool.h>
#include"mywindows.h"
#include"config.h"
#include <iostream>
#include <iostream>

bool IsFontInstalled(LPCWSTR fontName);
std::string RandomLineFromFile(const std::string& filename);
std::string removeAfterDash(const std::string& input);
int getstar(const std::string& input);
LPCWSTR random(int i);

int seed = 2434;
int star[256];

LPCWSTR random(int i) {
    std::string tmp1;
    LPCWSTR tmp3;
    const char* path = getConfigValue("namesfile");
    tmp1 = RandomLineFromFile(path);
    tmp3 = UTF8To16(tmp1.c_str());
    if (lstrcmpW(tmp3, L"FOF")==0||lstrcmpW(tmp3,L"FAIL TO READ") == 0) {
        MessageBox(NULL, L"FALL", L"", NULL);
        PostQuitMessage(0);
        return L"FALL";
    }
    else {
        star[i] = getstar(tmp1);
        tmp1 = removeAfterDash(tmp1);
        return tmp3;
    }
}

int getstar(const std::string& input) {
    // 找到第一个 "-"
    std::cerr << input << "\n";
    size_t dashPos = input.find("-");
    std::cerr << input << "\n";
    if (dashPos != std::string::npos) {
        // 如果找到了 "-", 获取 "-" 后面的数字
        std::string numberStr = input.substr(dashPos + 1);

        // 将数字字符串转换为整数
        int result = std::stoi(numberStr);
        return result;
    }

    // 如果未找到 "-" 或转换失败，返回默认值（可以根据需要修改）
    return 3;
}

std::string removeAfterDash(const std::string& input) {
    // 找到第一个 "-"
    size_t dashPos = input.find("-");

    if (dashPos != std::string::npos) {
        // 如果找到了 "-", 裁切掉 "-" 后面的内容
        return input.substr(0, dashPos);
    }
    else {
        // 如果未找到 "-", 直接返回原字符串
        return input;
    }
}

std::string RandomLineFromFile(const std::string& filename)
{
    std::vector<std::string> lines;
    std::ifstream file(filename);

    if (!file)
    {
        MessageBox(NULL, L"打开文件失败，请确保文件存在", L"敢让我不读文件？！", MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
        return "FOF";
    }

    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    if (lines.empty())
    {
        return "File is empty";
    }
    // 随机选择一行
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int randomIndex = std::rand() % lines.size();
    return lines[randomIndex];
}

bool IsFontInstalled(LPCWSTR fontName) {// 函数来检查字体是否存在
    LOGFONTW lf;
    memset(&lf, 0, sizeof(LOGFONTW));
    lf.lfCharSet = DEFAULT_CHARSET;
    wcscpy(lf.lfFaceName, fontName);

    BOOL fontExists = FALSE;
    EnumFontFamiliesExW(GetDC(NULL), &lf, [](const LOGFONTW* lpelfe, const TEXTMETRICW* lpntme, DWORD FontType, LPARAM lParam) -> int {
        BOOL* fontExists = (BOOL*)lParam;
        *fontExists = TRUE;
        return 0; // 停止枚举
        }, (LPARAM)&fontExists, 0);

    return fontExists;
}