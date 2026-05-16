# 架构设计说明 / Architecture Overview

## 中文版本

### 高层架构

```
  ┌─────────────────────────────────────────┐
  │                                Application Layer                                               │
  │                     (mainloop, UI Screens, Input Handling)                            │
  └────────────────┬────────────────────────┘
                                             │
                           ┌──────┴───────┐
                           │                                  │
    ┌────────▼─────┐  ┌──────▼────────┐
    │ Core System                 │  │ Render System                │
    │ - Explorer                    │  │ - OpenGL                       │
    │ - Config                       │  │ - Shaders                       │
    │ - Log                           │  │ - Textures                      │
    └────────┬─────┘  └──────┬────────┘
                           │                                  │
                          └──────┬───────┘
                                            │
                      ┌────────▼──────────┐
                      │ Base Items Layer                       │
                      │ - Audio, Video                           │
                      │ - Bitmap, Font                          │
                      │ - Button                                  │
                      └────────┬──────────┘
                                             │
                      ┌────────▼──────────┐
                      │External Libraries                       │
                      │ - GLFW, SDL2                            │   
                      │ - FFmpeg, Curl                          │
                      │ - FreeType                                │
                      └───────────────────┘
```

---

## 核心模块详解

### 1. 主循环 (Main Loop)

**文件**: `src/mainloop.cpp`, `include/mainloop.h`

职责：
- GLFW窗口管理
- 事件处理（键盘、鼠标、字符输入）
- 帧率控制和FPS统计
- 异常恢复

关键函数：
- `mainloop()` - 主循环入口
- `executeRenderingWithRecovery()` - 渲染与恢复
- 事件回调函数（KeyEvent, MouseButtonEvent等）

---

### 2. Explorer - 核心应用控制器

**文件**: `src/core/explorer.cpp`, `include/core/explorer.h`

职责：
- 资源加载和管理（位图、字体、音频、视频）
- 屏幕切换和管理
- 应用全局状态控制

关键类：
- `Explorer` - 主控制器
- `BitmapID` - 位图资源枚举
- `FontID` - 字体资源枚举

---

### 3. 配置系统 (Configuration)

**文件**: `src/core/baseItem/Config.cpp`, `include/core/Config.h`

职责：
- 加载/保存用户配置
- 管理配置项（名字文件路径、UI设置等）
- 读取和应用设置

关键类：
- `Config` - 配置管理器
- `ConfigItem` - 单个配置项

---

### 4. 渲染系统 (Rendering System)

**核心文件**: `src/core/render/`

#### 设计模式

采用**Command Pattern**和**Facade Pattern**：

```
Application
    ↓
  Renderer (Facade)
    ↓
  RenderAPI
    ↓
  OpenGL (GLBase)
```

#### 主要类

| 类 | 职责 |
|-----|------|
| `Renderer` | 高级渲染接口，管理渲染队列 |
| `RenderAPI` | 渲染API抽象层 |
| `GLBase` | OpenGL核心初始化 |
| `Shader` | 着色器管理（编译、链接、使用） |
| `Texture` | 纹理管理（加载、绑定） |
| `VertexArray` | VAO管理 |
| `VertexBuffer` | VBO管理 |
| `IndexBuffer` | EBO管理 |
| `OpenGLFontRenderer` | 字体渲染（贴图方式） |

#### 渲染流程

```
1. 初始化 OpenGL + 着色器
2. 加载纹理和字体
3. 每帧：
   - 清空缓冲
   - 收集渲染命令
   - 执行渲染
   - 交换缓冲
```

---

### 5. UI系统 (Screen/UI System)

**文件**: `src/core/screen/`, `include/core/screen/`

#### 屏幕层级

```
Screen (Base Class)
├── MainScreen      (主屏幕 - 点名功能)
├── NameScreen      (名字选择屏幕)
├── ListNameScreen  (名字列表屏幕)
├── SettingScreen   (设置屏幕)
└── VideoScreen     (视频播放屏幕)
```

#### 屏幕基类 (base.h)

提供：
- UI元素生命周期管理
- 输入事件处理
- 页面切换逻辑

每个屏幕实现：
- 初始化
- 更新逻辑
- 渲染
- 事件处理
- 清理

---

### 6. 媒体系统 (Media System)

