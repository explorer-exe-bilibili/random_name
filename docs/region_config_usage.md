# Config类Region功能使用指南

本文档介绍了如何使用Config类中新添加的Region相关功能。

## 概述

Region功能允许您在配置文件中保存和管理UI区域信息，包括位置、大小和屏幕比例设置。这对于游戏UI布局管理非常有用。

## 功能特性

### 1. 基本Region操作

- `getRegion()` - 获取Region配置
- `setRegion()` - 设置Region配置
- `setifnoRegion()` - 设置默认Region（仅在不存在时）

### 2. 批量Region操作

- `getRegionList()` - 获取Region列表
- `setRegionList()` - 设置Region列表
- `setifnoRegionList()` - 设置默认Region列表

### 3. 命名区域管理

- `getNamedRegion()` - 获取命名Region
- `setNamedRegion()` - 设置命名Region
- `setifnoNamedRegion()` - 设置默认命名Region

### 4. 区域预设管理

- `defineRegionPreset()` - 定义Region预设
- `getRegionPreset()` - 获取Region预设
- `applyRegionPreset()` - 应用Region预设
- `getAvailableRegionPresets()` - 获取可用预设列表

### 5. 区域验证和修正

- `validateRegion()` - 验证Region有效性
- `normalizeRegion()` - 标准化Region数据

## 使用示例

### 基本用法

```cpp
#include "core/Config.h"
#include "core/configItem.h"

// 获取Config实例
Config* config = Config::getInstance();

// 创建一个按钮区域
Region buttonRegion(0.1, 0.8, 0.3, 0.9, true);

// 保存到配置
config->setRegion(UI_REGION_BUTTON_AREA, buttonRegion);

// 从配置加载
Region loadedRegion = config->getRegion(UI_REGION_BUTTON_AREA, Region(0, 0, 1, 1));
```

### 命名区域管理

```cpp
// 为不同类型的按钮设置区域
config->setNamedRegion("buttons", "start", Region(0.4, 0.5, 0.6, 0.6, true));
config->setNamedRegion("buttons", "exit", Region(0.4, 0.7, 0.6, 0.8, true));

// 获取特定按钮区域
Region startButton = config->getNamedRegion("buttons", "start", Region());
```

### 预设管理

```cpp
// 定义布局预设
std::unordered_map<std::string, Region> defaultLayout = {
    {"main_menu", Region(0.1, 0.1, 0.9, 0.9, true)},
    {"button_area", Region(0.2, 0.8, 0.8, 0.95, true)}
};

config->defineRegionPreset("default", defaultLayout);

// 应用预设
config->applyRegionPreset("default");
```

## 预定义配置键

以下是预定义的Region配置键，可以直接使用：

### UI区域配置键

- `UI_REGION_MAIN_MENU` - 主菜单区域
- `UI_REGION_SETTING_PANEL` - 设置面板区域
- `UI_REGION_NAME_DISPLAY` - 名称显示区域
- `UI_REGION_BUTTON_AREA` - 按钮区域
- `UI_REGION_VIDEO_PLAYER` - 视频播放器区域
- `UI_REGION_OVERLAY` - 覆盖层区域
- `UI_REGION_FLOAT_WINDOW` - 浮动窗口区域
- `UI_REGION_SETTING_BUTTON` - 设置按钮区域
- `UI_REGION_NAME_LIST` - 名称列表区域
- `UI_REGION_CONTROL_PANEL` - 控制面板区域

### 预设名称

- `REGION_PRESET_DEFAULT` - 默认布局
- `REGION_PRESET_FULLSCREEN` - 全屏布局
- `REGION_PRESET_WINDOWED` - 窗口布局
- `REGION_PRESET_COMPACT` - 紧凑布局
- `REGION_PRESET_MOBILE` - 移动设备布局

## JSON存储格式

Region数据在配置文件中以JSON格式存储：

```json
{
  "config": {
    "ui_region_button_area": {
      "x": 0.1,
      "y": 0.8,
      "xend": 0.3,
      "yend": 0.9,
      "screenRatio": true
    },
    "region_presets.default": {
      "main_menu": {
        "x": 0.1,
        "y": 0.1,
        "xend": 0.9,
        "yend": 0.9,
        "screenRatio": true
      }
    }
  }
}
```

## 区域坐标系统

- **x, y**: 区域左上角坐标
- **xend, yend**: 区域右下角坐标
- **screenRatio**: 是否使用屏幕比例坐标
  - `true`: 坐标为0-1的比例值
  - `false`: 坐标为绝对像素值

## 最佳实践

1. **使用屏幕比例坐标**: 对于响应式UI，建议使用`screenRatio=true`
2. **验证区域数据**: 使用`validateRegion()`确保数据有效
3. **使用预设管理**: 为不同屏幕尺寸定义不同的布局预设
4. **命名规范**: 使用清晰的命名来组织相关区域
5. **默认值设置**: 使用`setifno*`方法设置合理的默认值

## 示例代码

完整的使用示例请参考 `examples/region_config_example.cpp` 文件。

## 注意事项

- Region坐标必须满足 `x < xend` 和 `y < yend`
- 使用屏幕比例时，坐标值应在0-1范围内
- 预设名称和区域名称区分大小写
- 所有Region操作都会自动保存到配置文件

## 错误处理

所有Region操作都包含完善的错误处理：

- 无效的Region数据会记录警告并使用默认值
- JSON解析错误会自动降级到默认值
- 所有异常都会被捕获并记录到日志中
