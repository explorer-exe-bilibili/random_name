/*
 * UTF-8字符处理测试示例
 * 
 * 这个文件展示了如何正确处理UTF-8编码的中文字符，
 * 避免在编辑文本时出现崩溃问题
 */

#include <iostream>
#include <string>
#include <vector>

// 模拟之前有问题的退格处理
void buggy_backspace(std::string& text, size_t& cursor) {
    std::cout << "=== 有问题的退格处理 ===" << std::endl;
    std::cout << "原始文本: " << text << std::endl;
    std::cout << "字节长度: " << text.length() << std::endl;
    
    if (cursor > 0) {
        // 这是有问题的处理方式 - 只删除一个字节
        text.erase(cursor - 1, 1);
        cursor--;
        std::cout << "删除后: " << text << " (可能是无效的UTF-8!)" << std::endl;
    }
}

// 正确的UTF-8退格处理
void safe_backspace(std::string& text, size_t& char_cursor) {
    std::cout << "=== 安全的退格处理 ===" << std::endl;
    std::cout << "原始文本: " << text << std::endl;
    
    if (char_cursor > 0 && !text.empty()) {
        size_t pos = text.length();
        
        // 从后往前找到一个完整字符的开始位置
        while (pos > 0) {
            pos--;
            unsigned char c = text[pos];
            
            // 如果是ASCII字符或UTF-8序列的开始字节
            if (c < 0x80 || (c >> 6) != 0x02) {
                break;
            }
        }
        
        text = text.substr(0, pos);
        char_cursor--;
        std::cout << "安全删除后: " << text << std::endl;
    }
}

// 计算UTF-8字符串的字符数量
size_t utf8_char_count(const std::string& str) {
    size_t count = 0;
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        if (c < 0x80) {
            i += 1; // ASCII
        } else if ((c >> 5) == 0x06) {
            i += 2; // 2字节字符
        } else if ((c >> 4) == 0x0E) {
            i += 3; // 3字节字符 (中文)
        } else if ((c >> 3) == 0x1E) {
            i += 4; // 4字节字符
        } else {
            i += 1; // 无效序列
        }
        count++;
    }
    return count;
}

int main() {
    std::cout << "UTF-8字符处理测试" << std::endl;
    std::cout << "==================" << std::endl;
    
    // 测试字符串：包含中文字符
    std::vector<std::string> test_strings = {
        "Hello世界",
        "中文测试",
        "Mixed混合文本",
        "🎮游戏🎯",
        "English"
    };
    
    for (const auto& test_str : test_strings) {
        std::cout << "\n--- 测试字符串: " << test_str << " ---" << std::endl;
        std::cout << "字节长度: " << test_str.length() << std::endl;
        std::cout << "字符数量: " << utf8_char_count(test_str) << std::endl;
        
        // 演示有问题的处理方式
        std::string buggy_copy = test_str;
        size_t buggy_cursor = buggy_copy.length();
        buggy_backspace(buggy_copy, buggy_cursor);
        
        // 演示安全的处理方式
        std::string safe_copy = test_str;
        size_t safe_cursor = utf8_char_count(safe_copy);
        safe_backspace(safe_copy, safe_cursor);
        
        std::cout << std::endl;
    }
    
    std::cout << "=== 重要说明 ===" << std::endl;
    std::cout << "1. UTF-8中文字符通常占用3个字节" << std::endl;
    std::cout << "2. 直接按字节删除会破坏字符边界" << std::endl;
    std::cout << "3. 必须按完整字符进行删除操作" << std::endl;
    std::cout << "4. 使用字符位置而不是字节位置来跟踪光标" << std::endl;
    std::cout << "5. 通过GLFW的CharCallback处理Unicode输入" << std::endl;
    
    return 0;
}
