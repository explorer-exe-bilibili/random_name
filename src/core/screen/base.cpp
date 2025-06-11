#include "core/screen/base.h"
#include "core/baseItem/VideoPlayer.h"
#include "core/Config.h"
#include "core/log.h"
#include <algorithm>

using namespace screen;
using namespace core;

std::shared_ptr<Screen> Screen::currentScreen = nullptr;
ScreenID Screen::currentScreenID = ScreenID::MainMenu;
std::map<ScreenID, std::shared_ptr<Screen>> Screen::screens;
bool Screen::useVideoBackground = false;
bool Screen::OffVideo = false;
core::VideoPlayer* Screen::videoBackground = nullptr;
std::vector<NameEntry> Screen::nameItems;

// 淡入淡出相关静态成员初始化
TransitionState Screen::transitionState = TransitionState::Stable;
std::chrono::steady_clock::time_point Screen::transitionStartTime;
float Screen::transitionDuration = 0.5f;
std::shared_ptr<Screen> Screen::nextScreen = nullptr;
std::function<void()> Screen::transitionCompleteCallback = nullptr;
std::mutex Screen::transitionMutex;
static int param = 0;
void Screen::Draw() {
    // 更新过渡状态
    updateTransition(param);

    // 获取当前alpha值
    float alpha = getCurrentAlpha();
    
    std::shared_ptr<Bitmap> currentFrame;
    if(useVideoBackground)currentFrame=videoBackground->getCurrentFrame();
    if(currentFrame){
        currentFrame->CreateTextureFromBuffer();
        currentFrame->Draw({0,0,1,1}, alpha);
    }
    else if (background) {
        background->Draw({0, 0, 1, 1}, 0.75f * alpha);
    }
    
    for (const auto& button : buttons) {
        if(button)
            button->Draw(static_cast<unsigned char>(alpha * 255.0f));
    }
}

void Screen::init() {
    static bool inited = false;
    if (inited) return;
    inited = true;
    OffVideo=Config::getInstance()->getBool(OFF_VIDEO);
    useVideoBackground = Config::getInstance()->getBool(USE_VIDEO_BACKGROUND);
    if (useVideoBackground) {
        videoBackground = Explorer::getInstance()->getVideo(VideoID::Background);
    }
}

bool Screen::Click(int x, int y) {
    bool return_value = false;
    for (const auto& button : buttons) {
        if(button)
            if(button->OnClick(Point(x, y, false)))
            {
                return_value = true;
            }
    }
    return return_value;
}

void Screen::RegisterScreen(ScreenID id, std::shared_ptr<Screen> screen) {
    if (!screens.contains(id)) {
        screens[id] = screen;
    } else {
        Log << Level::Error << "Screen with ID " << static_cast<int>(id) << " already exists." << op::endl;
    }
}

bool Screen::SwitchToScreen(ScreenID id,int param) {
    currentScreenID = id;
    if (screens.contains(id)) {
        if(currentScreen)
            currentScreen->exit();
        currentScreen = screens[id];
        currentScreen->enter(param);
        return true;
    }
    return false;
}

float Screen::getCurrentAlpha() {
    std::lock_guard<std::mutex> lock(transitionMutex);
    
    if (transitionState == TransitionState::Stable) {
        return 1.0f;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - transitionStartTime).count();
    float progress = std::min(1.0f, elapsed / (transitionDuration * 1000.0f));
    
    if (transitionState == TransitionState::FadeOut) {
        return 1.0f - progress; // 从1.0渐变到0.0
    } else if (transitionState == TransitionState::FadeIn) {
        return progress; // 从0.0渐变到1.0
    }
    
    return 1.0f;
}

void Screen::updateTransition(int param) {
    std::lock_guard<std::mutex> lock(transitionMutex);
    
    if (transitionState == TransitionState::Stable) {
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - transitionStartTime).count();
    float progress = elapsed / (transitionDuration * 1000.0f);
    
    if (progress >= 1.0f) {
        // 过渡完成
        if (transitionState == TransitionState::FadeOut) {
            // 淡出完成，切换到下一个屏幕并开始淡入
            if (nextScreen) {
                currentScreen = nextScreen;
                currentScreenID = nextScreen->getID();
                currentScreen->enter(param);
                
                // 开始淡入
                transitionState = TransitionState::FadeIn;
                transitionStartTime = now;
            }
        } else if (transitionState == TransitionState::FadeIn) {
            // 淡入完成，回到稳定状态
            transitionState = TransitionState::Stable;
            nextScreen = nullptr;
            
            // 调用完成回调
            if (transitionCompleteCallback) {
                auto callback = transitionCompleteCallback;
                transitionCompleteCallback = nullptr;
                callback();
            }
        }
    }
}

bool Screen::SwitchToScreenWithFade(ScreenID id,int param_, float fadeTime, std::function<void()> onComplete) {
    std::lock_guard<std::mutex> lock(transitionMutex);
    // 检查目标屏幕是否存在
    if (!screens.contains(id)) {
        Log << Level::Error << "Screen with ID " << static_cast<int>(id) << " not found." << op::endl;
        return false;
    }
    
    // 如果已经在过渡中，忽略新的切换请求
    if (transitionState != TransitionState::Stable) {
        Log << Level::Warn << "Screen transition already in progress, ignoring new request." << op::endl;
        return false;
    }
    
    // 如果目标屏幕就是当前屏幕，直接返回
    if (currentScreenID == id) {
        if (onComplete) {
            onComplete();
        }
        return true;
    }

    param = param_;
    // 设置过渡参数
    transitionDuration = fadeTime;
    nextScreen = screens[id];
    transitionCompleteCallback = onComplete;
    
    // 开始淡出
    transitionState = TransitionState::FadeOut;
    transitionStartTime = std::chrono::steady_clock::now();
    
    Log << Level::Info << "Starting fade transition from screen " << static_cast<int>(currentScreenID) 
        << " to screen " << static_cast<int>(id) << " (duration: " << fadeTime << "s)" << op::endl;
    
    return true;
}

void Screen::setUseVideoBackground(bool use) {
        useVideoBackground = use;
        if (use && videoBackground == nullptr) {
            videoBackground = core::Explorer::getInstance()->getVideo(core::VideoID::Background);
        }
}