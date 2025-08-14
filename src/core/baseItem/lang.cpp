#include "core/baseItem/lang.h"
#include "core/log.h"
#include "core/baseItem/Base.h"
#include <iostream>
#include <fstream>
#include <format>
#include <sstream>
#include <type_traits>
#include <algorithm>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#include <winnls.h>
#elif defined(__APPLE__)
#include <cstdlib>
#else
#include <cstdlib>
#endif

using namespace core;
using namespace nlohmann;

namespace core {
namespace LanguageUtils {

Language currentLang = Language::ChineseSimplified;
json languageData;

void setLang(Language lang) {
    languageData.clear();
    currentLang = lang;
    
    // 从文件加载语言数据
    std::string langCode = to_string(lang);
    std::string langFilePath = "files/localization/" + langCode + ".json";
    
    std::ifstream langFile(langFilePath);
    if (langFile.is_open()) {
        try {
            langFile >> languageData;
            Log << Level::Info << "Loaded language file: " << langFilePath << op::endl;
        } catch (const std::exception& e) {
            Log << Level::Error << "Failed to parse language file " << langFilePath << ": " << e.what() << op::endl;
        }
        langFile.close();
    } else {
        Log << Level::Warn<< "Could not open language file: " << langFilePath << op::endl;
    }
}

Language getLang() {
    return currentLang;
}

std::string text_base(const std::string& key) {
    if (languageData.contains(key)) {
        if(languageData[key].is_string())
            return languageData[key].get<std::string>();
    }
    Log << Level::Warn << "Missing translation for key: " << key << op::endl;
    return "Missing translation: " + key;
}

std::wstring wtext_base(const std::string& key) {
    std::string value = text_base(key);
    return string2wstring(value);
}

// 模板实现已移到头文件以确保每个翻译单元可见，避免链接错误

} // namespace LanguageUtils
} // namespace core
std::string GetDefaultLanguage() {
    std::string langCode = "zh-CN"; // 默认语言
    
#ifdef _WIN32
    // Windows平台：使用Windows API获取用户默认语言
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) > 0) {
        // 转换为std::string
        int size = WideCharToMultiByte(CP_UTF8, 0, localeName, -1, nullptr, 0, nullptr, nullptr);
        if (size > 0) {
            std::string result(size - 1, '\0');
            WideCharToMultiByte(CP_UTF8, 0, localeName, -1, &result[0], size, nullptr, nullptr);
            langCode = result;
        }
    }
#elif defined(__APPLE__)
    // macOS平台：检查环境变量
    const char* lang = std::getenv("LANG");
    if (!lang) lang = std::getenv("LC_ALL");
    if (!lang) lang = std::getenv("LC_MESSAGES");
    if (lang) {
        std::string envLang(lang);
        // 提取语言代码部分 (例如: zh_CN.UTF-8 -> zh-CN)
        size_t dotPos = envLang.find('.');
        if (dotPos != std::string::npos) {
            envLang = envLang.substr(0, dotPos);
        }
        std::replace(envLang.begin(), envLang.end(), '_', '-');
        langCode = envLang;
    }
#else
    // Linux和其他平台：检查环境变量
    const char* lang = std::getenv("LANG");
    if (!lang) lang = std::getenv("LC_ALL");
    if (!lang) lang = std::getenv("LC_MESSAGES");
    if (lang) {
        std::string envLang(lang);
        // 提取语言代码部分 (例如: zh_CN.UTF-8 -> zh-CN)
        size_t dotPos = envLang.find('.');
        if (dotPos != std::string::npos) {
            envLang = envLang.substr(0, dotPos);
        }
        std::replace(envLang.begin(), envLang.end(), '_', '-');
        langCode = envLang;
    }
#endif
    
    // 标准化语言代码，确保返回支持的语言
    if (langCode.starts_with("zh-CN") || langCode.starts_with("zh_CN")) {
        langCode = "zh-CN";
    } else if (langCode.starts_with("zh-TW") || langCode.starts_with("zh_TW") || 
               langCode.starts_with("zh-HK") || langCode.starts_with("zh_HK")) {
        langCode = "zh-TW";
    } else if (langCode.starts_with("en")) {
        langCode = "en-US";
    } else if (langCode.starts_with("ja")) {
        langCode = "ja-JP";
    } else if (langCode.starts_with("ko")) {
        langCode = "ko-KR";
    } else {
        // 如果不是支持的语言，保持默认的en-US
        langCode = "en-US";
    }
    
    std::cout << "Detected system language: " << langCode << std::endl;
    return langCode;
}
