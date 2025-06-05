#include "core/screen/SettingScreen.h"
#include "core/log.h"
#include <chrono>
#include <algorithm>

#define NEXT "c"
#define LAST "b"

using namespace screen;
using namespace core;

void SettingScreen::init() {
    Screen::init();
    background=core::Explorer::getInstance()->getBitmap(BitmapID::SettingBg);
    std::shared_ptr<Button> next=std::make_shared<Button>(Button());
    std::shared_ptr<Button> last=std::make_shared<Button>(Button());
    std::shared_ptr<Button> back=std::make_shared<Button>(Button());
    next->SetFontID(FontID::Icon);
    next->SetRegion({0.94,0.905,0.97,0.935});
    next->SetColor(Color(0xFFB266));
    next->SetClickFunc([this]{
        changePage(true);
    });
    next->SetEnableBitmap(false);
    next->SetFontScale(0.25);
    next->SetText(NEXT);
    last->SetFontID(FontID::Icon);
    last->SetRegion({0.89,0.905,0.92,0.935});
    last->SetColor(Color(0xFFB266));
    last->SetClickFunc([this]{
        changePage(false);
    });
    last->SetEnableBitmap(false);
    last->SetFontScale(0.25);
    last->SetText(LAST);
    back->SetBitmap(BitmapID::Exit);
    back->SetClickFunc([]{Screen::SwitchToScreenWithFade(ScreenID::MainMenu);});
    back->SetRegion({0.9,0.03,0.95,-1});
    back->SetEnableText(false);
    back->SetEnableBitmap(true);
    back->SetEnableFill(false);
    back->SetText("返回");
    buttons.push_back(next);
    buttons.push_back(last);
    buttons.push_back(back);
}

void SettingScreen::Draw() {
    // 更新过渡状态
    updateTransition();
    
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
    
    // 更新页面切换动画
    updatePageTransition();
    
    if(background){
        background->Draw({0, 0, 1, 1}, 0.75f * alpha);
    }

    for (const auto& button : buttons) {
        if(button)
            button->Draw(static_cast<unsigned char>(alpha * 255.0f));
    }
    

    // 绘制设置按钮（包含动画效果）
    if (isTransitioning) {
        // 在过渡期间，先绘制当前页面（渐隐），再绘制目标页面（渐现）
        float currentAlpha = calculateTransitionAlpha(true);
        float targetAlpha = calculateTransitionAlpha(false);
        
        // 绘制当前页面的按钮
        for(const auto& button : s_buttons) {
            if(button) {
                button->Draw(currentPage, static_cast<unsigned char>(currentAlpha * 255.0f));
            }
        }
        
        // 绘制目标页面的按钮
        for(const auto& button : s_buttons) {
            if(button) {
                button->Draw(targetPage, static_cast<unsigned char>(targetAlpha * 255.0f));
            }
        }
    } else {
        // 没有动画时，正常绘制
        for(const auto& button : s_buttons) {
            if(button) {
                button->Draw(currentPage);
            }
        }
    }

    // 绘制标题（带动画效果）
    core::Font* font = core::Explorer::getInstance()->getFont(FontID::Normal);
    
    if (isTransitioning) {
        // 当前页面标题（渐隐）
        if(currentPage < titles.size()) {
            float alpha = calculateTransitionAlpha(true);
            font->RenderTextBetween(titles[currentPage], {0,0.05,1,0.1}, 0.7f, 
                Color(255, 255, 255, static_cast<unsigned char>(alpha * 255.0f)));
        }
        
        // 目标页面标题（渐现）
        if (targetPage < titles.size()) {
            float alpha = calculateTransitionAlpha(false);
            font->RenderTextBetween(titles[targetPage], {0,0.05,1,0.1}, 0.7f, 
                Color(255, 255, 255, static_cast<unsigned char>(alpha * 255.0f)));
        }
    } else {
        // 没有动画时，正常绘制标题
        if(currentPage < titles.size()) {
            font->RenderTextBetween(titles[currentPage], {0,0.05,1,0.1}, 0.7f, Color(255, 255, 255, 255));
        }
    }
    font->RenderTextBetween(std::to_string(currentPage + 1) + "/" + std::to_string(pages + 1), 
        {0.9, 0.9, 0.96, 0.93}, 0.3f, Color(255, 150, 20, 255));
}

void SettingScreen::enter(int) {
    Log << Level::Info << "进入设置界面" << op::endl;
    loadButtons();
}

bool SettingScreen::Click(int x, int y) {
    if(Screen::Click(x, y))return true;
    for(auto& button : s_buttons) if(button)
        if(button->Click(Point(x, y,false),currentPage))return true;
    for(auto& b:buttons)if(b)
        if(b->OnClick(Point(x,y,false)))return true;
    return false;
}

void SettingScreen::changePage(bool forward){
    // 如果正在进行页面切换动画，忽略新的切换请求
    if (isTransitioning) {
        return;
    }
    
    int newPage;
    if(forward){
        if(currentPage<pages)
            newPage = currentPage + 1;
        else 
            newPage = 0; // 如果已经是最后一页，则回到第一页
    }
    else{
        if(currentPage>0)
            newPage = currentPage - 1;
        else 
            newPage = pages; // 如果已经是第一页，则回到最后一页
    }
    
    // 如果目标页面和当前页面相同，直接返回
    if (newPage == currentPage) {
        return;
    }
      // 开始页面切换动画
    targetPage = newPage;
    isTransitioning = true;
    transitionStartTime = std::chrono::steady_clock::now();
    currentPageAlpha = 1.0f;
    targetPageAlpha = 0.0f;
    slideOffset = 0.0f;
    
    Log << Level::Info << "开始页面切换动画 (" << 
        (transitionType == TransitionType::Fade ? "淡入淡出" : "滑动") << 
        "): " << currentPage << " -> " << targetPage << op::endl;
}

void SettingScreen::updatePageTransition() {
    if (!isTransitioning) {
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - transitionStartTime).count();
    float progress = std::min(1.0f, elapsed / (transitionDuration * 1000.0f));
    
    // 使用更平滑的过渡曲线（ease-in-out）
    float smoothProgress;
    if (progress < 0.5f) {
        smoothProgress = 2.0f * progress * progress; // ease-in
    } else {
        smoothProgress = 1.0f - 2.0f * (1.0f - progress) * (1.0f - progress); // ease-out
    }
    
    if (progress >= 1.0f) {
        // 动画完成
        currentPage = targetPage;
        isTransitioning = false;
        currentPageAlpha = 1.0f;
        targetPageAlpha = 0.0f;
        slideOffset = 0.0f;
        
        Log << Level::Info << "页面切换动画完成，当前页: " << currentPage << op::endl;
    } else {
        // 根据动画类型计算参数
        if (transitionType == TransitionType::Fade) {
            // 淡入淡出动画
            currentPageAlpha = 1.0f - smoothProgress;
            targetPageAlpha = smoothProgress;
        } else {
            // 滑动动画 - 保持不透明，但改变位置
            currentPageAlpha = 1.0f;
            targetPageAlpha = 1.0f;
            slideOffset = smoothProgress; // 0.0 到 1.0 的滑动进度
        }
    }
}

float SettingScreen::calculateTransitionAlpha(bool isCurrentPage) {
    if (!isTransitioning) {
        return 1.0f; // 没有动画时，完全不透明
    }
    
    if (isCurrentPage) {
        return currentPageAlpha;
    } else {
        return targetPageAlpha;
    }
}