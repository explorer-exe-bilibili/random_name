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
std::string GetDefaultLanguage(){
    std::string name=std::locale().name();
    if(name.empty())name="zh-CN";
    std::cout<<"language name:"<<name<<std::endl;
    return name;
}
