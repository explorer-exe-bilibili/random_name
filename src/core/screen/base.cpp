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
std::shared_ptr<core::Button> Screen::exitButtonEdit;

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
    exitButton->SetRegionStr(UI_REGION_EXIT);
    exitButtonEdit = std::make_unique<core::Button>();
    exitButtonEdit->SetAudioID(AudioID::enter);
    exitButtonEdit->SetEnableBitmap(false);
    exitButtonEdit->SetText("退出编辑模式");
    exitButtonEdit->SetFontID(FontID::Normal);
    exitButtonEdit->SetFillColor(Color(128,128,128,128));
    exitButtonEdit->SetFontScale(0.3f);
    exitButtonEdit->SetRegionStr(UI_REGION_EXIT_EDIT);
    exitButtonEdit->SetEnableText(true);
    exitButtonEdit->SetEnable(false);
    exitButtonEdit->SetEnableFill(true);
    exitButtonEdit->SetClickFunc([this](){
        SaveButtonLayout();
        Screen::getCurrentScreen()->SetEditMode(false);
        Screen::SwitchToScreen(ScreenID::Settings);
        if(!Explorer::getInstance()->getAudio()->isMusicPlaying())
            Explorer::getInstance()->playAudio(AudioID::bgm);
    });
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

std::shared_ptr<Screen> Screen::getScreen(ScreenID id) {
    if (screens.contains(id)) {
        return screens[id];
    }
    return nullptr;
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
    exitButtonEdit->SetEnable(enable);
    for(auto& b:EditingButtons)if(b)b->SetEnable(enable);
    // 为所有按钮设置编辑模式
    for (auto& button : buttons) {
        if (button)button->SetEditMode(enable);
    }
    
    // 为额外的可编辑按钮设置编辑模式
    for (auto& button : additionalEditableButtons) {
        if (button)button->SetEditMode(enable);
    }
    
    // 也为exitButton设置编辑模式
    if (exitButton)exitButton->SetEditMode(enable);
    
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
    
    // 检查额外的可编辑按钮
    for (auto& button : additionalEditableButtons) {
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
    
    // 处理额外的可编辑按钮
    for (auto& button : additionalEditableButtons) {
        if (button) {
            button->OnEditMouseMove(point);
        }
    }
}

void Screen::OnEditMouseUp(int x, int y) {
    if (!editModeEnabled) return;
    
    core::Point point(x, y, false);

    // 所有按钮都尝试处理释放事件
    exitButtonEdit->OnClick(point);

    for(auto &b:EditingButtons){
        if(b)b->OnClick(point);
    }

    if (exitButton) {
        exitButton->OnEditMouseUp(point);
    }
    
    for (auto& button : buttons) {
        if (button) {
            button->OnEditMouseUp(point);
        }
    }
    
    // 处理额外的可编辑按钮
    for (auto& button : additionalEditableButtons) {
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
    
    // 绘制额外的可编辑按钮的编辑覆盖层
    for (const auto& button : additionalEditableButtons) {
        if (button) {
            button->DrawEditOverlay();
        }
    }
    
    if (exitButtonEdit) {
        exitButtonEdit->Draw();
    }
    for(auto &b:EditingButtons){
        if(b)b->Draw();
    }
}

void Screen::SaveButtonLayout() {
    core::Config* config = core::Config::getInstance();
    
    // 保存exitButton的布局
    if (exitButton) {
        exitButton->SaveRegionToConfig();
    }
    for(auto& b: buttons){
        if(b)b->SaveRegionToConfig();
    }
    for(auto& b: additionalEditableButtons){
        if(b)b->SaveRegionToConfig();
    }
    
    // 保存配置到文件
    config->saveToFile();
    
    Log << "Button layout saved for screen " << static_cast<int>(ID) << op::endl;
}

// 管理额外可编辑按钮的方法实现
void Screen::RegisterEditableButton(std::shared_ptr<core::Button> button) {
    if (button) {
        // 避免重复添加
        auto it = std::find(additionalEditableButtons.begin(), additionalEditableButtons.end(), button);
        if (it == additionalEditableButtons.end()) {
            additionalEditableButtons.push_back(button);
            
            // 如果当前已经在编辑模式，为新按钮设置编辑模式
            if (editModeEnabled) button->SetEditMode(true);

            Log << "Button registered for editing" << op::endl;
        }
    }
}

void Screen::UnregisterEditableButton(std::shared_ptr<core::Button> button) {
    if (button) {
        auto it = std::find(additionalEditableButtons.begin(), additionalEditableButtons.end(), button);
        if (it != additionalEditableButtons.end()) {
            // 退出编辑模式
            button->SetEditMode(false);
            additionalEditableButtons.erase(it);
            
            // 如果这是当前选中的按钮，清除选择
            if (selectedButton == button) {
                selectedButton = nullptr;
            }
            
            Log << "Button unregistered from editing" << op::endl;
        }
    }
}

void Screen::ClearEditableButtons() {
    // 为所有额外按钮退出编辑模式
    for (auto& button : additionalEditableButtons) {
        if (button) {
            button->SetEditMode(false);
        }
    }
    
    additionalEditableButtons.clear();
    selectedButton = nullptr;
    
    Log << "All additional editable buttons cleared" << op::endl;
}

// 图像原比例吸附功能实现
void Screen::ToggleAspectRatioSnap() {
    if (!editModeEnabled) return;
    
    // 检查第一个按钮的吸附状态，用作基准
    bool currentState = false;
    if (!buttons.empty() && buttons[0]) {
        currentState = buttons[0]->IsAspectRatioSnapEnabled();
    }
    
    // 切换状态
    bool newState = !currentState;
    SetAspectRatioSnapForAllButtons(newState);
    
    Log << Level::Info << "Aspect ratio snap toggled: " << (newState ? "ON" : "OFF") << op::endl;
}

void Screen::SetAspectRatioSnapForAllButtons(bool enable) {
    if (!editModeEnabled) return;
    
    // 设置所有普通按钮的吸附状态
    for (auto& button : buttons) {
        if (button) {
            button->SetAspectRatioSnap(enable);
        }
    }
    
    // 设置所有额外可编辑按钮的吸附状态
    for (auto& button : additionalEditableButtons) {
        if (button) {
            button->SetAspectRatioSnap(enable);
        }
    }
    
    // 也设置退出按钮的吸附状态
    if (exitButton) {
        exitButton->SetAspectRatioSnap(enable);
    }
    if (exitButtonEdit) {
        exitButtonEdit->SetAspectRatioSnap(enable);
    }
    
    Log << Level::Info << "Aspect ratio snap set for all buttons: " << (enable ? "ON" : "OFF") << op::endl;
}