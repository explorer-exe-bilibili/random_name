# 构建指南 / Build Guide

## 中文版本

### 系统要求

#### 硬件要求
- **处理器**: x86-64（Intel/AMD）
- **内存**: 至少2GB
- **存储空间**: 构建期间至少5GB（依赖编译）

#### 软件要求

| 软件 | 版本 | 用途 |
|------|------|------|
| CMake | ≥ 3.15 | 构建系统 |
| VCPKG | 最新 | 包管理器 |
| Visual Studio | 2019+ | C++编译器（Windows）或其他编译器 |
| Xcode | 12+ | 编译器（macOS） |
| GCC/Clang | 最新 | 编译器（Linux） |

---

### 依赖配置

项目依赖在 `vcpkg.json` 中定义：

```json
"dependencies": [
  "freetype",        // 字体处理
  "glad",            // OpenGL加载器
  "glfw3",           // 窗口和输入
  "glm",             // 数学库
  "libwebp",         // WebP图片格式
  "nlohmann-json",   // JSON处理
  "sdl2",            // 音视频基础
  "stb",             // 图片处理库
  "utfcpp",          // UTF-8处理
  "curl",            // HTTP请求
  "minizip",         // ZIP解压
  "ffmpeg"           // 音视频编码解码
]
```

---

### 构建步骤

#### 方案1：使用VS Code集成（推荐）

1. **安装依赖**
   - 安装 CMake、VCPKG
   - VS Code安装 CMake Tools 扩展

2. **配置项目**
   ```
   - 打开项目文件夹
   - 选择工具包（如 Visual Studio 17 2022）
   - 选择构建类型（Debug/Release）
   ```

3. **构建**
   ```
   - Ctrl+Shift+B 调出构建任务
   - 选择构建目标或使用快捷键
   ```

4. **运行**
   - 按 F5 调试运行
   - 或使用工具栏运行按钮

#### 方案2：命令行构建

```bash
# 进入项目目录
cd random_name

# 生成构建目录
cmake -B out/build -G "Visual Studio 17 2022" \
  -DCMAKE_TOOLCHAIN_FILE=<VCPKG_ROOT>/scripts/buildsystems/cmake/vcpkg.cmake

# 构建
cmake --build out/build --config Debug

# 或发布版本
cmake --build out/build --config Release
```

#### 方案3：针对Linux/macOS

```bash
# Linux/macOS
cmake -B out/build -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE=<VCPKG_ROOT>/scripts/buildsystems/cmake/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release

cmake --build out/build --parallel $(nproc)
```

---

### CMake选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `USE_SYSTEM_DEPS` | OFF | 使用系统安装的依赖而非VCPKG |
| `VCPKG_TARGET_TRIPLET` | 自动 | 构建目标平台（x64-windows等） |

---

### 构建问题排查

#### 问题1：VCPKG依赖下载失败

**原因**：网络连接问题或包源不可用

**解决**：
```bash
# 更新VCPKG
git -C <VCPKG_ROOT> pull

# 清理并重新构建
cmake --build out/build --target clean
```

#### 问题2：OpenGL驱动问题

**症状**：运行时渲染错误或黑屏

**解决**：
- 更新显卡驱动
- 确保支持OpenGL 3.3+

#### 问题3：UTF-8编码问题

**症状**：中文显示为乱码

**解决**：
- 确保源文件编码为UTF-8 BOM
- 检查 `utfcpp` 依赖是否正确安装

#### 问题4：FFmpeg库链接失败

**症状**：编译时找不到FFmpeg

**解决**：
```bash
# 强制重新安装FFmpeg
vcpkg install ffmpeg:x64-windows --recurse
```

---

### 输出文件

构建完成后，输出文件位置：

```
out/build/
├── Debug/
│   ├── random.exe          # 主程序（调试版）
│   └── upgrade.exe         # 升级程序（调试版）
└── Release/
    ├── random.exe          # 主程序（发布版）
    └── upgrade.exe         # 升级程序（发布版）
```

---

### 版本号管理

版本信息从以下来源生成：
- `CMakeLists.txt` 中定义的版本
- `build_number.txt` 中的构建号
- CMake模板（`.in`文件）自动生成实际的头文件

---

### CI/CD 集成

项目支持GitHub Actions构建（见CMakeLists.txt中的检测）。

在GitHub Actions中：
- 使用 `x64-windows-static` 三元组用于静态链接
- 自动化测试和打包

---

## English Version

### System Requirements

#### Hardware
- x86-64 processor (Intel/AMD)
- Minimum 2GB RAM
- 5GB+ free space during build

#### Software
- CMake ≥ 3.15
- VCPKG (latest)
- Visual Studio 2019+ (Windows), Xcode 12+ (macOS), or GCC/Clang (Linux)

### Build Process

#### Using VS Code (Recommended)

1. Open project folder
2. Select C++ compiler kit
3. Select build type (Debug/Release)
4. Press Ctrl+Shift+B to build
5. Press F5 to debug run

#### Command Line

```bash
# Generate build
cmake -B out/build -G "Visual Studio 17 2022" \
  -DCMAKE_TOOLCHAIN_FILE=<VCPKG_ROOT>/scripts/buildsystems/cmake/vcpkg.cmake

# Build
cmake --build out/build --config Release
```

### Build Outputs

- **random.exe** - Main application
- **upgrade.exe** - Auto-updater program

### Troubleshooting

| Issue | Solution |
|-------|----------|
| VCPKG download fails | Update VCPKG and check network |
| OpenGL errors | Update graphics drivers |
| Chinese text garbled | Ensure UTF-8 encoding |
| FFmpeg not found | Reinstall: `vcpkg install ffmpeg:x64-windows` |

