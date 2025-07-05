// 编辑模式使用示例
#include "core/screen/base.h"
#include "core/Config.h"
#include "core/log.h"

// 在主事件循环中添加编辑模式处理
void HandleEditModeInput() {
    static bool editModeToggled = false;
    
    // 按F2切换编辑模式
    if (IsKeyPressed(GLFW_KEY_F2) && !editModeToggled) {
        auto currentScreen = screen::Screen::getCurrentScreen();
        if (currentScreen) {
            bool currentEditMode = currentScreen->IsEditModeEnabled();
            bool newEditMode = !currentEditMode;
            
            currentScreen->SetEditMode(newEditMode);
            
            core::Log << "UI Edit mode " << (newEditMode ? "enabled" : "disabled") << core::op::endl;
        }
        editModeToggled = true;
    }
    
    if (!IsKeyPressed(GLFW_KEY_F2)) {
        editModeToggled = false;
    }
    
    // 按F3重置当前屏幕的布局
    static bool resetToggled = false;
    if (IsKeyPressed(GLFW_KEY_F3) && !resetToggled) {
        auto currentScreen = screen::Screen::getCurrentScreen();
        if (currentScreen && currentScreen->HasCustomLayout()) {
            currentScreen->ResetButtonLayout();
            core::Log << "Button layout reset" << core::op::endl;
        }
        resetToggled = true;
    }
    
    if (!IsKeyPressed(GLFW_KEY_F3)) {
        resetToggled = false;
    }
}

// 在鼠标事件处理中添加编辑模式支持
void HandleMouseEvents(double mouseX, double mouseY) {
    auto currentScreen = screen::Screen::getCurrentScreen();
    if (!currentScreen) return;
    
    int x = static_cast<int>(mouseX);
    int y = static_cast<int>(mouseY);
    
    if (IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        if (currentScreen->IsEditModeEnabled()) {
            currentScreen->OnEditMouseDown(x, y);
        } else {
            currentScreen->Click(x, y);
        }
    }
    
    if (IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && currentScreen->IsEditModeEnabled()) {
        currentScreen->OnEditMouseMove(x, y);
    }
    
    if (IsMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT) && currentScreen->IsEditModeEnabled()) {
        currentScreen->OnEditMouseUp(x, y);
    }
}

// 在主循环中调用
void MainLoop() {
    while (!ShouldClose()) {
        // 处理输入
        HandleEditModeInput();
        
        // 获取鼠标位置
        double mouseX, mouseY;
        GetCursorPos(&mouseX, &mouseY);
        HandleMouseEvents(mouseX, mouseY);
        
        // 渲染
        Clear();
        
        auto currentScreen = screen::Screen::getCurrentScreen();
        if (currentScreen) {
            currentScreen->Draw();
        }
        
        SwapBuffers();
        PollEvents();
    }
}

// 使用方法说明：
/*
1. 运行时按键操作：
   - F2: 切换编辑模式开关
   - F3: 重置当前屏幕的按钮布局到默认状态

2. 编辑模式下的鼠标操作：
   - 点击按钮内部: 开始移动按钮
   - 点击按钮边框的小方块: 调整按钮大小
   - 拖拽: 实时预览变化
   - 释放鼠标: 自动保存布局

3. 功能特性：
   - 每个屏幕的布局独立保存
   - 支持最小尺寸限制
   - 支持屏幕边界限制
   - 自动保存到配置文件
   - 重启程序后布局会自动加载

4. 配置文件中的布局数据：
   - 保存在对应的RegionName类别下
   - 格式: "screen_[ID]_button_[index]" 或 "screen_[ID]_exit"
   - 可以手动编辑配置文件中的数值
*/
