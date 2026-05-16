# 代码风格指南 / Code Style Guide

## 中文版本

### 命名规范

#### 类型命名

| 类型 | 规范 | 示例 |
|------|------|------|
| 类 | PascalCase | `class VideoPlayer { }` |
| 结构体 | PascalCase | `struct Config { }` |
| 枚举 | PascalCase | `enum class BitmapID { }` |
| 枚举值 | PascalCase | `BitmapID::Background` |
| 类型别名 | PascalCase | `using ImageData = std::vector<uint8_t>;` |

#### 变量和函数命名

| 类型 | 规范 | 示例 |
|------|------|------|
| 成员变量 | camelCase或snake_case | `windowWidth`, `frame_count` |
| 静态变量 | snake_case | `static int max_frames` |
| 局部变量 | camelCase | `deltaTime, pixelData` |
| 函数/方法 | camelCase | `void playAudio()` |
| 常量 | SCREAMING_SNAKE_CASE | `const int MAX_WIDTH = 1920;` |
| 宏 | SCREAMING_SNAKE_CASE | `#define GL_CHECK_ERROR()` |

#### 文件命名

| 类型 | 规范 | 示例 |
|------|------|------|
| 头文件 | .h | `VideoPlayer.h` |
| 源文件 | .cpp | `VideoPlayer.cpp` |
| 文件名 | PascalCase | `MainScreen.cpp` |
| 配置文件 | snake_case | `cmake_config.json` |

---

### 代码格式

#### 缩进和空格

- **缩进**: 4个空格（不使用制表符）
- **行长**: 最多120字符（超过时换行）
- **空行**: 逻辑块之间使用空行分隔

```cpp
#include <iostream>
#include <vector>

class MyClass {
public:
    MyClass();
    ~MyClass();
    
    void doSomething();

private:
    int memberVar;
    std::vector<int> items;
};
```

#### 大括号风格

使用**Egypt style**（左大括号在行尾）：

```cpp
// ✓ 推荐
void myFunction() {
    if (condition) {
        doSomething();
    }
}

// ✗ 不推荐
void myFunction()
{
    if (condition)
    {
        doSomething();
    }
}
```

#### 指针和引用

指针和引用符号靠近类型：

```cpp
// ✓ 推荐
int* ptr = nullptr;
const std::string& str = getName();

// ✗ 不推荐
int *ptr = nullptr;
const std::string &str = getName();
```

---

### 类设计规范

#### 成员变量排序

```cpp
class MyClass {
public:
    // 公开接口
    MyClass();
    ~MyClass();
    
    void publicMethod();
    int getStatus() const;

protected:
    // 受保护成员
    void protectedMethod();
    
    int protectedMember;

private:
    // 私有成员
    void privateMethod();
    
    int privateMember;
    std::vector<Data> dataList;
};
```

#### 初始化列表

```cpp
// ✓ 推荐
MyClass::MyClass(int width, int height)
    : width(width), height(height), data(nullptr) {
    // 构造函数体
}

// ✗ 不推荐
MyClass::MyClass(int width, int height) {
    this->width = width;
    this->height = height;
    data = nullptr;
}
```

#### 虚函数

```cpp
class Base {
public:
    virtual ~Base() = default;  // 虚析构函数
    virtual void process() = 0;  // 纯虚函数
    virtual void update() { }    // 可覆盖的虚函数
};

class Derived : public Base {
public:
    void process() override;  // 明确标记重写
};
```

---

### 头文件规范

#### Include守卫

使用`#pragma once`（现代C++推荐）：

```cpp
#pragma once

#include <memory>
#include <vector>
#include "core/config.h"

class MyClass {
    // 定义
};
```

或使用传统守卫：

```cpp
#ifndef MYCLASS_H
#define MYCLASS_H

#include <memory>

class MyClass {
    // 定义
};

#endif // MYCLASS_H
```

#### Include顺序

```cpp
// 1. 标准库
#include <iostream>
#include <vector>
#include <map>

// 2. 第三方库
#include <glad/glad.h>
#include <glfw/glfw3.h>

// 3. 项目内部
#include "core/log.h"
#include "core/explorer.h"
```

---

### 注释规范

#### 文件头注释

```cpp
/**
 * @file VideoPlayer.h
 * @brief 视频播放器实现
 * @details 支持FFmpeg解码，支持多种视频格式
 * @author Your Name
 * @date 2024-01-15
 */
```

#### 函数注释

```cpp
/**
 * @brief 播放视频文件
 * @param filePath 视频文件路径
 * @param loop 是否循环播放（默认false）
 * @return 如果成功加载返回true，否则返回false
 * @throw std::runtime_error 当FFmpeg初始化失败时抛出
 */
bool play(const std::string& filePath, bool loop = false);
```

#### 代码注释

