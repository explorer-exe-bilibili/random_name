# UI编辑模式智能吸附功能使用说明

## 功能描述

为UI编辑模式添加了三种智能吸附功能：

1. **图像原比例自动吸附** - 使按钮在拖拽/缩放时能自动吸附到其绑定图片的原始宽高比（**默认开启**）
2. **居中自动吸附** - 使按钮在移动时能自动吸附到屏幕中心位置
3. **自定义吸附点** - 允许用户定义特定的X或Y坐标值，按钮可以自动吸附到这些位置（**新增功能**）

## 功能特性

### 4. 批量控制按钮吸附功能

```cpp
// 在Screen的编辑模式下，禁用所有按钮的图像比例吸附功能
if (editModeEnabled) {
    SetAspectRatioSnapForAllButtons(false);
    
    // 启用居中吸附功能
    SetCenterSnapForAllButtons(true);
    
    // 启用自定义吸附功能并设置常用吸附点
    SetCustomSnapForAllButtons(true);
    AddCustomSnapXForAllButtons(200.0f);    // 左侧边界
    AddCustomSnapXForAllButtons(800.0f);    // 右侧边界
    AddCustomSnapYForAllButtons(150.0f);    // 顶部导航区
    AddCustomSnapYForAllButtons(600.0f);    // 底部操作区
}
```

### 5. 在HandleKeyInput中添加快捷键支持像原比例自动吸附

#### 1.1 自动检测图像比例

- 当调用 `SetBitmap()` 方法设置按钮图像时，会自动计算并存储图像的原始宽高比
- 支持通过 `BitmapID` 或字符串ID设置图像
- **新建的按钮默认启用吸附功能**

#### 1.2 智能吸附判断

- 当按钮在编辑模式下调整大小时，系统会检测当前比例与原始比例的差值
- 当差值小于设定阈值（默认0.05）时，自动吸附到原始比例
- 吸附功能仅在非1:1比例模式下生效，避免与现有1:1比例功能冲突

### 2. 居中自动吸附

#### 2.1 屏幕中心检测

- 在移动模式下检测按钮中心与屏幕中心的距离
- 支持水平和垂直方向的独立吸附
- 当距离小于设定阈值（默认10像素）时，自动吸附到对应的中心线

#### 2.2 实时视觉反馈

- 在拖拽过程中显示半透明的居中辅助线
- 水平居中时显示垂直辅助线
- 垂直居中时显示水平辅助线

### 3. 自定义吸附点

#### 3.1 定义吸附点

- 用户可以在设计时或运行时定义特定的X或Y坐标作为吸附点
- 支持通过API分别设置X轴和Y轴的吸附点
- 可以为每个按钮设置不同的自定义吸附点集合

#### 3.2 智能吸附到自定义点

- 当按钮移动到接近自定义吸附点时，系统会自动吸附到该点
- 支持同时吸附X轴和Y轴的不同吸附点
- 支持对自定义吸附点的增删改查操作

#### 3.3 实时视觉反馈

- 在拖拽过程中显示半透明的青色辅助线
- X轴吸附点显示垂直辅助线
- Y轴吸附点显示水平辅助线

### 4. 按钮间对齐吸附

#### 4.1 自动对齐检测

- 当移动按钮时，系统会自动检测与其他按钮的对齐关系
- 支持X轴（垂直对齐）和Y轴（水平对齐）的独立检测
- 可自定义对齐检测的敏感度阈值

#### 4.2 智能对齐吸附

- 当按钮中心接近其他按钮的中心线时，自动吸附对齐
- 支持多个按钮同时参与对齐检测
- 自动选择最近的对齐目标进行吸附

#### 4.3 实时视觉反馈

- 在拖拽过程中显示半透明的黄色对齐辅助线
- 垂直对齐时显示垂直辅助线
- 水平对齐时显示水平辅助线

### 5. 视觉反馈

- **青色边框**：自定义吸附功能启用且当前已吸附到自定义点（**最高优先级**）
- **黄色边框**：按钮间对齐吸附功能启用且当前已吸附到其他按钮
- **紫色边框**：居中吸附功能启用且当前已吸附到中心
- **绿色边框**：图像比例吸附功能启用且当前已吸附到原始比例
- **橙色边框**：图像比例吸附功能启用但当前未吸附
- **蓝色边框**：所有吸附功能已禁用

### 6. 快捷键控制

