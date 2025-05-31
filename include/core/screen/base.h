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
#include "core/baseItem/Video/VideoPlayer.h"


namespace screen{
enum class ScreenID {
    MainMenu,
    Settings,
    Name,
    Video
};

enum class TransitionState {
    Stable,     // 稳定状态，无过渡
    FadeOut,    // 淡出状态
    FadeIn      // 淡入状态
};

class Screen {
protected:
    std::vector<std::shared_ptr<core::Button>> buttons;
    core::Bitmap* background;
    ScreenID ID;
    static bool useVideoBackground;
    static core::VideoPlayer* videoBackground;
    static ScreenID currentScreenID;
    static std::shared_ptr<Screen> currentScreen;
    static std::map<ScreenID, std::shared_ptr<Screen>> screens;

    // 淡入淡出相关成员
    static TransitionState transitionState;
    static std::chrono::steady_clock::time_point transitionStartTime;
    static float transitionDuration; // 过渡持续时间（秒）
    static std::shared_ptr<Screen> nextScreen; // 待切换的下一个屏幕
    static std::function<void()> transitionCompleteCallback;
    static std::mutex transitionMutex; // 保护过渡状态的互斥锁

    // 计算当前的alpha值（基于过渡状态）
    static float getCurrentAlpha();
    
    // 更新过渡状态
    static void updateTransition();
public:
    Screen(ScreenID ID) : ID(ID) {}
    virtual ~Screen(){background=nullptr;};
    virtual void init();
    virtual bool Click(int x, int y);

    virtual void Draw();
    virtual void enter(int param)=0;
    virtual void exit(){};

    ScreenID getID() const { return ID; }
    static std::shared_ptr<Screen> getCurrentScreen() { return currentScreen; }    // 屏幕管理系统功能
    static void RegisterScreen(ScreenID id, std::shared_ptr<Screen> screen);
    static bool SwitchToScreen(ScreenID id);
    
    // 淡入淡出功能
    static bool SwitchToScreenWithFade(ScreenID id, float fadeTime = 0.5f, std::function<void()> onComplete = nullptr);
    static void SetTransitionDuration(float duration) { transitionDuration = duration; }
    static float GetTransitionDuration() { return transitionDuration; }
    static TransitionState GetTransitionState() { return transitionState; }
private:
};

}