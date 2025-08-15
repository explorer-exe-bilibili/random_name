#include "core/baseItem/lang.h"
#include <iostream>

int main() {
    // 测试基本的字符串转换
    std::string result1 = core::to_string_helper(42);
    std::string result2 = core::to_string_helper(3.14);
    std::string result3 = core::to_string_helper("test");
    
    std::cout << "Int: " << result1 << std::endl;
    std::cout << "Double: " << result2 << std::endl;
    std::cout << "String: " << result3 << std::endl;
    
    return 0;
}