- 在编辑模式下按 `A` 键可切换所有按钮的图像原比例吸附功能
- 在编辑模式下按 `C` 键可切换所有按钮的居中自动吸附功能
- 在编辑模式下按 `S` 键可切换所有按钮的自定义吸附功能
- 在编辑模式下按 `B` 键可切换所有按钮的按钮间对齐吸附功能
- 在编辑模式下按 `C` 键可切换所有按钮的居中自动吸附功能
- 在编辑模式下按 `P` 键可切换所有按钮的自定义吸附点功能

## API接口

### Button类公共接口

#### 图像原比例吸附相关

```cpp
// 启用/禁用图像原比例吸附功能
void SetAspectRatioSnap(bool enable);

// 检查是否启用了图像原比例吸附功能
bool IsAspectRatioSnapEnabled() const;

// 设置吸附阈值（当前比例与原始比例差值的允许范围）
void SetAspectRatioSnapThreshold(float threshold);

// 获取图像原始宽高比
float GetImageAspectRatio() const;
```

#### 居中自动吸附相关

```cpp
// 启用/禁用居中自动吸附功能
void SetCenterSnap(bool enable);

// 检查是否启用了居中自动吸附功能
bool IsCenterSnapEnabled() const;

// 设置居中吸附阈值（像素距离）
void SetCenterSnapThreshold(float threshold);
```

#### 自定义吸附点相关

```cpp
// 启用/禁用自定义吸附点功能
void SetCustomSnap(bool enable);

// 检查自定义吸附点功能是否启用
bool IsCustomSnapEnabled() const;

// 设置自定义吸附阈值（像素距离）
void SetCustomSnapThreshold(float threshold);

// 添加X轴自定义吸附点
void AddCustomSnapX(float x);

// 添加Y轴自定义吸附点
void AddCustomSnapY(float y);

// 移除指定的X轴自定义吸附点
void RemoveCustomSnapX(float x);

// 移除指定的Y轴自定义吸附点
void RemoveCustomSnapY(float y);

// 清除所有X轴自定义吸附点
void ClearCustomSnapX();

// 清除所有Y轴自定义吸附点
void ClearCustomSnapY();

// 获取当前的X轴自定义吸附点列表
const std::vector<float>& GetCustomSnapX() const;

// 获取当前的Y轴自定义吸附点列表
const std::vector<float>& GetCustomSnapY() const;
```

### Screen基类接口

```cpp
// 切换所有按钮的图像原比例吸附功能
void ToggleAspectRatioSnap();

// 设置所有按钮的图像原比例吸附功能
void SetAspectRatioSnapForAllButtons(bool enable);

// 切换所有按钮的居中自动吸附功能
void ToggleCenterSnap();

// 设置所有按钮的居中自动吸附功能
void SetCenterSnapForAllButtons(bool enable);

// 切换所有按钮的自定义吸附功能
void ToggleCustomSnap();

// 设置所有按钮的自定义吸附功能
void SetCustomSnapForAllButtons(bool enable);

// 为所有按钮添加X轴吸附点
void AddCustomSnapXForAllButtons(float x);

// 为所有按钮添加Y轴吸附点
void AddCustomSnapYForAllButtons(float y);

// 清除所有按钮的自定义吸附点
void ClearCustomSnapForAllButtons();
```

## 使用示例

### 1. 使用默认开启的吸附功能

```cpp
// 创建按钮并设置图像
auto button = std::make_shared<core::Button>();
button->SetBitmap(BitmapID::SomeImage); // 自动计算原始比例，吸附功能默认已启用

// 可选：禁用吸附功能
button->SetAspectRatioSnap(false);

// 可选：设置自定义吸附阈值
button->SetAspectRatioSnapThreshold(0.1f); // 10%的容差
```

### 2. 批量控制按钮吸附功能

```cpp
// 在Screen的编辑模式下，禁用所有按钮的吸附功能
if (editModeEnabled) {
    SetAspectRatioSnapForAllButtons(false);
}
```

### 3. 使用自定义吸附点功能

```cpp
// 创建按钮
auto button = std::make_shared<core::Button>();

// 启用自定义吸附功能
button->SetCustomSnap(true);

// 添加X轴吸附点（例如：屏幕的1/4、1/2、3/4位置）
button->AddCustomSnapX(WindowInfo.width * 0.25f);
button->AddCustomSnapX(WindowInfo.width * 0.5f);
button->AddCustomSnapX(WindowInfo.width * 0.75f);

// 添加Y轴吸附点（例如：顶部、中部、底部）
button->AddCustomSnapY(100.0f);  // 距离顶部100像素
button->AddCustomSnapY(WindowInfo.height * 0.5f);
button->AddCustomSnapY(WindowInfo.height - 100.0f);  // 距离底部100像素

// 设置自定义吸附阈值
button->SetCustomSnapThreshold(15.0f);  // 15像素范围内吸附

// 批量为所有按钮添加相同的吸附点
SetCustomSnapForAllButtons(true);
AddCustomSnapXForAllButtons(WindowInfo.width * 0.5f);  // 屏幕中心X轴
AddCustomSnapYForAllButtons(WindowInfo.height * 0.3f); // 屏幕上方1/3位置
```