**文件**: `src/core/baseItem/`

#### Base Item层级

```
Base (虚基类)
├── Bitmap     (图片)
├── Font       (字体)
├── Button     (按钮)
├── Audio      (音频)
└── VideoPlayer(视频)
```

#### 关键功能

- **Bitmap** - WebP/BMP图片加载和渲染
- **Font** - FreeType字体处理和渲染
- **Audio** - SDL2音频播放
- **VideoPlayer** - FFmpeg视频解码和播放
- **Button** - UI按钮交互逻辑

---

### 7. 国际化系统 (Localization)

**文件**: `src/core/baseItem/lang.cpp`, `files/localization/`

支持的语言：
- 简体中文 (zh-CN)
- 繁体中文 (zh-TW)
- 日文 (ja-JP)
- 英文 (en-US)

字符串存储在JSON文件中，通过Key-Value方式访问。

---

### 8. 随机名字选择 (Name Randomizer)

**文件**: `src/core/baseItem/nameRandomer.cpp`

职责：
- 加载名字库
- 解析名字的等级和类型
- 随机选择名字
- 支持加权选择（等级权重）

---

### 9. 升级系统 (Update System)

**文件**: `src/upgrade/`

功能：
- 检查更新
- 下载升级包（Curl）
- 解压升级文件（MiniZip）
- 应用更新

组件：
- `web.cpp` - 网络通信
- `unzip.cpp` - 解压功能
- `upgrade.cpp` - 升级逻辑

---

## 数据流

### 启动流程

```
main()
  ↓
init()  [初始化GLFW/OpenGL/资源]
  ↓
mainloop()
  ├─ 事件处理 (键盘/鼠标)
  ├─ 屏幕更新
  ├─ 渲染
  └─ 帧率控制
  ↓
cleanup() [清理资源]
```

### 事件处理流

```
GLFW Event
  ↓
回调函数 (KeyEvent, MouseButtonEvent等)
  ↓
Explorer 分发事件
  ↓
当前 Screen 处理事件
  ↓
UI 元素更新/回调
```

### 渲染流

```
每帧开始
  ↓
清空帧缓冲
  ↓
Explorer 调用当前Screen的render()
  ↓
Screen 调用UI元素的render()
  ↓
Renderer 收集渲染命令
  ↓
RenderAPI 执行OpenGL调用
  ↓
交换缓冲
```

---

## 关键设计模式

| 模式 | 使用位置 | 目的 |
|------|--------|------|
| **Singleton** | Explorer, Logger | 全局唯一实例 |
| **Factory** | 资源加载 | 创建各类资源 |
| **Observer** | 事件系统 | 事件回调 |
| **Command** | 渲染系统 | 渲染命令队列 |
| **Template Method** | Screen基类 | 屏幕生命周期 |
| **Strategy** | 字体渲染 | 不同渲染方式 |

---

## 性能考虑

1. **渲染优化**
   - 使用VAO/VBO缓存顶点数据
   - 着色器预编译
   - 纹理缓存

2. **资源管理**
   - 懒加载（按需加载资源）
   - 资源引用计数（防止泄漏）

3. **帧率控制**
   - FPS限制（避免过度渲染）
   - 可配置帧率

---

## English Version

### System Architecture

The application follows a **layered architecture** pattern:

- **Application Layer** - Screen management, event handling, main loop
- **Core Layer** - Explorer (resource manager), Config system, Logger
- **Rendering Layer** - OpenGL abstraction with Shader, Texture, Buffer management
- **Media Layer** - Audio, Video, Font, Bitmap handling
- **Library Layer** - GLFW, SDL2, FFmpeg, Curl, FreeType

### Key Components

1. **Explorer** - Central hub for resource management and screen control
2. **Renderer** - Abstraction over OpenGL with command pattern
3. **Screen System** - Hierarchical UI screens with base class pattern
4. **Config Manager** - Persistent configuration storage
5. **Localization** - Multi-language support via JSON
6. **Update System** - Auto-update with network and zip support

### Design Principles

- **Separation of Concerns** - Each module has clear responsibility
- **Abstraction** - Render API abstracts OpenGL details
- **Reusability** - Base classes for common functionality
- **Extensibility** - Easy to add new screens or UI elements

