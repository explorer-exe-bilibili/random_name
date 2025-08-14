#pragma once
#include <string>
#include <format>
#include <type_traits>
#include <algorithm>
#include <sstream>

// 前向声明
namespace core {
    std::wstring string2wstring(const std::string& str);
    std::string wstring2string(const std::wstring& wstr);
}

namespace core {

enum class Language {
    ChineseSimplified,  // zh-CN
    English,           // en-US
    Japanese,         // ja-JP
    Korean,           // ko-KR
    ChineseTraditional // zh-TW
};

inline std::string to_string(Language lang) {
    switch (lang) {
        case Language::ChineseSimplified: return "zh-CN";
        case Language::English: return "en-US";
        case Language::Japanese: return "ja-JP";
        case Language::Korean: return "ko-KR";
        case Language::ChineseTraditional: return "zh-TW";
        default: return "unknown";
    }
}

inline Language to_languageID(std::string name){
    if (name == "zh-CN") return Language::ChineseSimplified;
    if (name == "en-US") return Language::English;
    if (name == "ja-JP") return Language::Japanese;
    if (name == "ko-KR") return Language::Korean;
    if (name == "zh-TW") return Language::ChineseTraditional;
    return Language::ChineseSimplified;  // 默认返回英文
}

namespace LanguageUtils {
    void setLang(Language lang);
    Language getLang();
    
    // 内部实现：不带格式化的基础函数
    std::string text_base(const std::string& key);
    std::wstring wtext_base(const std::string& key);
    
    // 支持wstring键的版本
    template<typename... Args>
    std::string text(const std::wstring& key, Args&&... args) {
        return text(wstring2string(key), std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    std::wstring wtext(const std::wstring& key, Args&&... args) {
        return wtext(wstring2string(key), std::forward<Args>(args)...);
    }
    
    // 内部格式化实现（声明）
    template<typename... Args>
    std::string text_formatted(const std::string& key, Args&&... args);
    
    template<typename... Args>
    std::wstring wtext_formatted(const std::string& key, Args&&... args);
    // 公共接口：支持可变参数的text函数
    template<typename... Args>
    std::string text(const std::string& key, Args&&... args) {
        if constexpr (sizeof...(args) == 0) {
            return text_base(key);
        } else {
            return text_formatted(key, std::forward<Args>(args)...);
        }
    }
    template<typename... Args>
    std::wstring wtext(const std::string& key, Args&&... args) {
        if constexpr (sizeof...(args) == 0) {
            return wtext_base(key);
        } else {
            return wtext_formatted(key, std::forward<Args>(args)...);
        }
    }

    extern Language currentLang;

    // Helper: 将任意类型转换为字符串（模板实现放在头文件以便每个翻译单元可见）
    template<typename T>
    inline std::string to_string_helper(T&& value) {
        if constexpr (std::is_arithmetic_v<std::decay_t<T>>) {
            return std::to_string(std::forward<T>(value));
        } else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
            return std::forward<T>(value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, const char*>) {
            return std::string(value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, char*>) {
            return std::string(value);
        } else if constexpr (std::is_same_v<std::decay_t<T>, std::wstring>) {
            return wstring2string(std::forward<T>(value));
        } else if constexpr (std::is_same_v<std::decay_t<T>, const wchar_t*>) {
            return wstring2string(std::wstring(value));
        } else if constexpr (std::is_same_v<std::decay_t<T>, wchar_t*>) {
            return wstring2string(std::wstring(value));
        } else {
            try {
                return std::format("{}", std::forward<T>(value));
            } catch (...) {
                return std::string("[无法转换的类型]");
            }
        }
    }

    // 占位符替换（递归）
    inline std::string replace_placeholder_recursive(const std::string& text, int /*placeholder_index*/) {
        return text;
    }

    template<typename T, typename... Args>
    inline std::string replace_placeholder_recursive(const std::string& text, int placeholder_index, T&& first, Args&&... rest) {
        std::string result = text;
        std::string placeholder = "{" + std::to_string(placeholder_index) + "}";
        std::string replacement = to_string_helper(std::forward<T>(first));

        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), replacement);
            pos += replacement.length();
        }

        return replace_placeholder_recursive(result, placeholder_index + 1, std::forward<Args>(rest)...);
    }

    // 模板实现：可见于头文件，避免链接错误
    template<typename... Args>
    inline std::string text_formatted(const std::string& key, Args&&... args) {
        std::string template_text = text_base(key);

        if (template_text.rfind("Missing translation", 0) == 0) {
            return template_text;
        }

        try {
            return replace_placeholder_recursive(template_text, 0, std::forward<Args>(args)...);
        } catch (...) {
            return template_text;
        }
    }

    template<typename... Args>
    inline std::wstring wtext_formatted(const std::string& key, Args&&... args) {
        std::string formatted = text_formatted(key, std::forward<Args>(args)...);
        return string2wstring(formatted);
    }
};

} // namespace core