### 4. 批量控制按钮吸附功能_

```cpp
// 在Screen的编辑模式下，禁用所有按钮的吸附功能
if (editModeEnabled) {
    SetAspectRatioSnapForAllButtons(false);
}
```

### 5. 快捷键控制_

- 在编辑模式下按 `A` 键可切换所有按钮的图像原比例吸附功能
- 在编辑模式下按 `C` 键可切换所有按钮的居中自动吸附功能
- 在编辑模式下按 `S` 键可切换所有按钮的自定义吸附功能

```cpp
bool YourScreen::HandleKeyInput(char key) {
    if (editModeEnabled) {
        if (key == 'A' || key == 'a') {
            ToggleAspectRatioSnap();
            return true;
        }
        if (key == 'C' || key == 'c') {
            ToggleCenterSnap();
            return true;
        }
        if (key == 'S' || key == 's') {
            ToggleCustomSnap();
            return true;
        }
    }
    // 处理其他键盘输入...
    return false;
}
```

## 技术实现细节

### 私有成员变量

```cpp
// 图像原比例吸附相关
bool enableAspectRatioSnap = true;         // 是否启用图像比例吸附功能（默认开启）
float aspectRatioSnapThreshold = 0.2f;     // 图像比例吸附阈值
float originalImageAspectRatio = 1.0f;     // 图像原始宽高比
bool hasValidImageAspectRatio = false;     // 是否有有效的图像宽高比

// 居中自动吸附相关
bool enableCenterSnap = false;             // 是否启用居中吸附功能（默认关闭）
float centerSnapThreshold = 10.0f;         // 居中吸附阈值（像素）
bool isSnappedToCenter = false;            // 当前是否已吸附到中心

// 自定义吸附点相关
bool enableCustomSnap = false;             // 是否启用自定义吸附功能（默认关闭）
float customSnapThreshold = 8.0f;          // 自定义吸附阈值（像素）
std::vector<float> customSnapX;            // X轴自定义吸附点列表
std::vector<float> customSnapY;            // Y轴自定义吸附点列表
bool isSnappedToCustom = false;            // 当前是否已吸附到自定义点
```

### 私有辅助方法

```cpp
// 图像原比例吸附相关
void UpdateImageAspectRatio();                           // 更新图像原始比例
bool ShouldSnapToAspectRatio(float currentAspectRatio) const; // 判断是否应该吸附到原始比例
void ApplyAspectRatioSnap(Region& targetRegion) const;   // 应用图像比例吸附调整

// 居中自动吸附相关
bool ShouldSnapToCenter(const Region& targetRegion) const;     // 判断是否应该吸附到中心
void ApplyCenterSnap(Region& targetRegion) const;            // 应用居中吸附调整

// 自定义吸附点相关
bool ShouldSnapToCustomPoints(const Region& targetRegion, float& snapX, float& snapY) const; // 判断是否应该吸附到自定义点
void ApplyCustomSnap(Region& targetRegion, float snapX, float snapY) const;  // 应用自定义吸附调整
```

## 注意事项

1. **默认行为**：所有新创建的按钮都会默认开启图像原比例吸附功能，无需手动设置。

2. **与1:1比例模式的兼容性**：图像原比例吸附功能仅在非1:1比例模式下生效，确保与现有功能无冲突。

3. **性能考虑**：吸附检测仅在编辑模式下的拖拽操作中进行，不影响正常显示性能。

4. **线程安全**：所有操作都在主线程中进行，无需额外的线程同步。

5. **错误处理**：当图像未加载或无效时，吸附功能会自动禁用，不会产生错误。

## 扩展建议

1. **配置持久化**：可以将吸附设置保存到配置文件中，实现跨会话的设置保持。

2. **更多快捷键**：可以添加其他编辑相关的快捷键，如调整吸附阈值等。

3. **视觉增强**：可以在吸附时显示临时的比例信息文本，帮助用户了解当前状态。
