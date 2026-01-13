// 简单的示例测试 / Simple example test
// 用于演示测试基础设施 / Demonstrates test infrastructure

#include <iostream>
#include <string>

// 简单的测试框架函数 / Simple test framework functions
bool test_passed = true;

void assert_equal(int actual, int expected, const std::string& test_name) {
    if (actual == expected) {
        std::cout << "✓ " << test_name << " passed" << std::endl;
    } else {
        std::cout << "✗ " << test_name << " failed: expected " << expected 
                  << " but got " << actual << std::endl;
        test_passed = false;
    }
}

void assert_equal(const std::string& actual, const std::string& expected, const std::string& test_name) {
    if (actual == expected) {
        std::cout << "✓ " << test_name << " passed" << std::endl;
    } else {
        std::cout << "✗ " << test_name << " failed: expected '" << expected 
                  << "' but got '" << actual << "'" << std::endl;
        test_passed = false;
    }
}

// 测试函数 / Test functions
void test_basic_arithmetic() {
    std::cout << "\n=== Testing Basic Arithmetic ===" << std::endl;
    
    assert_equal(2 + 2, 4, "Addition");
    assert_equal(5 - 3, 2, "Subtraction");
    assert_equal(3 * 4, 12, "Multiplication");
    assert_equal(10 / 2, 5, "Division");
}

void test_string_operations() {
    std::cout << "\n=== Testing String Operations ===" << std::endl;
    
    std::string str1 = "Hello";
    std::string str2 = "World";
    std::string combined = str1 + " " + str2;
    
    assert_equal(combined, "Hello World", "String concatenation");
    assert_equal((int)str1.length(), 5, "String length");
}

int main() {
    std::cout << "Running basic tests..." << std::endl;
    
    test_basic_arithmetic();
    test_string_operations();
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    if (test_passed) {
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Some tests failed!" << std::endl;
        return 1;
    }
}
