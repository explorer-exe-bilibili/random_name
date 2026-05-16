# 快速参考 / Quick Reference

## 中文版本 - 快速查询

### 项目信息速览

**项目名称：** 原神点名 (Genshin Name Picker)  
**语言：** C++17  
**构建系统：** CMake + VCPKG  
**平台：** Windows, macOS, Linux  
**许可证：** 见 LICENSE.txt

---

## 文件位置速查

### 源代码
```
src/
├── main.cpp          # 程序入口
├── mainloop.cpp      # 主循环
└── core/
    ├── baseItem/     # 音视频、字体、按钮等
    ├── render/       # OpenGL渲染系统
    └── screen/       # UI屏幕
```

### 资源文件
```
files/
├── fonts/            # 字体文件
├── imgs/             # 图片资源
├── localization/     # 多语言（JSON）
├── names/            # 名字库（文本）
├── music/            # 音乐文件
└── video/            # 视频文件
```

### 配置和构建
```
CMakeLists.txt       # 构建配置
CMakePresets.json    # 构建预设
CMakeSettings.json   # VS集成设置
vcpkg.json          # 依赖声明
```

---

## 常用命令速查

### 构建相关
```bash
# 生成构建目录（首次）
cmake -B out/build -G "Visual Studio 17 2022" \
  -DCMAKE_TOOLCHAIN_FILE=<VCPKG>/scripts/buildsystems/cmake/vcpkg.cmake

# 构建Debug版本
cmake --build out/build --config Debug

# 构建Release版本
cmake --build out/build --config Release

# 清理构建
cmake --build out/build --target clean
```

### VS Code快捷键
| 快捷键 | 功能 |
|--------|------|
| Ctrl+Shift+B | 打开构建任务 |
| F5 | 开始调试 |
| Ctrl+K Ctrl+C | 注释代码 |
| Ctrl+Shift+F | 全局搜索 |
| F12 | 转到定义 |

---

## 代码片段速查

### 日志输出
```cpp
#include "core/log.h"

LOG_DEBUG("调试信息");
LOG_INFO("普通信息");
LOG_WARN("警告信息");
LOG_ERROR("错误信息");
```

### 加载和播放音频
```cpp
#include "core/baseItem/Audio.h"

Audio audio;
if(audio.load("path/to/sound.wav")) {
    audio.play();
}
```

### 加载和显示图片
```cpp
#include "core/baseItem/Bitmap.h"

Bitmap bitmap;
if(bitmap.load("path/to/image.webp")) {
    bitmap.render(x, y);
}
```

### 播放视频
```cpp
#include "core/baseItem/VideoPlayer.h"

VideoPlayer video;
if(video.load("path/to/video.mp4")) {
    video.play();
    if(video.isCompleted()) {
        // 视频播放完成
    }
}
```

### 获取配置
```cpp
#include "core/Config.h"

std::string value = config->get("key", "default");
config->set("key", "value");
config->save();
```

### 获取本地化字符串
```cpp
#include "core/baseItem/lang.h"

std::string text = lang::get("ui_button_name");
```

### 切换屏幕
```cpp
#include "core/explorer.h"

explorer->switchScreen("screen_name");
```

### 创建按钮
```cpp
Button* btn = new Button(x, y, width, height);
btn->setText("按钮文本");
btn->setOnClick([](){ 
    // 按钮点击回调
});
```

---

## 依赖库速查

| 库 | 用途 | 头文件示例 |
|----|------|----------|
| GLFW3 | 窗口和输入 | `#include <GLFW/glfw3.h>` |
| Glad | OpenGL加载器 | `#include <glad/glad.h>` |
| GLM | 数学库 | `#include <glm/glm.hpp>` |
| FreeType | 字体处理 | 内部使用 |
| SDL2 | 音视频基础 | 内部使用 |
| FFmpeg | 视频解码 | 内部使用 |
| Curl | HTTP请求 | 内部使用 |
| libwebp | WebP图片 | 内部使用 |
| nlohmann/json | JSON处理 | 内部使用 |

---

## 关键类速查

| 类 | 位置 | 用途 |
|----|------|------|
| `Explorer` | core/ | 主控制器，资源管理 |
| `Config` | core/baseItem/ | 配置管理 |
| `Renderer` | core/render/ | 渲染器 |
| `Bitmap` | core/baseItem/ | 图片处理 |
| `Audio` | core/baseItem/ | 音频播放 |
| `VideoPlayer` | core/baseItem/ | 视频播放 |
| `Font` | core/baseItem/ | 字体处理 |
| `Button` | core/baseItem/ | UI按钮 |
| `Screen` | core/screen/ | 屏幕基类 |
| `MainScreen` | core/screen/ | 点名屏幕 |
| `SettingScreen` | core/screen/ | 设置屏幕 |

---

## 编码规范速查

### 命名规则
```cpp
class MyClass { };           // 类：PascalCase
void myFunction() { }        // 函数：camelCase
const int MAX_SIZE = 100;    // 常量：SCREAMING_SNAKE_CASE
int myVar = 0;              // 变量：camelCase
#define MY_MACRO()          // 宏：SCREAMING_SNAKE_CASE
```

### 格式规则
```cpp
// 缩进：4个空格
void function() {
    if (condition) {
        doSomething();
    }
}

// 指针靠近类型
int* ptr = nullptr;
const std::string& str = getName();

// 智能指针首选
std::unique_ptr<Resource> res(new Resource());
```

