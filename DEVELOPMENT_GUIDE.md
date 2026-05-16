# 开发指南 / Development Guide

## 中文版本

### 快速开始

#### 1. 项目设置

```bash
# 克隆仓库
git clone <repository-url>
cd random_name

# 安装依赖（首次）
# - 安装 CMake (https://cmake.org)
# - 安装 VCPKG (https://vcpkg.io)

# 配置VCPKG
set VCPKG_ROOT=<path-to-vcpkg>
```

#### 2. 开发环境配置

**推荐**：使用VS Code + CMake Tools

```
1. 打开VS Code
2. 安装扩展：
   - C/C++（Microsoft）
   - CMake Tools（Microsoft）
   - CMake（cmake language support）
3. 打开项目文件夹
4. 选择编译器工具链
5. 选择CMakePresets.json中的配置
```

#### 3. 首次构建

```bash
# VS Code中：
# Ctrl+Shift+B → 选择"build-upgrade" 或直接构建

# 或命令行：
cmake -B out/build -G "Visual Studio 17 2022" \
  -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/cmake/vcpkg.cmake
cmake --build out/build --config Debug
```

---

### 代码组织

#### 模块结构规范

每个主要模块遵循以下结构：

```
module_name/
├── Module.h          # 公共接口
├── Module.cpp        # 实现
└── SubModule/        # 子模块
    ├── SubModule.h
    └── SubModule.cpp
```

#### 命名规范

| 元素 | 规范 | 示例 |
|------|------|------|
| 类名 | PascalCase | `VideoPlayer`, `Renderer` |
| 函数 | camelCase或snake_case | `playAudio()`, `load_config()` |
| 变量 | camelCase或snake_case | `windowWidth`, `frame_count` |
| 常量 | SCREAMING_SNAKE_CASE | `MAX_FRAMES`, `DEFAULT_FONT_SIZE` |
| 枚举 | PascalCase | `BitmapID`, `ScreenType` |
| 宏 | SCREAMING_SNAKE_CASE | `GL_CHECK_ERROR()` |

#### 文件名规范

- 头文件：`.h`
- 源文件：`.cpp`
- 示例：CamelCase（如 `VideoPlayer.h`, `VideoPlayer.cpp`）

---

### 常见任务

#### 添加新屏幕 (Screen)

1. **创建头文件** `include/core/screen/NewScreen.h`

```cpp
#pragma once
#include "base.h"

class NewScreen : public Screen {
public:
    NewScreen();
    ~NewScreen();
    
    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void handleKeyEvent(int key, int action) override;
    void handleMouseEvent(double x, double y, int button, int action) override;
    void cleanup() override;
    
private:
    // 私有成员变量和方法
};
```

2. **创建实现文件** `src/core/screen/NewScreen.cpp`

```cpp
#include "core/screen/NewScreen.h"
#include "core/explorer.h"

NewScreen::NewScreen() { }

NewScreen::~NewScreen() { }

void NewScreen::init() {
    // 初始化屏幕资源
}

void NewScreen::update(float deltaTime) {
    // 更新逻辑
}

void NewScreen::render() {
    // 渲染逻辑
}

void NewScreen::handleKeyEvent(int key, int action) {
    // 键盘事件处理
}

void NewScreen::cleanup() {
    // 清理资源
}
```

3. **在Explorer中注册**

```cpp
// 在 explorer.cpp 中
NewScreen* newScreen = new NewScreen();
screens["new"] = newScreen;
```

4. **屏幕切换**

```cpp
// 从其他屏幕切换到新屏幕
explorer->switchScreen("new");
```

---

#### 添加新的UI元素 (Button)

1. **创建按钮实现** `src/core/screen/SomeScreen.cpp`

```cpp
// 在 init() 中
Button* myButton = new Button(x, y, width, height);
myButton->setText("Click Me");
myButton->setOnClick([this]() {
    // 按钮点击回调
    handleButtonClick();
});
addUIElement(myButton);
```

2. **处理回调**

```cpp
void handleButtonClick() {
    // 处理按钮点击逻辑
    explorer->switchScreen("next");
}
```

---

#### 添加新的配置项

1. **定义配置键** `include/core/Config.h`

```cpp
#define CONFIG_KEY_MY_SETTING "my_setting"
```

2. **读取配置** `src/core/baseItem/Config.cpp`

```cpp
std::string value = config->get(CONFIG_KEY_MY_SETTING, "default");
```

3. **保存配置**

```cpp
config->set(CONFIG_KEY_MY_SETTING, value);
config->save();
```

---

#### 添加新的本地化字符串

1. **编辑语言文件** `files/localization/zh-CN.json`

```json
{
  "ui_new_button": "新按钮",
  "ui_new_message": "你好，世界"
}
```

2. **代码中获取**

```cpp
#include "core/baseItem/lang.h"

std::string text = lang::get("ui_new_button");
```

---

#### 处理媒体资源

1. **加载位图**

```cpp
#include "core/baseItem/Bitmap.h"

Bitmap bitmap;
if(bitmap.load("path/to/image.webp")) {
    bitmap.render(x, y);
}
```

2. **播放音频**

```cpp
#include "core/baseItem/Audio.h"

Audio audio;
if(audio.load("path/to/sound.wav")) {
    audio.play();
}
```

