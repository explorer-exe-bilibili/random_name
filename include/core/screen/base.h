#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <mutex>
#include <functional>

#include "core/baseItem/Button.h"
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/VideoPlayer.h"
#include "core/nameRandomer.h"


namespace screen{
enum class ScreenID {
    Unknown = -1,
    MainMenu,
    Settings,
    Name,
    Video,
    ListName,
};

enum class TransitionState {
    Stable,     // 稳定状态，无过渡
    FadeOut,    // 淡出状态
    FadeIn      // 淡入状态
};

class Screen {
protected:
    std::vector<std::shared_ptr<core::Button>> buttons;
    std::vector<std::shared_ptr<core::Button>> additionalEditableButtons; // 额外的可编辑按钮容器
    core::Bitmap** background;
    ScreenID ID;
    static core::VideoPlayer* videoBackground;
    static ScreenID currentScreenID;
    static std::shared_ptr<Screen> currentScreen;
    static std::map<ScreenID, std::shared_ptr<Screen>> screens;
    static std::vector<core::NameEntry> nameItems;
    static std::shared_ptr<core::Button> exitButton;
    static std::shared_ptr<core::Button> exitButtonEdit; // 编辑模式下的退出按钮
    std::vector<std::shared_ptr<core::Button>> EditingButtons; // 用于主菜单的覆盖按钮

    // 淡入淡出相关成员
    static TransitionState transitionState;
    static std::chrono::steady_clock::time_point transitionStartTime;
    static float transitionDuration; // 过渡持续时间（秒）
    static std::shared_ptr<Screen> nextScreen; // 待切换的下一个屏幕
    static std::function<void()> transitionCompleteCallback;
    static std::mutex transitionMutex; // 保护过渡状态的互斥锁

    // 编辑模式相关成员
    bool editModeEnabled = false;
    std::shared_ptr<core::Button> selectedButton = nullptr;
    std::function<void()> onEditCompleteCallback = nullptr;

    // 管理额外按钮的方法
    void RegisterEditableButton(std::shared_ptr<core::Button> button);
    void UnregisterEditableButton(std::shared_ptr<core::Button> button);
    void ClearEditableButtons();

    // 计算当前的alpha值（基于过渡状态）
    static float getCurrentAlpha();
    
    // 更新过渡状态
    static void updateTransition(int param = 0);
    virtual void reloadButtonsRegion(){}
public:
    Screen(ScreenID ID) : ID(ID) {}
    virtual ~Screen(){background=nullptr;};
    virtual void init();
    virtual bool Click(int x, int y);

    virtual void Draw();
    virtual void DrawEditOverlays();
    virtual void enter(int param)=0;
    virtual void exit(){};
    virtual bool HandleKeyInput(char key) { return false; } // 处理键盘输入，默认不处理
    virtual bool HandleUnicodeInput(const std::string& utf8_char) { return false; } // 处理Unicode字符输入，默认不处理

    // 编辑模式相关方法
    virtual void SetEditMode(bool enable);
    virtual bool IsEditModeEnabled() const { return editModeEnabled; }
    virtual void OnEditMouseDown(int x, int y);
    virtual void OnEditMouseMove(int x, int y);  
    virtual void OnEditMouseUp(int x, int y);
    virtual void SaveButtonLayout();
    
    // 图像原比例吸附功能
    virtual void ToggleAspectRatioSnap(); // 切换所有按钮的图像原比例吸附功能
    virtual void SetAspectRatioSnapForAllButtons(bool enable); // 设置所有按钮的图像原比例吸附功能
    
    // 居中自动吸附功能
    virtual void ToggleCenterSnap(); // 切换所有按钮的居中自动吸附功能
    virtual void SetCenterSnapForAllButtons(bool enable); // 设置所有按钮的居中自动吸附功能

    // 自定义吸附点功能
    virtual void ToggleCustomSnap(); // 切换所有按钮的自定义吸附功能
    virtual void SetCustomSnapForAllButtons(bool enable); // 设置所有按钮的自定义吸附功能
    virtual void AddCustomSnapXForAllButtons(float x); // 为所有按钮添加X轴吸附点
    virtual void AddCustomSnapYForAllButtons(float y); // 为所有按钮添加Y轴吸附点
    virtual void ClearCustomSnapForAllButtons(); // 清除所有按钮的自定义吸附点

    ScreenID getID() const { return ID; }
    static std::shared_ptr<Screen> getCurrentScreen() { return currentScreen; }    // 屏幕管理系统功能
    static std::shared_ptr<Screen> getScreen(ScreenID id);
    static void RegisterScreen(ScreenID id, std::shared_ptr<Screen> screen);
    static bool SwitchToScreen(ScreenID id,int param = 0); // 切换到指定屏幕
    
    // 淡入淡出功能
    static bool SwitchToScreenWithFade(ScreenID id, int param=0,float fadeTime = 0.5f, std::function<void()> onComplete = nullptr);
    static void SetTransitionDuration(float duration) { transitionDuration = duration; }
    static float GetTransitionDuration() { return transitionDuration; }
    static TransitionState GetTransitionState() { return transitionState; }

    static void setUseVideoBackground(bool use);

    static void reloadConfig();
private:
};

}