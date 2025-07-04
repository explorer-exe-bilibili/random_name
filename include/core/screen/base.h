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
    core::Bitmap** background;
    ScreenID ID;
    static core::VideoPlayer* videoBackground;
    static ScreenID currentScreenID;
    static std::shared_ptr<Screen> currentScreen;
    static std::map<ScreenID, std::shared_ptr<Screen>> screens;
    static std::vector<core::NameEntry> nameItems;
    static std::shared_ptr<core::Button> exitButton;

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
    virtual void DrawEditOverlays();
    virtual void SaveButtonLayout();
    virtual void LoadButtonLayout();
    virtual void ResetButtonLayout();
    virtual bool HasCustomLayout() const;

    ScreenID getID() const { return ID; }
    static std::shared_ptr<Screen> getCurrentScreen() { return currentScreen; }    // 屏幕管理系统功能
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