3. **播放视频**

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

---

### 调试

#### 启用调试日志

**方法1：代码中**

```cpp
#include "core/log.h"

LOG_DEBUG("调试信息: %d", value);
LOG_INFO("信息: %s", message);
LOG_WARN("警告: %s", warning);
LOG_ERROR("错误: %s", error);
```

**方法2：运行时**

```cpp
// 在 mainloop.cpp 中修改日志级别
logger->setLevel(LogLevel::DEBUG);
```

#### 使用VS Code调试器

```
1. 按 F5 启动调试
2. 设置断点（点击行号左边）
3. 逐步执行（F10）或继续（F5）
4. 在变量面板查看变量值
```

#### OpenGL调试

```cpp
// 检查OpenGL错误
#define GL_CHECK_ERROR() \
    do { \
        GLenum err = glGetError(); \
        if(err != GL_NO_ERROR) { \
            LOG_ERROR("OpenGL Error: %d", err); \
        } \
    } while(false)
```

---

### 测试

#### 单元测试位置

测试代码位于 `examples/` 目录：

- `test_completion_logic.cpp` - 视频完成检测测试
- `region_1to1_test.cpp` - 区域配置测试
- 其他特性测试文件

#### 运行测试

```bash
# 编译测试
cmake --build out/build --target <test_name>

# 运行测试
./out/build/Debug/<test_name>
```

---

### 性能优化建议

#### 1. 渲染优化

```cpp
// 使用VAO缓存顶点数据
VertexArray vao;
VertexBuffer vbo(vertices);
vao.addBuffer(vbo, layout);
// 复用vao而不是每帧重新创建
```

#### 2. 资源缓存

```cpp
// 检查缓存中是否已加载
if(resourceCache.exists("key")) {
    return resourceCache.get("key");
}
// 加载新资源
auto resource = load("file");
resourceCache.set("key", resource);
```

#### 3. 帧率控制

```cpp
// 限制帧率避免过度渲染
float deltaTime = 1.0f / MAX_FPS;  // MAX_FPS = 60
```

#### 4. 内存管理

```cpp
// 使用智能指针避免内存泄漏
std::unique_ptr<Resource> resource(new Resource());
// 超出作用域自动释放
```

---

### 常见问题

| 问题 | 原因 | 解决 |
|------|------|------|
| 编译错误：未定义引用 | 遗漏链接库 | 检查CMakeLists.txt中的target_link_libraries |
| 运行时崩溃 | 空指针或资源未加载 | 使用调试器检查变量值 |
| 渲染异常 | OpenGL状态错误 | 使用GL_CHECK_ERROR()检查错误 |
| 中文显示乱码 | 编码问题 | 确保文件为UTF-8 BOM编码 |
| 音视频无法播放 | FFmpeg库问题 | 重新安装FFmpeg依赖 |

---

### 代码审查检查清单

提交代码前检查：

- [ ] 编译无错误和警告
- [ ] 命名规范符合要求
- [ ] 添加了必要的注释
- [ ] 没有遗留的调试代码
- [ ] 内存管理正确（无泄漏）
- [ ] 更新了相关文档
- [ ] 测试已通过
- [ ] 遵循项目代码风格

---

### 版本管理

#### 版本号格式

遵循语义化版本（Semantic Versioning）：
```
MAJOR.MINOR.PATCH
例如：1.2.3
```

#### 更新版本

1. 编辑 `CMakeLists.txt` 中的版本号
2. 更新 `build_number.txt`
3. 更新 `CHANGELOG.md`（如存在）

---

## English Version

### Quick Start

1. **Clone and Setup**
   ```bash
   git clone <url>
   cd random_name
   # Install CMake, VCPKG
   ```

2. **Configure in VS Code**
   - Install C/C++ and CMake Tools extensions
   - Select compiler and configuration
   - Configure CMake

3. **Build**
   - Ctrl+Shift+B to build
   - F5 to debug

### Code Standards

- **Classes**: PascalCase (e.g., `VideoPlayer`)
- **Functions**: camelCase (e.g., `playAudio()`)
- **Constants**: SCREAMING_SNAKE_CASE (e.g., `MAX_FRAMES`)
- **Files**: CamelCase (e.g., `VideoPlayer.cpp`)

### Common Tasks

#### Add New Screen
1. Create `NewScreen.h` inheriting from `Screen`
2. Implement required methods: init, update, render, cleanup
3. Register in Explorer
4. Implement screen switching logic

#### Add New Button
```cpp
Button* button = new Button(x, y, w, h);
button->setOnClick([](){ /* callback */ });
addUIElement(button);
```

#### Add Config Item
```cpp
config->set(KEY, value);
config->save();
```

#### Add Localization
```cpp
// files/localization/zh-CN.json
{ "key": "text" }

// In code
std::string text = lang::get("key");
```

### Debugging

- Press **F5** to start debug session
- Set breakpoints by clicking line numbers
- Use **LOG_DEBUG()** for logging
- Check **GL_CHECK_ERROR()** for OpenGL issues

### Performance Tips

- Cache resources and VAO objects
- Limit frame rate
- Use smart pointers for memory management
- Avoid creating objects in render loop

