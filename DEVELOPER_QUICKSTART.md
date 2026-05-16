# 开发者速成手册 / Developer Quick Start

## 中文版本

欢迎加入开发团队！这个手册能帮助你快速上手。

---

## 🚀 5分钟快速开始

### 1. 环境检查
```bash
# 检查CMake版本（需要3.15+）
cmake --version

# 检查VCPKG是否安装
vcpkg --version
```

### 2. 打开项目
- 用VS Code打开项目文件夹
- 自动安装推荐的扩展（C/C++, CMake Tools）

### 3. 首次构建
```bash
# Ctrl+Shift+B 或在命令行运行
cmake --build out/build --config Debug
```

### 4. 运行程序
```bash
# F5开始调试，或双击运行
./out/build/Debug/random.exe
```

---

## 📚 推荐阅读顺序

### 第一周
1. **5分钟** - 阅读 [README.md](README.md)
2. **10分钟** - 浏览 [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) 了解目录
3. **15分钟** - 熟悉 [QUICK_REFERENCE.md](QUICK_REFERENCE.md)
4. **30分钟** - 阅读 [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md) 的命名规范部分

### 第二周
1. **30分钟** - 学习 [ARCHITECTURE.md](ARCHITECTURE.md) 的基本概念
2. **30分钟** - 研究 [FEATURES.md](FEATURES.md) 了解现有功能
3. **1小时** - 按 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) 做一个小任务

### 第三周
- 根据分配的任务查看相关文档
- 参与实际开发

---

## 🎯 你的第一个任务

### 任务：添加一个简单按钮

**目标：** 在主屏幕添加一个新按钮，点击时打印日志

**步骤：**

1. **打开主屏幕实现**
   ```
   编辑: src/core/screen/mainScreen.cpp
   ```

2. **在init()函数中添加按钮**
   ```cpp
   Button* testBtn = new Button(100, 100, 150, 50);
   testBtn->setText("Test Button");
   testBtn->setOnClick([this]() {
       LOG_INFO("Test button clicked!");
   });
   ```

3. **编译和测试**
   ```bash
   Ctrl+Shift+B  # 构建
   F5            # 运行和调试
   ```

4. **验证**
   - 运行程序看到新按钮
   - 点击按钮查看日志输出

