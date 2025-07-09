# 图像原比例自动吸附功能使用说明

## 功能描述

为UI编辑模式添加了"按设置的图像原比例自动吸附"功能，使按钮在拖拽/缩放时能自动吸附到其绑定图片的原始宽高比。**此功能默认开启**。

## 功能特性

### 1. 自动检测图像比例

- 当调用 `SetBitmap()` 方法设置按钮图像时，会自动计算并存储图像的原始宽高比
- 支持通过 `BitmapID` 或字符串ID设置图像
- **新建的按钮默认启用吸附功能**

### 2. 智能吸附判断

- 当按钮在编辑模式下调整大小时，系统会检测当前比例与原始比例的差值
- 当差值小于设定阈值（默认0.05）时，自动吸附到原始比例
- 吸附功能仅在非1:1比例模式下生效，避免与现有1:1比例功能冲突

### 3. 视觉反馈

- **绿色边框**：吸附功能启用且当前已吸附到原始比例
- **橙色边框**：吸附功能启用但当前未吸附
- **蓝色边框**：吸附功能已禁用

### 4. 快捷键控制

- 在编辑模式下按 `A` 键可切换所有按钮的图像原比例吸附功能

## API接口

### Button类公共接口

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

### Screen基类接口

```cpp
// 切换所有按钮的图像原比例吸附功能
void ToggleAspectRatioSnap();

// 设置所有按钮的图像原比例吸附功能
void SetAspectRatioSnapForAllButtons(bool enable);
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

### 3. 在HandleKeyInput中添加快捷键支持

```cpp
bool YourScreen::HandleKeyInput(char key) {
    if (editModeEnabled) {
        if (key == 'A' || key == 'a') {
            ToggleAspectRatioSnap();
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
bool enableAspectRatioSnap = true;         // 是否启用吸附功能（默认开启）
float aspectRatioSnapThreshold = 0.05f;    // 吸附阈值
float originalImageAspectRatio = 1.0f;     // 图像原始宽高比
bool hasValidImageAspectRatio = false;     // 是否有有效的图像宽高比
```

### 私有辅助方法

```cpp
void UpdateImageAspectRatio();                           // 更新图像原始比例
bool ShouldSnapToAspectRatio(float currentAspectRatio) const; // 判断是否应该吸附
void ApplyAspectRatioSnap(Region& targetRegion) const;   // 应用吸附调整
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
