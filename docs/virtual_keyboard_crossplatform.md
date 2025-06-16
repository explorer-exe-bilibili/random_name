# 跨平台软键盘功能说明

## 概述

本项目实现了跨平台的软键盘自动显示和隐藏功能，支持在文本输入时自动调用系统虚拟键盘，特别适用于触屏设备和无物理键盘的环境。

## 支持的平台

### Windows

- **使用的键盘**: OSK (屏幕键盘)
- **启动方式**: `ShellExecuteW` + `osk.exe`
- **关闭方式**: 查找窗口句柄 + `SendMessage`
- **兼容性**: Windows 7 及以上版本
- **特点**: 内置系统功能，无需额外安装

### macOS

- **使用的键盘**: 辅助功能键盖
- **启动方式**: AppleScript + System Events
- **关闭方式**: AppleScript + System Events
- **兼容性**: macOS 10.9 及以上版本
- **特点**: 系统内置，支持多语言

### Linux

- **支持的键盘**:
  - `onboard` (推荐) - 功能丰富，用户友好
  - `florence` - 轻量级，资源占用少
  - `matchbox-keyboard` - 嵌入式系统常用
  - `kvkbd` - KDE 环境优化
  - `cellwriter` - 支持手写识别
- **启动方式**: `fork` + `exec`
- **关闭方式**: `kill` + `SIGTERM`
- **安装**: 使用提供的 `install_virtual_keyboard.sh` 脚本

## 功能特点

### 自动化

- ✅ 点击文本框时自动显示软键盘
- ✅ 完成输入时自动隐藏软键盘
- ✅ 取消输入时自动隐藏软键盘
- ✅ 点击其他区域时自动保存并隐藏

### 智能检测

- ✅ 自动检测平台并选择合适的键盘
- ✅ 检查虚拟键盘是否可用
- ✅ 智能选择最佳的虚拟键盘程序
- ✅ 错误处理和日志记录

### 跨平台兼容

- ✅ Windows、macOS、Linux 统一接口
- ✅ 条件编译确保代码整洁
- ✅ 平台特定优化
- ✅ 向后兼容处理

## 使用方法

### 对于开发者

```cpp
// 检查是否支持虚拟键盘
if (IsVirtualKeyboardSupported()) {
    // 显示虚拟键盘
    ShowVirtualKeyboard();
    
    // ... 用户输入 ...
    
    // 隐藏虚拟键盘
    HideVirtualKeyboard();
}
```

### 对于用户

1. **Windows**: 无需额外设置，系统内置 OSK
2. **macOS**: 需要在系统偏好设置中启用辅助功能键盘
3. **Linux**: 运行 `./install_virtual_keyboard.sh` 安装虚拟键盘

## 安装说明

### Linux 系统

```bash
# 使用提供的安装脚本
chmod +x install_virtual_keyboard.sh
./install_virtual_keyboard.sh

# 或手动安装（Ubuntu/Debian）
sudo apt-get update
sudo apt-get install onboard

# CentOS/RHEL
sudo yum install onboard

# Arch Linux
sudo pacman -S onboard
```

### macOS 系统

1. 打开系统偏好设置
2. 选择"辅助功能"
3. 在左侧选择"键盘"
4. 勾选"启用辅助功能键盘"

## 技术实现

### 架构设计

```none
SettingButton (文本框按钮)
    ↓
跨平台软键盘接口
    ↓
┌─────────────┬─────────────┬─────────────┐
│   Windows   │    macOS    │    Linux    │
│   OSK.exe   │ AppleScript │ onboard等   │
└─────────────┴─────────────┴─────────────┘
```

### 关键函数

- `IsVirtualKeyboardSupported()` - 检测平台支持
- `GetPlatformKeyboardInfo()` - 获取平台信息
- `ShowVirtualKeyboard()` - 显示虚拟键盘
- `HideVirtualKeyboard()` - 隐藏虚拟键盘

## 使用场景

### 适合的场景

- 🎮 触屏游戏或媒体中心
- 🏪 公用设备和信息亭系统
- 🔧 嵌入式系统和IoT设备
- ♿ 辅助功能和可访问性需求
- 📱 平板电脑应用程序

### 不适合的场景

- 传统桌面应用（有物理键盘）
- 命令行工具
- 服务器端应用程序

## 故障排除

### Windows7-11

- **问题**: OSK 无法启动
- **解决**: 检查 Windows 服务和系统完整性
- **命令**: `sfc /scannow`

### macOS 10+

- **问题**: 辅助功能键盘不显示
- **解决**: 检查辅助功能权限和系统偏好设置

### Linux desktop

- **问题**: 虚拟键盘程序未找到
- **解决**: 使用安装脚本或手动安装所需程序
- **检查**: `which onboard florence matchbox-keyboard`

## 日志和调试

程序会记录以下日志信息：

- 平台检测结果
- 虚拟键盘支持状态
- 启动/关闭操作结果
- 错误和异常信息

查看日志级别：

- `Info`: 一般信息
- `Debug`: 调试信息
- `Warn`: 警告信息
- `Error`: 错误信息

## 贡献和扩展

### 添加新平台支持

1. 在条件编译中添加新平台宏
2. 实现平台特定的显示/隐藏函数
3. 更新支持检测函数
4. 添加相应的文档和测试

### 添加新的Linux虚拟键盘

1. 在 `DetectAvailableKeyboard()` 中添加新键盘
2. 在 `ShowVirtualKeyboard()` 中添加启动逻辑
3. 更新安装脚本
4. 测试兼容性

## 版本历史

- **v1.0**: 基础Windows支持
- **v2.0**: 添加跨平台支持 (Windows/macOS/Linux)
- **v2.1**: 智能检测和错误处理优化
- **v2.2**: 添加安装脚本和文档

## 许可和鸣谢

本功能作为开源项目的一部分，遵循项目的开源许可证。

感谢各个操作系统提供的虚拟键盘支持，特别是：

- Microsoft Windows OSK 团队
- Apple 辅助功能团队
- Linux 社区的虚拟键盘开发者们
