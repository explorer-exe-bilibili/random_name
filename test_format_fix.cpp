#include "core/baseItem/lang.h"
#include <iostream>

int main() {
    // 测试基本的字符串转换
    std::string result1 = core::LanguageUtils::to_string_helper(42);
    std::string result2 = core::LanguageUtils::to_string_helper(3.14);
    std::string result3 = core::LanguageUtils::to_string_helper("test");
    
    std::cout << "Int: " << result1 << std::endl;
    std::cout << "Double: " << result2 << std::endl;
    std::cout << "String: " << result3 << std::endl;
    
    // 验证结果
    if (result1 == "42" && result2 == "3.140000" && result3 == "test") {
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Test failed!" << std::endl;
        return 1;
    }
}
