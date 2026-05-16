# 项目结构说明 / Project Structure

## 中文版本

### 顶级目录结构

```
random_name/
├── src/                 # 源代码实现
├── include/             # 头文件
├── examples/            # 示例代码
├── docs/                # 文档（特性、修复说明等）
├── files/               # 资源文件（字体、图片、音乐、视频、名字、本地化）
├── installer/           # 安装程序配置
├── upgrade/             # 升级功能相关代码
├── CMakeLists.txt       # CMake构建配置
├── CMakePresets.json    # CMake预设
├── CMakeSettings.json   # CMake设置
├── vcpkg.json           # VCPKG依赖配置
├── README.md            # 主要说明文档
└── build_number.txt     # 构建号记录
```

---

## 核心模块说明

### 1. `src/` - 源代码目录

#### `src/main.cpp`
- 程序入口点
- 初始化应用程序

#### `src/mainloop.cpp`
- 主循环实现
- 渲染和事件处理循环
- 帧率控制

#### `src/core/` - 核心模块

##### `baseItem/` - 基础元素
- **Audio.cpp/h** - 音频播放功能
- **Bitmap.cpp/h** - 位图/图片处理
- **Button.cpp/h** - UI按钮控件
- **Font.cpp/h** - 字体处理
- **VideoPlayer.cpp/h** - 视频播放功能
- **Base.cpp/h** - 基础类（所有UI元素的父类）
- **lang.cpp/h** - 语言/国际化支持
- **nameRandomer.cpp/h** - 随机名字选择逻辑
- **Config.cpp/h & ConfigItem.cpp/h** - 配置管理

##### `render/` - 渲染系统
- **Renderer.cpp/h** - 主渲染器
- **GLBase.cpp/h** - OpenGL基础
- **RenderAPI.cpp/h** - 渲染API抽象
- **OpenGLFontRenderer.cpp/h** - 字体渲染（OpenGL）
- **Shader.cpp/h** - 着色器管理
- **Texture.cpp/h** - 纹理管理
- **VertexBuffer.cpp/h** - 顶点缓冲
- **VertexArray.cpp/h** - 顶点数组
- **IndexBuffer.cpp/h** - 索引缓冲

##### `screen/` - 屏幕/UI
- **base.cpp/h** - 屏幕基类
- **mainScreen.cpp/h** - 主屏幕（点名功能）
- **NameScreen.cpp/h** - 名字选择屏幕
- **ListNameScreen.cpp/h** - 名字列表屏幕
- **SettingScreen/** - 设置屏幕及子屏幕
- **VideoScreen.cpp/h** - 视频播放屏幕

##### 其他核心文件
- **explorer.cpp/h** - 核心应用逻辑和资源管理
- **explorerInit.cpp** - 初始化逻辑
- **Drawer.cpp/h** - 绘制辅助
- **log.cpp/h** - 日志系统

---

### 2. `include/` - 头文件目录

与 `src/` 结构相对应，包含所有类和函数的声明。

**特殊文件：**
- **version.h.in** - 版本信息模板（CMake生成）
- **mainloop.h** - 主循环接口
- **core/** - 核心模块头文件

---

### 3. `files/` - 资源文件目录

#### `files/fonts/`
- 项目使用的字体文件

#### `files/imgs/`
- UI图片资源
- Python脚本：WebP到BMP的转换工具

#### `files/localization/`
- **en-US.json** - 英文
- **ja-JP.json** - 日文
- **zh-CN.json** - 简体中文
- **zh-TW.json** - 繁体中文

#### `files/names/`
- **1.txt, 2.txt, 3.txt, 4.txt** - 可选的名字库文件

#### `files/music/` 和 `files/video/`
- 音乐和视频资源（如适用）

---

### 4. `examples/` - 示例代码

包含各个功能模块的使用示例和测试代码：
- `edit_mode_example.cpp` - 编辑模式示例
- `video_completion_example.cpp` - 视频完成逻辑
- `region_config_example.cpp` - 区域配置
- `audio_example.cpp` - 音频使用
- 其他功能示例

---

### 5. `docs/` - 文档

记录了各个特性和修复的详细说明：
- **utf8_crash_fix.md** - UTF-8处理修复
- **aspect_ratio_snap_feature.md** - 宽高比锁定特性
- **video_completion_improvement.md** - 视频完成功能改进
- **ui_edit_mode_integration.md** - UI编辑模式
- 其他特性和修复文档

---

### 6. `upgrade/` - 升级模块

- **upgrade.cpp** - 升级逻辑
- **web.cpp/h** - 网络请求（下载升级包）
- **unzip.cpp/h** - 解压功能
- **log.cpp/h** - 升级过程日志

---

### 7. `installer/` - 安装程序

- **common.iss** - Inno Setup安装程序脚本

---

## 编译相关文件

### CMakeLists.txt
- 定义CMake构建规则
- 配置依赖项（VCPKG）
- 生成版本信息

### CMakePresets.json
- 提供预定义的CMake配置预设
- 支持不同的构建类型

### CMakeSettings.json
- Visual Studio CMake集成设置

### vcpkg.json
- 项目依赖：
  - **图形/渲染**：glad, glfw3, glm, SDL2
  - **字体**：freetype
  - **图片**：libwebp, stb
  - **媒体**：ffmpeg（含音视频支持）
  - **其他**：curl（网络), minizip（压缩), nlohmann-json（JSON), utfcpp（UTF-8处理）

---

## 文件说明

| 文件 | 用途 |
|------|------|
| `build_number.txt` | 记录构建版本号 |
| `app.rc.in` | 应用程序资源文件模板 |
| `upgrade.rc.in` | 升级程序资源文件模板 |
| `test_format_fix.cpp` | 格式修复测试代码 |

---

## English Version

### Directory Structure

The project follows a modular architecture:

- **src/** - Source code implementation
- **include/** - Header files (declarations)
- **files/** - Resources (fonts, images, localization, audio, video)
- **examples/** - Example code and tests
- **docs/** - Feature and fix documentation
- **installer/** - Installer configuration
- **upgrade/** - Auto-upgrade functionality

### Core Components

1. **Rendering System** (render/) - OpenGL-based 2D rendering
2. **UI System** (screen/) - Multiple screen interfaces (Main, Settings, Video)
3. **Media System** (baseItem/) - Audio, video, bitmap, and font handling
4. **Configuration** - Settings management and persistence
5. **Localization** - Multi-language support (EN, JA, ZH-CN, ZH-TW)
6. **Network/Update** - Auto-update functionality via curl and minizip

### Build System

- **CMake** - Primary build system
- **VCPKG** - Package manager for dependencies
- **Inno Setup** - Windows installer

