/*
 * 语言格式化功能使用示例
 * 
 * 展示统一的 text() 和 wtext() 函数，自动检测是否需要格式化
 * 无需记住两套不同的函数名！
 */

#include "core/baseItem/lang.h"
#include "core/log.h"
#include <iostream>

using namespace core::LanguageUtils;

int main() {
    // 设置语言为简体中文
    setLang(core::Language::ChineseSimplified);
    
    std::cout << "=== 统一的语言函数测试 ===" << std::endl;
    
    // 1. 无参数调用（传统用法）
    std::cout << "无参数: " << text("button.skip") << std::endl;
    
    // 2. 单个参数（自动格式化）
    std::string current_path = "C:\\Users\\Test\\随机点名";
    std::cout << "单个参数: " << text("warn.msgbox.line2", current_path) << std::endl;
    
    // 3. 多个参数（自动格式化）
    int width = 1920, height = 1080;
    std::cout << "多个参数: " << text("log.window_resized", width, height) << std::endl;
    
    // 4. 混合类型参数
    float progress = 75.6f;
    int current = 756, total = 1000;
    std::cout << "混合类型: " << text("progress.status", progress, current, total) << std::endl;
    
    // 5. 不同数据类型
    long long memory_used = 512, memory_total = 2048;
    std::cout << "长整型: " << text("performance.memory", memory_used, memory_total) << std::endl;
    
    std::string username = "张三";
    int level = 42;
    std::cout << "字符串+整数: " << text("user.greeting", username, level) << std::endl;
    
    // 6. wstring版本也是同样的用法
    std::wcout << L"宽字符无参数: " << wtext("button.skip") << std::endl;
    std::wcout << L"宽字符格式化: " << wtext("warn.msgbox.line2", current_path) << std::endl;
    
    // 7. 测试特殊情况
    std::cout << "不存在的键: " << text("nonexistent.key", "test", 123) << std::endl;
    std::cout << "空参数但有占位符: " << text("warn.msgbox.line2") << std::endl;
    
    // 8. 用const char*
    std::cout << "C字符串: " << text("file.loading", "config.json") << std::endl;
    
    // 9. 浮点数格式化
    float fps = 60.5f;
    std::cout << "浮点数: " << text("performance.fps", fps) << std::endl;
    
    std::cout << "\n=== 同一个函数，智能判断是否格式化！ ===" << std::endl;
    return 0;
}
