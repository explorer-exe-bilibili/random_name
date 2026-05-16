# 项目文档索引 / Documentation Index

## 文档导航

这个文档提供了所有项目文档的快速导航和查找指南。

---

## 📚 完整文档列表

### 核心文档

| 文档 | 文件 | 用途 | 适合人群 |
|------|------|------|--------|
| **项目结构说明** | [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) | 了解项目目录和文件组织 | 所有开发者 |
| **架构设计** | [ARCHITECTURE.md](ARCHITECTURE.md) | 理解系统架构和模块设计 | 架构师、高级开发者 |
| **功能说明** | [FEATURES.md](FEATURES.md) | 查看所有功能和特性 | 功能开发者、测试人员 |
| **代码风格** | [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md) | 编码规范和最佳实践 | 所有开发者 |
| **开发指南** | [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) | 快速开始和常见任务 | 新开发者、贡献者 |
| **构建指南** | [BUILD_GUIDE.md](BUILD_GUIDE.md) | 构建和编译说明 | 构建工程师、DevOps |

### 特性文档（在docs/文件夹中）

| 文档 | 用途 |
|------|------|
| `utf8_crash_fix.md` | UTF-8处理和文本编码修复 |
| `aspect_ratio_snap_feature.md` | 宽高比锁定特性说明 |
| `aspect_ratio_snap_fix_verification.md` | 宽高比锁定验证 |
| `video_completion_improvement.md` | 视频播放完成检测 |
| `ui_edit_mode_integration.md` | UI编辑模式集成 |
| `additional_editable_buttons_solution.md` | 可编辑按钮功能 |
| `region_config_usage.md` | 屏幕区域配置使用 |
| `virtual_keyboard_crossplatform.md` | 跨平台虚拟键盘 |

---

## 🎯 按角色快速导航

### 👨‍💻 新开发者

**从这里开始：**

1. 阅读 [README.md](README.md) - 项目概况
2. 查看 [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) - 理解项目结构
3. 按照 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) - 快速开始
4. 参考 [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md) - 编码规范

