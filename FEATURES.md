# 功能说明 / Features Documentation

## 中文版本

### 核心功能

#### 1. 随机点名 (Random Name Selection)

**描述**: 从名单中随机选择一个名字，支持带特效的展示

**相关文件**:
- `src/core/baseItem/nameRandomer.cpp` - 名字选择逻辑
- `src/core/screen/mainScreen.cpp` - 点名界面

**功能特性**:
- 支持自定义名字文件
- 支持等级系统（3★、4★、5★、6★）
- 支持武器类型背景（剑、法器、双手剑、长矛）
- 加权随机选择（等级权重）

**使用方式**:
```
名字格式: 名字+等级+类型
示例: 张三5 (等级5，类型默认)
      李四63 (等级6，双手剑)
```

---

#### 2. 自定义名字库

**描述**: 支持用户导入自己的名字列表

**相关文件**:
- `files/names/` - 默认名字库位置
- `src/core/baseItem/nameRandomer.cpp` - 文件解析逻辑

**支持格式**:
- 纯文本文件（.txt）
- 支持多种编码（GBK、UTF-8等）
- 每行一个名字
- 支持注释行（`#`开头）

**配置位置**:
- 通过"设置"屏幕选择名字文件

---

#### 3. 自定义背景图片

**描述**: 支持用户设置自定义的背景图片

**相关文件**:
- `src/core/baseItem/Bitmap.cpp` - 图片加载
- `src/core/Config.cpp` - 配置保存

**支持格式**:
- WebP（推荐，更小的文件大小）
- BMP
- PNG（需转换）

**图片转换工具**:
- `files/imgs/webp to bmp.py` - Python脚本，用于WebP到BMP的转换

---

#### 4. 自定义音效

**描述**: 支持自定义背景音乐和点名音效

**相关文件**:
- `src/core/baseItem/Audio.cpp` - 音频播放
- `include/core/baseItem/Audio.h` - 音频接口

**支持格式**:
- WAV
- MP3
- FLAC
- OGG

**功能**:
- 背景音乐循环播放
- 点名音效
- 音量控制

---

#### 5. 视频播放

**描述**: 支持在特定场景播放视频

**相关文件**:
- `src/core/baseItem/VideoPlayer.cpp` - 视频解码和播放
- `src/core/screen/VideoScreen.cpp` - 视频展示界面

**技术**:
- 使用FFmpeg进行视频解码
- 支持H.264/H.265等主流编码
- 实时硬件加速（如果可用）

**支持格式**:
- MP4（H.264）
- MKV
- WebM
- MOV

---

### UI和交互功能

#### 6. 多屏幕导航

**屏幕列表**:
- **MainScreen** - 点名主屏幕
- **NameScreen** - 名字选择和显示
- **ListNameScreen** - 名字列表浏览
- **SettingScreen** - 设置界面
- **VideoScreen** - 视频播放界面

**相关文件**:
- `src/core/screen/` - 屏幕实现
- `src/core/explorer.cpp` - 屏幕管理和切换

---

#### 7. 设置系统

**配置项目**:
- 名字文件路径
- 背景图片路径
- 音乐文件路径
- 音量
- 语言选择
- 显示选项

**相关文件**:
- `src/core/Config.cpp` - 配置管理
- `src/core/screen/SettingScreen/` - 设置界面

**存储位置**:
- Windows: `%APPDATA%/Random/config.json`（或项目目录）

---

#### 8. 编辑模式

**描述**: UI元素可拖动编辑，保持位置设置

**相关文件**:
- `docs/ui_edit_mode_integration.md` - 详细说明
- `src/core/screen/` - 编辑模式实现

**功能**:
- 拖动移动UI元素
- 保存布局配置
- 恢复默认布局

---

#### 9. 按钮系统

**支持功能**:
- 标准点击按钮
- 可编辑按钮（可配置）
- 各种样式（颜色、大小、形状）

**相关文件**:
- `src/core/baseItem/Button.cpp`
- `docs/additional_editable_buttons_solution.md` - 可编辑按钮说明

---

### 国际化 (Internationalization)

#### 10. 多语言支持

**支持的语言**:
- 🇨🇳 简体中文 (zh-CN)
- 🇭🇰 繁体中文 (zh-TW)
- 🇯🇵 日文 (ja-JP)
- 🇬🇧 英文 (en-US)

**相关文件**:
- `files/localization/*.json` - 语言文件
- `src/core/baseItem/lang.cpp` - 语言加载和查询