```cpp
// ✓ 好的注释（解释为什么）
// 使用双倍缓冲避免屏幕闪烁
glBindFramebuffer(GL_FRAMEBUFFER, offscreenFBO);

// ✗ 不好的注释（解释代码本身）
// 绑定帧缓冲对象
glBindFramebuffer(GL_FRAMEBUFFER, offscreenFBO);
```

---

### 现代C++实践

#### 使用auto

```cpp
// ✓ 推荐：类型显然
auto it = myVector.begin();
auto result = calculateValue();

// ✗ 避免：类型不清楚
auto var = getSomeData();  // getSomeData返回什么类型？
```

#### 智能指针

```cpp
// ✓ 推荐：自动管理生命周期
std::unique_ptr<MyClass> ptr(new MyClass());
auto shared = std::make_shared<Resource>();

// ✗ 避免：手动管理
MyClass* ptr = new MyClass();
delete ptr;  // 容易遗忘
```

#### constexpr和const

```cpp
// ✓ 推荐
const int MAX_SIZE = 100;
constexpr float PI = 3.14159f;
void process(const std::vector<int>& data);

// ✗ 避免
int maxSize = 100;  // 不清楚是否应该修改
void process(std::vector<int> data);  // 不必要的复制
```

#### 范围for循环

```cpp
// ✓ 推荐
for (const auto& item : myList) {
    process(item);
}

// ✗ 避免
for (int i = 0; i < myList.size(); i++) {
    process(myList[i]);
}
```

---

### 错误处理

#### 异常使用

```cpp
// 使用异常处理严重错误
try {
    if (!file.open()) {
        throw std::runtime_error("无法打开文件");
    }
} catch (const std::exception& e) {
    LOG_ERROR("文件错误: %s", e.what());
}
```

#### 日志使用

```cpp
#include "core/log.h"

// 调试信息
LOG_DEBUG("变量值: %d", value);

// 一般信息
LOG_INFO("应用启动");

// 警告信息
LOG_WARN("某个资源未找到，使用默认值");

// 错误信息
LOG_ERROR("致命错误: %s", error.message());
```

#### 返回值检查

```cpp
// ✓ 推荐：检查返回值
if (!resource.load(path)) {
    LOG_ERROR("无法加载资源: %s", path.c_str());
    return false;
}

// ✗ 避免：忽视返回值
resource.load(path);  // 失败时无法诊断
```

---

### 性能建议

#### 避免不必要的复制

```cpp
// ✓ 推荐：使用引用
void process(const std::vector<int>& data);
std::string name = getString();  // 移动语义

// ✗ 避免：不必要的复制
void process(std::vector<int> data);  // 复制整个vector
```

#### 预分配内存

```cpp
// ✓ 推荐
std::vector<int> items;
items.reserve(1000);  // 预分配空间
for (int i = 0; i < 1000; i++) {
    items.push_back(i);
}

// ✗ 避免：频繁重新分配
std::vector<int> items;
for (int i = 0; i < 1000; i++) {
    items.push_back(i);  // 可能多次重新分配
}
```

#### 缓存常用值

```cpp
// ✓ 推荐
const int size = myList.size();
for (int i = 0; i < size; i++) {
    // 使用 size 而不是每次调用 myList.size()
}

// ✗ 避免
for (int i = 0; i < myList.size(); i++) {
    // 每次迭代都调用 size()
}
```

---

### 测试代码规范

测试代码遵循相同的风格，位于 `examples/` 目录：

```cpp
/**
 * @file test_video_completion.cpp
 * @brief 视频完成检测功能测试
 */

#include <cassert>
#include "core/baseItem/VideoPlayer.h"

int main() {
    // 准备
    VideoPlayer player;
    
    // 执行
    player.load("test_video.mp4");
    player.play();
    
    // 验证
    assert(player.isPlaying());
    
    return 0;
}
```

---

## English Version

### Naming Conventions

**Classes**: PascalCase (`VideoPlayer`)  
**Functions**: camelCase (`playAudio()`)  
**Constants**: SCREAMING_SNAKE_CASE (`MAX_WIDTH`)  
**Files**: PascalCase (`VideoPlayer.cpp`)  
**Macros**: SCREAMING_SNAKE_CASE (`GL_CHECK_ERROR`)

### Code Format

- **Indentation**: 4 spaces
- **Line length**: Max 120 characters
- **Braces**: Egypt style (left brace at end of line)
- **Pointers**: Attach to type (`int* ptr`)

### Best Practices

- Use `auto` for obvious types
- Prefer smart pointers (`unique_ptr`, `shared_ptr`)
- Use `const` and `constexpr` appropriately
- Use range-based for loops
- Include in order: std library, third-party, project internal
- Use exceptions for serious errors
- Always check return values
- Avoid unnecessary copies
- Pre-allocate memory when size is known

### Comments

- **Why, not what**: Explain reasoning, not implementation details
- **Use documentation comments**: For public APIs
- **Keep comments updated**: With code changes

### File Headers

All source files should have a header comment with:
- File name and purpose
- Brief description
- Author (if applicable)
- Date (if tracking)