**相关文档：**
- [DEVELOPMENT_GUIDE.md#添加新的UI元素-button](DEVELOPMENT_GUIDE.md#添加新的ui元素-button)
- [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md)

---

## 💡 常见任务快速指南

### "我想添加新的屏幕"

1. 查看：[PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) 了解屏幕结构
2. 参考：[DEVELOPMENT_GUIDE.md#添加新屏幕](DEVELOPMENT_GUIDE.md#添加新屏幕-screen)
3. 复制：参考 `mainScreen.cpp` 的结构
4. 实现：创建头文件和源文件
5. 注册：在 `explorer.cpp` 中注册新屏幕

**所需时间：** 1-2小时

### "我想加载自定义资源（图片/音乐）"

1. 查看：[FEATURES.md](FEATURES.md) 了解支持的格式
2. 参考：[QUICK_REFERENCE.md](#代码片段速查)
3. 使用：
   ```cpp
   Bitmap img; img.load("path"); img.render(x, y);
   Audio audio; audio.load("file.wav"); audio.play();
   ```

**所需时间：** 30分钟

### "我想添加多语言支持"

1. 编辑：`files/localization/zh-CN.json`
2. 添加键值对：`"ui_my_text": "显示文本"`
3. 代码中使用：`lang::get("ui_my_text")`

**所需时间：** 15分钟

### "我遇到编译错误"

1. 查看：[BUILD_GUIDE.md#构建问题排查](BUILD_GUIDE.md#构建问题排查)
2. 检查：
   - VCPKG依赖是否正确安装
   - CMakeLists.txt配置是否正确
   - 文件编码是否为UTF-8

**所需时间：** 30分钟

---

## 🐛 调试技巧

### 启用日志
```cpp
#include "core/log.h"

LOG_DEBUG("调试信息: %d", value);
LOG_ERROR("错误信息");
```

### 使用断点
- 点击行号左边设置断点（红点）
- F5启动调试
- 使用F10逐步执行
- 在变量面板查看值

### 检查OpenGL错误
```cpp
GL_CHECK_ERROR();  // 定义在项目中
```

---

## 📖 官方文档参考

| 文档 | 何时查阅 |
|------|---------|
| [README.md](README.md) | 了解项目概况 |
| [QUICK_REFERENCE.md](QUICK_REFERENCE.md) | 快速查找代码片段 |
| [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md) | 编码规范 |
| [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) | 具体开发任务 |
| [ARCHITECTURE.md](ARCHITECTURE.md) | 理解系统设计 |
| [FEATURES.md](FEATURES.md) | 了解现有功能 |
| [BUILD_GUIDE.md](BUILD_GUIDE.md) | 编译和构建问题 |

---

## ⚠️ 常见陷阱

### 陷阱1：文件编码问题
**症状：** 中文显示乱码  
**解决：** 在VS Code中设置文件编码为UTF-8 with BOM
```
文件 → 首选项 → 设置 → 编码 → 选择 UTF-8 with BOM
```

### 陷阱2：忘记include头文件
**症状：** 编译错误"undefined reference"  
**解决：** 检查必要的 `#include` 语句
```cpp
#include "core/log.h"          // 日志
#include "core/baseItem/Audio.h"  // 音频
```

### 陷阱3：内存泄漏
**症状：** 程序长时间运行后崩溃  
**解决：** 使用智能指针或确保cleanup()被调用
```cpp
std::unique_ptr<Resource> res(new Resource());  // 自动释放
```

### 陷阱4：窗口事件未处理
**症状：** UI按钮点击无反应  
**解决：** 在屏幕的事件处理函数中处理事件
```cpp
void MyScreen::handleMouseEvent(double x, double y, int button, int action) {
    // 检查按钮碰撞并调用回调
}
```

### 陷阱5：资源路径错误
**症状：** 图片或音频无法加载  
**解决：** 使用正确的相对路径或绝对路径
```cpp
// 相对于可执行文件目录
bitmap.load("../../files/imgs/background.webp");

// 或使用配置中的路径
std::string path = config->get("image_path");
```

---

## ✅ 代码提交前检查清单

在提交代码前，确保：

- [ ] **编译无错误**
  ```bash
  Ctrl+Shift+B 构建成功
  ```

- [ ] **编译无警告**
  ```bash
  查看"问题"面板确保没有黄色警告
  ```

- [ ] **遵循代码风格**
  ```
  查看 CODE_STYLE_GUIDE.md
  ```

- [ ] **添加必要的日志**
  ```cpp
  LOG_INFO("功能已启用");
  ```

- [ ] **测试基本功能**
  ```
  运行程序进行功能测试
  ```

- [ ] **更新相关文档**
  ```
  如果添加了新功能，更新 FEATURES.md
  ```

- [ ] **没有调试代码**
  ```cpp
  删除所有 std::cout 和临时调试代码
  ```

- [ ] **内存管理正确**
  ```
  使用智能指针或确保释放资源
  ```

---

## 🆘 获取帮助

### 我遇到问题了

1. **查阅文档** → [DOCUMENTATION_INDEX.md](DOCUMENTATION_INDEX.md)
2. **查看快速参考** → [QUICK_REFERENCE.md](QUICK_REFERENCE.md)
3. **搜索相关代码** → 查看 `examples/` 目录中的示例
4. **查看已有实现** → 参考类似功能的源代码

### 常见问题解答

**Q: 我不知道从哪里开始**  
A: 从[你的第一个任务](#-你的第一个任务)开始

**Q: 我的代码编译失败**  
A: 查看[BUILD_GUIDE.md](BUILD_GUIDE.md#构建问题排查)

**Q: 我不确定代码应该写在哪里**  
A: 查看[PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)和相关的 `src/` 目录

**Q: 我想知道某个类如何使用**  
A: 查看[QUICK_REFERENCE.md](QUICK_REFERENCE.md#代码片段速查)或相关的 `examples/`

**Q: 代码应该遵循什么规范**  
A: 参考[CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md)

---

## 📊 项目统计

- **总代码行数:** ~15,000+ 行C++代码
- **主要模块:** 10+
- **支持语言:** 4种（中文、繁体中文、日文、英文）
- **依赖库:** 10+个（FFmpeg、GLFW、SDL2等）
- **文档:** 8+ 个深度文档

---

## 🎓 学习资源

### C++相关
- [cppreference.com](https://en.cppreference.com) - C++参考
- [Learn C++ Online](https://www.learncpp.com/) - C++教程

### OpenGL相关
- [LearnOpenGL](https://learnopengl.com) - OpenGL入门教程
- [OpenGL Documentation](https://www.khronos.org/opengl/) - 官方文档

### CMake相关
- [CMake Official](https://cmake.org/documentation/) - 官方文档

---

## 🎉 成功标志

当你能够完成以下任务时，说明你已经掌握了基础知识：

- [ ] 成功编译和运行项目
- [ ] 在主屏幕添加一个按钮
- [ ] 加载并显示自定义图片
- [ ] 播放音频文件
- [ ] 在调试器中设置断点并检查变量
- [ ] 理解项目的屏幕架构
- [ ] 添加一个新的配置项

---

## 下一步

一旦你完成了上面的任务，你可以：

1. **添加新功能** - 参考[DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md)
2. **优化代码** - 参考[CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md)
3. **修复Bug** - 查看issue和pull requests
4. **改进文档** - 更新或扩展现有文档

---

## 📞 联系和反馈

如果你有：
- **问题** - 查阅相关文档
- **建议** - 提交issue
- **修复** - 提交pull request
- **文档改进** - 直接编辑markdown文件

---

## English Version

### Quick 5-Minute Setup

1. Check CMake version (need 3.15+)
2. Open project in VS Code
3. Build with Ctrl+Shift+B
4. Debug with F5

### First Week Reading

1. **README.md** (5 min) - Project overview
2. **PROJECT_STRUCTURE.md** (10 min) - Directory structure
3. **QUICK_REFERENCE.md** (15 min) - Common patterns
4. **CODE_STYLE_GUIDE.md** (30 min) - Naming rules

### Your First Task

Add a test button to the main screen:

1. Edit `src/core/screen/mainScreen.cpp`
2. Create a Button and set click callback
3. Build (Ctrl+Shift+B) and debug (F5)
4. Verify the button works

### Common Tasks

- **Add new screen** → DEVELOPMENT_GUIDE.md
- **Load images/audio** → QUICK_REFERENCE.md
- **Add localization** → DEVELOPMENT_GUIDE.md
- **Fix compile errors** → BUILD_GUIDE.md
- **Debug code** → DEVELOPMENT_GUIDE.md#调试

### Pre-Commit Checklist

- [ ] Compiles without errors
- [ ] Compiles without warnings
- [ ] Follows code style (CODE_STYLE_GUIDE.md)
- [ ] Has necessary logging
- [ ] Tested basic functionality
- [ ] Updated documentation
- [ ] No debug code left
- [ ] Memory properly managed

### Getting Help

1. Check **DOCUMENTATION_INDEX.md**
2. Search **QUICK_REFERENCE.md**
3. Look at **examples/** directory
4. Review similar source code