**字符串格式**:
```json
{
  "key_name": "显示文本",
  "button_start": "开始"
}
```

---

### 系统功能

#### 11. 自动更新

**描述**: 应用程序可自动检查和安装更新

**相关文件**:
- `src/upgrade/upgrade.cpp` - 升级逻辑
- `src/upgrade/web.cpp` - 网络下载
- `src/upgrade/unzip.cpp` - 解压安装

**工作流**:
1. 检查更新（HTTP请求）
2. 下载升级包
3. 验证完整性
4. 解压并安装
5. 重启应用

---

#### 12. 日志系统

**描述**: 记录应用运行日志，便于调试

**相关文件**:
- `src/core/log.cpp` - 日志实现
- `include/core/log.h` - 日志接口

**功能**:
- 多级日志（DEBUG, INFO, WARN, ERROR）
- 时间戳记录
- 文件输出
- 控制台输出

**日志位置**:
- Windows: `%APPDATA%/Random/logs/`
- Linux/macOS: `~/.config/Random/logs/`

---

#### 13. 宽高比锁定 (Aspect Ratio Snap)

**描述**: 自动调整窗口大小以符合特定宽高比

**相关文件**:
- `docs/aspect_ratio_snap_feature.md` - 特性说明
- `docs/aspect_ratio_snap_fix_verification.md` - 验证文档

**用途**:
- 保持UI布局完整
- 适配不同屏幕分辨率

---

#### 14. 虚拟键盘 (Virtual Keyboard)

**描述**: 跨平台虚拟键盘支持

**相关文件**:
- `docs/virtual_keyboard_crossplatform.md` - 跨平台实现

**用途**:
- 触摸屏支持
- 移动设备兼容

---

#### 15. UTF-8处理修复

**描述**: 解决了UTF-8文本处理的崩溃问题

**相关文件**:
- `docs/utf8_crash_fix.md` - 修复详情
- `src/core/baseItem/lang.cpp` - UTF-8处理

**问题解决**:
- 正确处理多字节字符
- 避免缓冲区溢出
- 支持各种文本编码转换

---

### 高级功能

#### 16. 视频完成检测

**描述**: 检测视频播放完成并触发相应事件

**相关文件**:
- `docs/video_completion_improvement.md` - 改进说明
- `src/core/baseItem/VideoPlayer.cpp` - 视频完成检测

**应用场景**:
- 视频播放完成后自动切换屏幕
- 触发后续动画或音效

---

#### 17. 区域配置

**描述**: 支持屏幕区域配置和布局管理

**相关文件**:
- `docs/region_config_usage.md` - 使用说明
- `include/core/baseItem/configItem.h` - 配置数据结构

**配置内容**:
- UI元素位置和大小
- 交互区域定义
- 布局预设

---

### 性能和优化

#### 18. 帧率控制

**特性**:
- 可配置帧率上限
- FPS实时显示
- 渲染优化

**相关文件**:
- `src/mainloop.cpp` - 帧率控制
- `include/mainloop.h` - FPS变量

---

#### 19. 资源缓存

**特性**:
- 位图和纹理缓存
- 字体缓存
- 音频预加载

**相关文件**:
- `src/core/explorer.cpp` - 资源管理器

---

## English Version

### Main Features

1. **Random Name Selection** - Select names from custom lists with tier system
2. **Custom Images** - Set custom background images (WebP, BMP)
3. **Audio Support** - Background music and sound effects
4. **Video Playback** - Display video content with FFmpeg
5. **Multi-Screen UI** - Multiple interface screens with navigation
6. **Settings System** - Configurable options
7. **Localization** - Support for EN, JP, ZH-CN, ZH-TW
8. **Auto-Update** - Automatic program updates
9. **Logging** - Debug logging system
10. **Edit Mode** - UI element positioning and layout
11. **Virtual Keyboard** - Cross-platform touch support
12. **Video Completion** - Detect video end and trigger events
13. **Region Config** - Screen layout and positioning
14. **Aspect Ratio Snap** - Auto-fit UI layout
15. **UTF-8 Support** - Proper Unicode text handling
16. **Frame Rate Control** - Performance optimization
17. **Resource Caching** - Efficient resource management
18. **Button System** - Various button types and styles

### File Format Support

- **Images**: WebP, BMP, PNG (via conversion)
- **Audio**: WAV, MP3, FLAC, OGG
- **Video**: MP4, MKV, WebM, MOV
- **Names**: Text files (.txt) with UTF-8 encoding
- **Config**: JSON format
- **Localization**: JSON files

