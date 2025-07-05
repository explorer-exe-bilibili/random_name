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
core::VideoPlayer* Screen::videoBackground = nullptr;
std::vector<NameEntry> Screen::nameItems;
std::shared_ptr<core::Button> Screen::exitButton;

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
    if(bools[boolconfig::use_video_background]){
        if(!videoBackground){
            if(!Explorer::getInstance()->isVideoLoaded(VideoID::Background)) {
                Explorer::getInstance()->loadVideo(VideoID::Background, Config::getInstance()->getPath(BACKGROUND_VIDEO_PATH));
            }
            videoBackground = Explorer::getInstance()->getVideo(VideoID::Background);
            videoBackground->setLoop(true);
            videoBackground->play();
        }
        currentFrame=videoBackground->getCurrentFrame();
    }
    if(currentFrame){
        currentFrame->CreateTextureFromBuffer();
        currentFrame->Draw({0,0,1,1}, alpha);
    }
    else if (background && *background) {
        (*background)->Draw({0, 0, 1, 1}, 0.75f * alpha);
    }
    
    for (const auto& button : buttons) {
        if(button)
            button->Draw(static_cast<unsigned char>(alpha * 255.0f));
    }
    if(exitButton)
        exitButton->Draw(static_cast<unsigned char>(alpha * 255.0f));
    
    // 在最后绘制编辑覆盖层
    if (editModeEnabled) {
        DrawEditOverlays();
    }
}

void Screen::init() {
    static bool inited = false;
    if (inited) return;
    inited = true;
    // 从config中加载bool值到bools映射中
    LoadBoolsFromConfig();
    if (bools[boolconfig::use_video_background]) {
        videoBackground = Explorer::getInstance()->getVideo(VideoID::Background);
    }
    exitButton = std::make_unique<core::Button>();
    exitButton->SetAudioID(AudioID::enter);
    exitButton->SetBitmap(BitmapID::Exit);
    exitButton->SetEnableBitmap(true);
    exitButton->SetEnableText(false);
    exitButton->SetText("exit");
    exitButton->SetRegion(Config::getInstance()->getRegion(UI_REGION_EXIT));
}

bool Screen::Click(int x, int y) {
    core::Point point(x, y, false);
    
    if (!editModeEnabled) {
        // 正常点击处理
        if (exitButton && exitButton->OnClick(point)) return true;
        for (const auto& button : buttons) {
            if (button && button->OnClick(point)) return true;
        }
        return false;
    }
    
    // 编辑模式下的处理
    if (exitButton && exitButton->OnEditMouseDown(point)) {
        selectedButton = exitButton;
        return true;
    }
    
    for (auto& button : buttons) {
        if (button && button->OnEditMouseDown(point)) {
            selectedButton = button;
            return true;
        }
    }
    
    return false;
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
    if(bools[boolconfig::nosmoothui]){
        SwitchToScreen(id,param_);
        transitionState = TransitionState::Stable;
        onComplete();
        return true;
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
    if (use && videoBackground == nullptr) {
        videoBackground = core::Explorer::getInstance()->getVideo(core::VideoID::Background);
    }
}

void Screen::reloadConfig() {
    exitButton->SetRegion(Config::getInstance()->getRegion(UI_REGION_EXIT));
    for(auto & s:screens)s.second->reloadButtonsRegion();
}

// 编辑模式方法实现
void Screen::SetEditMode(bool enable) {
    editModeEnabled = enable;
    
    if (enable) {
        // 进入编辑模式时，先加载自定义布局
        LoadButtonLayout();
    }
    
    // 为所有按钮设置编辑模式
    for (auto& button : buttons) {
        if (button) {
            button->SetEditMode(enable);
            if (enable) {
                // 设置编辑完成回调
                button->SetOnEditComplete([this](const core::Region& newRegion) {
                    SaveButtonLayout();
                });
            }
        }
    }
    
    // 也为exitButton设置编辑模式
    if (exitButton) {
        exitButton->SetEditMode(enable);
        if (enable) {
            exitButton->SetOnEditComplete([this](const core::Region& newRegion) {
                SaveButtonLayout();
            });
        }
    }
    
    if (!enable) {
        selectedButton = nullptr;
    }
    
    Log << "Screen edit mode " << (enable ? "enabled" : "disabled") << op::endl;
}

void Screen::OnEditMouseDown(int x, int y) {
    if (!editModeEnabled) return;
    
    core::Point point(x, y, false);
    
    // 编辑模式下的处理
    if (exitButton && exitButton->OnEditMouseDown(point)) {
        selectedButton = exitButton;
        return;
    }
    
    for (auto& button : buttons) {
        if (button && button->OnEditMouseDown(point)) {
            selectedButton = button;
            return;
        }
    }
}

void Screen::OnEditMouseMove(int x, int y) {
    if (!editModeEnabled) return;
    
    core::Point point(x, y, false);
    
    // 所有按钮都尝试处理移动事件
    if (exitButton) {
        exitButton->OnEditMouseMove(point);
    }
    
    for (auto& button : buttons) {
        if (button) {
            button->OnEditMouseMove(point);
        }
    }
}

void Screen::OnEditMouseUp(int x, int y) {
    if (!editModeEnabled) return;
    
    core::Point point(x, y, false);
    
    // 所有按钮都尝试处理释放事件
    if (exitButton) {
        exitButton->OnEditMouseUp(point);
    }
    
    for (auto& button : buttons) {
        if (button) {
            button->OnEditMouseUp(point);
        }
    }
}

void Screen::DrawEditOverlays() {
    if (!editModeEnabled) return;
    
    // 绘制所有按钮的编辑覆盖层
    if (exitButton) {
        exitButton->DrawEditOverlay();
    }
    
    for (const auto& button : buttons) {
        if (button) {
            button->DrawEditOverlay();
        }
    }
}

void Screen::SaveButtonLayout() {
    core::Config* config = core::Config::getInstance();
    
    // 使用当前屏幕ID作为区域类别来组织按钮布局
    std::string screenPrefix = "screen_" + std::to_string(static_cast<int>(ID));
    
    // 保存exitButton的布局
    if (exitButton) {
        core::Region region = exitButton->GetRegion();
        // 使用Config的Region设置方法，将区域保存到特定类别下
        config->set(screenPrefix + "_exit", region);
    }
    
    // 保存普通按钮的布局
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i]) {
            core::Region region = buttons[i]->GetRegion();
            std::string buttonName = screenPrefix + "_button_" + std::to_string(i);
            
            // 使用Config的Region设置方法
            config->set(buttonName, region);
        }
    }
    
    // 保存配置到文件
    config->saveToFile();
    
    Log << "Button layout saved for screen " << static_cast<int>(ID) << op::endl;
}

