# 中文字符输入崩溃问题修复

## 问题描述

在文本框中输入中文字符后按退格键会导致程序崩溃。这是一个典型的UTF-8编码处理问题。

## 问题原因

### 原始有问题的代码

```cpp
if (key == '\b' || key == 127) { // 退格键
    if (cursorPosition > 0) {
        editingText.erase(cursorPosition - 1, 1);  // ❌ 只删除1个字节
        cursorPosition--;
    }
}
```

### 问题分析

1. **字符编码差异**:
   - ASCII字符占用1个字节
   - 中文字符在UTF-8中通常占用3个字节

2. **字符边界破坏**:
   - `erase(pos, 1)` 只删除1个字节
   - 对中文字符会破坏字符边界
   - 产生无效的UTF-8序列
   - 导致后续字符串操作崩溃

3. **光标位置错误**:
   - 使用字节位置而不是字符位置
   - 导致光标定位错误

## 解决方案

### 1. 添加UTF-8字符处理函数

```cpp
// 计算UTF-8字符串中的字符数量
size_t utf8_length(const std::string& str);

// 安全地删除最后一个字符
std::string utf8_remove_last_char(const std::string& str);

// 字符位置与字节位置转换
size_t utf8_char_pos_to_byte_pos(const std::string& str, size_t char_pos);
size_t utf8_byte_pos_to_char_pos(const std::string& str, size_t byte_pos);
```

### 2. 修复退格键处理

```cpp
if (key == '\b' || key == 127) { // 退格键
    if (cursorPosition > 0) {
        try {
            // 找到前一个字符的完整边界
            size_t byte_pos = utf8_char_pos_to_byte_pos(editingText, cursorPosition);
            size_t prev_char_start = byte_pos;
            
            // 向前查找字符开始位置
            do {
                prev_char_start--;
                if (prev_char_start == 0) break;
                unsigned char c = editingText[prev_char_start];
                if (c < 0x80 || (c >> 6) != 0x02) break;
            } while (prev_char_start > 0);
            
            // 删除完整字符
            editingText.erase(prev_char_start, byte_pos - prev_char_start);
            cursorPosition--; // 字符位置减1
        }
        catch (...) {
            // 备选方案
            editingText = utf8_remove_last_char(editingText);
            cursorPosition = utf8_length(editingText);
        }
    }
}
```

### 3. 添加Unicode字符输入支持

#### 3.1 添加字符回调函数

```cpp
void CharEvent(GLFWwindow* window, unsigned int codepoint) {
    // 将Unicode码点转换为UTF-8字符串
    std::string utf8_char = unicode_to_utf8(codepoint);
    
    // 传递给当前屏幕处理
    if (screen::Screen::getCurrentScreen()) {
        screen::Screen::getCurrentScreen()->HandleUnicodeInput(utf8_char);
    }
}
```

#### 3.2 在main.cpp中设置回调

```cpp
glfwSetCharCallback(WindowInfo.window, CharEvent);
```

#### 3.3 实现Unicode处理链

```none
GLFW CharCallback 
    ↓
Screen::HandleUnicodeInput() 
    ↓
SettingScreen::HandleUnicodeInput() 
    ↓
SettingButton::HandleUnicodeInput()
```

### 4. 修复光标位置计算

```cpp
// 使用字符位置而不是字节位置
cursorPosition = utf8_length(editingText);

// 插入光标显示
size_t byte_pos = utf8_char_pos_to_byte_pos(displayText, cursorPosition);
displayText.insert(byte_pos, "|");
```

## 修复文件列表

### 修改的文件

1. **SettingButtton.cpp**
   - 添加UTF-8处理函数
   - 修复退格键处理
   - 添加HandleUnicodeInput方法
   - 修复光标位置计算

2. **mainloop.cpp**
   - 添加CharEvent字符回调函数

3. **mainloop.h**
   - 声明CharEvent函数

4. **main.cpp**
   - 设置glfwSetCharCallback

5. **base.h**
   - 添加HandleUnicodeInput虚函数

6. **SettingScreen.h**
   - 添加HandleUnicodeInput方法声明

7. **SettingScreen.cpp**
   - 实现HandleUnicodeInput方法

### 新增的示例文件

1. **utf8_text_handling_demo.cpp** - UTF-8处理演示
2. **本文档** - 修复说明

## 技术细节

### UTF-8编码规则

- 1字节: `0xxxxxxx` (ASCII, 0-127)
- 2字节: `110xxxxx 10xxxxxx` (128-2047)
- 3字节: `1110xxxx 10xxxxxx 10xxxxxx` (2048-65535, 包含大部分中文)
- 4字节: `11110xxx 10xxxxxx 10xxxxxx 10xxxxxx` (65536+, 包含Emoji)

### 字符边界检测

```cpp
// 检查是否为UTF-8序列的开始字节
if (c < 0x80 || (c >> 6) != 0x02) {
    // 这是字符的开始字节
}
```

### 安全删除策略

1. 从当前位置向前查找
2. 找到字符开始边界
3. 删除完整字符
4. 更新字符位置(不是字节位置)

## 测试建议

### 测试用例

1. 纯英文输入和删除
2. 纯中文输入和删除
3. 中英文混合输入和删除
4. 包含Emoji的文本
5. 光标移动到中间位置的编辑

### 测试步骤

1. 打开设置界面
2. 点击文本框类型的设置项
3. 输入中文字符 (如: "你好世界")
4. 使用退格键逐个删除字符
5. 验证不会崩溃且删除正确

## 性能影响

- UTF-8处理函数的时间复杂度为O(n)，其中n为字符串长度
- 对于一般长度的设置文本(通常<100字符)，性能影响微乎其微
- 相比崩溃问题，这个开销是完全可以接受的

## 后续改进建议

1. **光标移动支持**: 添加左右箭头键支持
2. **选择文本**: 支持Shift+箭头键选择文本
3. **剪贴板操作**: 支持Ctrl+C/V/X
4. **IME支持**: 更好的输入法支持
5. **文本验证**: 实时的输入验证和提示

## 兼容性

- ✅ Windows (UTF-8 + Unicode)
- ✅ macOS (UTF-8 + Unicode)  
- ✅ Linux (UTF-8 + Unicode)
- ✅ 所有现代字体和输入法
- ✅ Emoji和特殊符号