**快速任务：**
- 添加新屏幕 → [DEVELOPMENT_GUIDE.md#添加新屏幕](DEVELOPMENT_GUIDE.md#添加新屏幕-screen)
- 添加UI按钮 → [DEVELOPMENT_GUIDE.md#添加新的ui元素](DEVELOPMENT_GUIDE.md#添加新的ui元素-button)
- 添加配置项 → [DEVELOPMENT_GUIDE.md#添加新的配置项](DEVELOPMENT_GUIDE.md#添加新的配置项)

### 🏗️ 架构师/资深开发者

**关键文档：**

1. [ARCHITECTURE.md](ARCHITECTURE.md) - 系统设计
2. [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) - 模块组织
3. [FEATURES.md](FEATURES.md) - 功能清单

**重点内容：**
- 分层架构设计
- 设计模式应用
- 性能优化策略
- 扩展性考虑

### 🧪 测试/QA

**相关文档：**

1. [FEATURES.md](FEATURES.md) - 功能完整列表
2. [docs/](docs/) - 各特性详细说明
3. [examples/](examples/) - 测试代码和示例

**测试清单：**
- 名字随机选择功能
- 自定义资源加载（图片、音乐、视频）
- UI交互和屏幕切换
- 国际化支持（多语言）
- 视频完成检测
- 编辑模式
- 升级功能

### 🔨 构建工程师/DevOps

**关键文档：**

1. [BUILD_GUIDE.md](BUILD_GUIDE.md) - 编译构建说明
2. [CMakeLists.txt](CMakeLists.txt) - 构建配置
3. [vcpkg.json](vcpkg.json) - 依赖声明

**主要任务：**
- 环境配置
- 依赖管理
- CI/CD集成
- 版本发布

### 🎨 UI/功能开发者

**相关文档：**

1. [FEATURES.md](FEATURES.md) - 完整功能列表
2. [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md#添加新屏幕-screen) - 屏幕开发
3. [ARCHITECTURE.md](ARCHITECTURE.md#5-ui系统-screenui-system) - UI架构
4. [docs/ui_edit_mode_integration.md](docs/ui_edit_mode_integration.md) - UI编辑模式

**常见任务：**
- 创建新UI屏幕
- 添加UI控件
- 实现交互逻辑
- 编辑模式支持

### 🎬 多媒体功能开发者

**相关文档：**

1. [FEATURES.md#音频和视频](FEATURES.md#4-自定义音效) - 媒体功能说明
2. [DEVELOPMENT_GUIDE.md#处理媒体资源](DEVELOPMENT_GUIDE.md#处理媒体资源) - 媒体接口
3. [docs/video_completion_improvement.md](docs/video_completion_improvement.md) - 视频完成检测

**技术栈：**
- FFmpeg（音视频解码）
- SDL2（音频播放）
- FreeType（字体渲染）

### 🌍 国际化/本地化开发者

**相关文档：**

1. [FEATURES.md#国际化](FEATURES.md#10-多语言支持) - i18n概览
2. [files/localization/](files/localization/) - 语言文件
3. [DEVELOPMENT_GUIDE.md#添加新的本地化字符串](DEVELOPMENT_GUIDE.md#添加新的本地化字符串) - 添加翻译

**支持的语言：**
- 🇨🇳 简体中文 (zh-CN)
- 🇭🇰 繁体中文 (zh-TW)
- 🇯🇵 日文 (ja-JP)
- 🇬🇧 英文 (en-US)

---

## 🔍 按任务查找

### "我想了解这个项目做什么"
→ 阅读 [README.md](README.md)

### "我想理解项目的目录结构"
→ 查看 [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)

### "我想开始开发新功能"
→ 按照 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md)

### "我想遵循编码规范"
→ 参考 [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md)

### "我想了解系统架构"
→ 研究 [ARCHITECTURE.md](ARCHITECTURE.md)

### "我想知道所有功能"
→ 查看 [FEATURES.md](FEATURES.md)

### "我想编译和构建项目"
→ 遵循 [BUILD_GUIDE.md](BUILD_GUIDE.md)

### "我想添加新的UI屏幕"
→ [DEVELOPMENT_GUIDE.md#添加新屏幕-screen](DEVELOPMENT_GUIDE.md#添加新屏幕-screen)

### "我想处理音视频"
→ [DEVELOPMENT_GUIDE.md#处理媒体资源](DEVELOPMENT_GUIDE.md#处理媒体资源)

### "我想添加多语言支持"
→ [DEVELOPMENT_GUIDE.md#添加新的本地化字符串](DEVELOPMENT_GUIDE.md#添加新的本地化字符串)

### "我遇到编译问题"
→ [BUILD_GUIDE.md#构建问题排查](BUILD_GUIDE.md#构建问题排查)

### "我想调试代码"
→ [DEVELOPMENT_GUIDE.md#调试](DEVELOPMENT_GUIDE.md#调试)

---

## 📊 文档概览

```
项目文档总览

├─ README.md (项目说明)
│
├─ 基础文档
│  ├─ PROJECT_STRUCTURE.md (目录结构)
│  ├─ ARCHITECTURE.md (系统架构)
│  └─ FEATURES.md (功能列表)
│
├─ 开发文档
│  ├─ DEVELOPMENT_GUIDE.md (开发入门)
│  ├─ CODE_STYLE_GUIDE.md (编码规范)
│  └─ BUILD_GUIDE.md (构建指南)
│
└─ 特性文档 (docs/文件夹)
   ├─ utf8_crash_fix.md
   ├─ aspect_ratio_snap_feature.md
   ├─ video_completion_improvement.md
   ├─ ui_edit_mode_integration.md
   ├─ additional_editable_buttons_solution.md
   ├─ region_config_usage.md
   └─ virtual_keyboard_crossplatform.md
```

---

## 🤝 贡献指南

在开始贡献前，请：

1. ✅ 阅读 [README.md](README.md) 了解项目
2. ✅ 查看 [ARCHITECTURE.md](ARCHITECTURE.md) 理解设计
3. ✅ 遵循 [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md)
4. ✅ 按照 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md)
5. ✅ 更新相关文档

---

## 🆘 常见问题

**Q: 我找不到我需要的文档**  
A: 查看这个索引文件的 "按任务查找" 部分，或使用工作空间搜索功能。

**Q: 文档太长了，我只想快速了解**  
A: 每个文档都有英文版本（English Version）部分，更加简洁。

**Q: 我想修改文档**  
A: 欢迎！修改后请确保更新相关链接和索引。

**Q: 文档过期了怎么办**  
A: 请提交更新。文档应与代码同步维护。

---

## 📝 文档维护

### 文档更新规则

- 代码有重大改动时更新相关文档
- 每个月检查文档是否需要更新
- 重点关注代码风格和架构文档

### 文档贡献者

欢迎贡献文档改进和翻译！

---

## 🔗 快速链接

| 链接 | 说明 |
|------|------|
| [📁 源代码](src/) | 主源代码目录 |
| [📚 示例](examples/) | 代码示例和测试 |
| [⚙️ 配置文件](CMakeLists.txt) | CMake构建配置 |
| [🔧 依赖](vcpkg.json) | VCPKG依赖声明 |
| [🌐 本地化](files/localization/) | 多语言文件 |

---

## 📖 English Version

### Document Navigation

All documentation is available in both Chinese and English.

**Core Documents:**
- [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) - Directory organization
- [ARCHITECTURE.md](ARCHITECTURE.md) - System design
- [FEATURES.md](FEATURES.md) - Feature list
- [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) - Getting started
- [CODE_STYLE_GUIDE.md](CODE_STYLE_GUIDE.md) - Coding standards
- [BUILD_GUIDE.md](BUILD_GUIDE.md) - Build instructions

**By Role:**
- **New Developer** → Start with DEVELOPMENT_GUIDE.md
- **Architect** → Review ARCHITECTURE.md
- **Build Engineer** → Follow BUILD_GUIDE.md
- **Tester** → Check FEATURES.md

**By Task:**
- Add new screen → DEVELOPMENT_GUIDE.md
- Understand architecture → ARCHITECTURE.md
- Set up build → BUILD_GUIDE.md
- Learn code style → CODE_STYLE_GUIDE.md