void Screen::LoadButtonLayout() {
    core::Config* config = core::Config::getInstance();
    
    // 使用当前屏幕ID作为区域类别
    std::string screenPrefix = "screen_" + std::to_string(static_cast<int>(ID));
    
    // 加载exitButton的布局
    if (exitButton) {
        core::Region defaultRegion = exitButton->GetRegion(); // 使用当前区域作为默认值
        core::Region region = config->getRegion(screenPrefix + "_exit", 
                                               core::RegionName::NONE, 
                                               defaultRegion);
        
        // 如果加载的区域与默认区域不同，说明有自定义布局
        if (region.getOriginX() != defaultRegion.getOriginX() || 
            region.getOriginY() != defaultRegion.getOriginY() ||
            region.getOriginW() != defaultRegion.getOriginW() || 
            region.getOriginH() != defaultRegion.getOriginH()) {
            exitButton->SetRegion(region);
        }
    }
    
    // 加载普通按钮的布局
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i]) {
            std::string buttonName = screenPrefix + "_button_" + std::to_string(i);
            core::Region defaultRegion = buttons[i]->GetRegion(); // 使用当前区域作为默认值
            core::Region region = config->getRegion(buttonName, 
                                                   core::RegionName::NONE, 
                                                   defaultRegion);
            
            // 如果加载的区域与默认区域不同，说明有自定义布局
            if (region.getOriginX() != defaultRegion.getOriginX() || 
                region.getOriginY() != defaultRegion.getOriginY() ||
                region.getOriginW() != defaultRegion.getOriginW() || 
                region.getOriginH() != defaultRegion.getOriginH()) {
                buttons[i]->SetRegion(region);
            }
        }
    }
    
    Log << "Button layout loaded for screen " << static_cast<int>(ID) << op::endl;
}

void Screen::ResetButtonLayout() {
    core::Config* config = core::Config::getInstance();
    
    // 删除当前屏幕的自定义布局配置
    std::string screenPrefix = "screen_" + std::to_string(static_cast<int>(ID));
    
    // 删除exitButton的自定义布局
    if (exitButton) {
        config->remove(screenPrefix + "_exit");
    }
    
    // 删除普通按钮的自定义布局
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i]) {
            std::string buttonName = screenPrefix + "_button_" + std::to_string(i);
            config->remove(buttonName);
        }
    }
    
    // 重新加载默认布局（通过调用reloadButtonsRegion或重新初始化）
    reloadButtonsRegion();
    
    config->saveToFile();
    Log << "Button layout reset for screen " << static_cast<int>(ID) << op::endl;
}

bool Screen::HasCustomLayout() const {
    core::Config* config = core::Config::getInstance();
    std::string screenPrefix = "screen_" + std::to_string(static_cast<int>(ID));
    
    // 检查是否存在任何自定义布局配置
    if (exitButton) {
        core::Region defaultRegion = exitButton->GetRegion();
        core::Region customRegion = config->getRegion(screenPrefix + "_exit", 
                                                     core::RegionName::NONE, 
                                                     defaultRegion);
        if (customRegion.getOriginX() != defaultRegion.getOriginX() || 
            customRegion.getOriginY() != defaultRegion.getOriginY() ||
            customRegion.getOriginW() != defaultRegion.getOriginW() || 
            customRegion.getOriginH() != defaultRegion.getOriginH()) {
            return true;
        }
    }
    
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i]) {
            std::string buttonName = screenPrefix + "_button_" + std::to_string(i);
            core::Region defaultRegion = buttons[i]->GetRegion();
            core::Region customRegion = config->getRegion(buttonName, 
                                                         core::RegionName::NONE, 
                                                         defaultRegion);
            if (customRegion.getOriginX() != defaultRegion.getOriginX() || 
                customRegion.getOriginY() != defaultRegion.getOriginY() ||
                customRegion.getOriginW() != defaultRegion.getOriginW() || 
                customRegion.getOriginH() != defaultRegion.getOriginH()) {
                return true;
            }
        }
    }
    
    return false;
}