---

## 事件处理速查

### 键盘事件
```cpp
void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // action: GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        // 处理ESC键按下
    }
}
```

### 鼠标事件
```cpp
void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
    // button: GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT等
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // 处理左键点击
    }
}

void MouseMoveEvent(GLFWwindow* window, double xpos, double ypos) {
    // 处理鼠标移动
}
```

### 字符输入
```cpp
void CharEvent(GLFWwindow* window, unsigned int codepoint) {
    // 处理Unicode字符输入
}
```

---

## 调试技巧速查

### 启用调试日志
```cpp
// 在代码中
LOG_DEBUG("debug message");

// 或修改日志级别
logger->setLevel(LogLevel::DEBUG);
```

### OpenGL错误检查
```cpp
#define GL_CHECK_ERROR() \
    { \
        GLenum err = glGetError(); \
        if(err != GL_NO_ERROR) { \
            LOG_ERROR("OpenGL Error: %d", err); \
        } \
    }
```

### 变量监控（VS Code调试）
1. 按F5启动调试
2. 点击行号左边设置断点
3. 使用Watch窗口添加变量监控
4. F10逐步执行，F5继续

---

## 文件类型支持速查

### 图片格式
- WebP（推荐，文件小）
- BMP
- PNG（需转换）

### 音频格式
- WAV
- MP3
- FLAC
- OGG

### 视频格式
- MP4（H.264）
- MKV
- WebM
- MOV

### 文本编码
- UTF-8（推荐）
- GBK/GB2312
- 其他单字节编码

### 配置格式
- JSON（project配置）

### 本地化
- JSON（语言文件）

---

## 目录关键文件

| 目录 | 关键文件 | 用途 |
|------|---------|------|
| `src/` | main.cpp | 程序入口 |
| `src/core/` | explorer.cpp | 核心控制 |
| `src/core/render/` | Renderer.cpp | 渲染系统 |
| `src/core/baseItem/` | 各类实现 | 媒体和UI |
| `src/core/screen/` | 各屏幕.cpp | UI屏幕 |
| `include/` | 对应头文件 | API声明 |
| `files/localization/` | *.json | 多语言 |
| `files/names/` | *.txt | 名字库 |
| `examples/` | test*.cpp | 测试代码 |
| `docs/` | *.md | 特性文档 |

---

## 错误排查速查

| 症状 | 可能原因 | 解决方法 |
|------|---------|--------|
| 编译错误：undefined reference | 缺少链接库 | 检查CMakeLists.txt的target_link_libraries |
| 运行崩溃 | 空指针或资源未加载 | 使用调试器检查变量 |
| 黑屏 | OpenGL驱动问题 | 更新显卡驱动 |
| 中文乱码 | 编码问题 | 确保文件编码为UTF-8 BOM |
| 音视频无法播放 | FFmpeg问题 | 重新安装FFmpeg依赖 |
| 图片无法加载 | 路径或格式问题 | 检查文件路径和格式支持 |

---

## 版本和构建信息

- **CMake最低版本：** 3.15
- **C++标准：** C++17
- **Windows编译器：** Visual Studio 2019+
- **macOS编译器：** Xcode 12+
- **Linux编译器：** GCC/Clang（最新版）

---

## 快速清单

### 开始新功能前
- [ ] 查阅相关文档
- [ ] 运行项目确认环境OK
- [ ] 创建新分支（如使用Git）
- [ ] 参考CODE_STYLE_GUIDE.md

### 完成功能后
- [ ] 编译无错误和警告
- [ ] 测试功能正常
- [ ] 遵循代码风格
- [ ] 更新相关文档

---

## English Version

### Quick Project Info
- **Name:** Genshin Name Picker
- **Language:** C++17
- **Build:** CMake + VCPKG
- **Platforms:** Windows, macOS, Linux

### Common Commands
```bash
# Build
cmake -B out/build -G "Visual Studio 17 2022" \
  -DCMAKE_TOOLCHAIN_FILE=<VCPKG>/scripts/buildsystems/cmake/vcpkg.cmake
cmake --build out/build --config Release

# Debug (VS Code)
F5  # Start debug
Ctrl+Shift+B  # Build
```

### Key Classes
- `Explorer` - Main controller
- `Renderer` - OpenGL rendering
- `Screen` - Base UI screen class
- `Config` - Configuration manager
- `Audio`, `VideoPlayer`, `Bitmap` - Media handlers

### Naming Rules
- Classes: PascalCase (`MyClass`)
- Functions: camelCase (`myFunction()`)
- Constants: SCREAMING_SNAKE_CASE (`MAX_SIZE`)
- Files: PascalCase (`MyClass.cpp`)

### Common Code Patterns
```cpp
// Logging
LOG_DEBUG("message");
LOG_ERROR("error");

// Loading audio
Audio audio; audio.load("file.wav"); audio.play();

// Getting config
config->get("key", "default");

// Getting localized text
lang::get("ui_key");

// Screen switching
explorer->switchScreen("name");
```

### Supported Formats
- **Images:** WebP, BMP, PNG
- **Audio:** WAV, MP3, FLAC, OGG
- **Video:** MP4, MKV, WebM, MOV
- **Encoding:** UTF-8 (recommended)

