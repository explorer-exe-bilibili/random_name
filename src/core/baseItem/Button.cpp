#include "core/baseItem/Button.h"

#include "core/explorer.h"
#include "core/log.h"
#include "core/Drawer.h"
#include "core/Config.h"
#include <thread>
#include <chrono>

using namespace core;

Button::Button(const std::string& text, FontID fontid, const Region& region, Bitmap** bitmapPtr) :
 text(text), fontid(fontid), region(region), bitmapPtr(bitmapPtr) {}

Button::Button(const Button& button) :
    region(button.region),
    enableText(button.enableText),
    enable(button.enable),
    enableBitmap(button.enableBitmap),
    text(button.text),
    bitmapPtr(button.bitmapPtr),
    ClickFunc(button.ClickFunc),
    fontid(button.fontid) {}

Button& Button::operator=(const Button& button) {
    if (this != &button) {
        region = button.region;
        enableText = button.enableText;
        enable = button.enable;
        enableBitmap = button.enableBitmap;
        text = button.text;
        bitmapPtr = button.bitmapPtr;
        ClickFunc = button.ClickFunc;
        fontid = button.fontid;
    }
    return *this;
}

Button::~Button() {
    // 请求停止所有可能运行的动画线程
    stopRequested = true;
    fadeStopRequested = true;
    
    // 等待一小段时间，让线程有机会响应停止请求
    if (animationRunning || fadeAnimationRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // 清理资源
}

void Button::Draw(unsigned char alpha) {
    // Draw the button
    if(!enable)return;
    if(region.getxend() < 0 || region.getyend() < 0 || region.getx() > WindowInfo.width || region.gety() > WindowInfo.height)return;
    if(region.getWidth() <= 0 || region.getHeight() <= 0)return; // 确保区域有效

    // 如果淡出动画正在运行，使用淡出动画的alpha值，否则使用传入的alpha值
    unsigned char finalAlpha = alpha;
    if (fadeAnimationRunning) {
        finalAlpha = currentFadeAlpha.load();
    }
    if(bools[boolconfig::debug]){
        Drawer::getInstance()->DrawSquare(region, Color(255, 0, 0, 255));
    }
    if (enableBitmap) {
        if(bitmapPtr && *bitmapPtr)
            (*bitmapPtr)->Draw(region, finalAlpha/255.0f);
        else if(bitmapid != BitmapID::Unknown && core::Explorer::getInstance()->isBitmapLoaded(bitmapid)) {
            bitmapPtr = core::Explorer::getInstance()->getBitmapPtr(bitmapid);
            if (bitmapPtr && *bitmapPtr) {
                (*bitmapPtr)->Draw(region, finalAlpha / 255.0f);
            } else {
                Log << "Bitmap with ID " << int(bitmapid) << " not found." << op::endl;
            }
        } else {
            Log << "Bitmap pointer is null or bitmap not loaded.Button name:" << text << op::endl;
        }
    }
    if(enableFill) {
        Drawer::getInstance()->DrawSquare(region, fillColor,true);
    }
    if (enableText) {
        if (font) {
            Color tmp=color;
            tmp.a=finalAlpha;
            if(isCentered)font->RenderTextBetween(text, region, fontScale, tmp);
            else font->RenderText(text, region.getx(), region.gety(), fontScale, tmp);
        }
    }
    
}
bool core::Button::OnClick(Point point)
{
    if (point.getx() >= region.getx() && point.getx() <= region.getxend() && 
    point.gety() >= region.gety() && point.gety() <= region.getyend())
    {
        Log << "Button "<<text<<" clicked" << op::endl;
        if(audioid!=AudioID::Unknown && core::Explorer::getInstance()->isAudioLoaded(audioid)) {
            core::Explorer::getInstance()->playSound(audioid, 0);
        }
        if (ClickFunc) ClickFunc();
        else return false; // 如果没有设置点击函数，返回false
        return true;
    }
    return false;
}

void Button::MoveTo(const Region& region, const bool enableFluent, const float speed_, std::function<void()> onComplete)
{
    float speed=speed_*WindowInfo.width/800.0f*100;
      // 尝试在有限时间内获取锁，避免死锁
    std::unique_lock<std::timed_mutex> lock(animMutex, std::defer_lock);
    
    // 尝试获取锁，超时时间为1000ms
    if (!lock.try_lock_for(std::chrono::milliseconds(1000))) {
        Log << Level::Error << "Button::MoveTo - 无法获取动画互斥锁，可能存在死锁" << op::endl;
        // 执行回调函数（如果有）
        if (onComplete) {
            onComplete();
        }
        return;
    }
    
    // 先停止可能正在进行的动画
    if (animationRunning) {
        stopRequested = true;
        // 等待先前的动画线程意识到停止请求，但有超时限制
        lock.unlock();
        auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
        while (animationRunning && std::chrono::steady_clock::now() < timeout) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        if (animationRunning) {
            Log << Level::Warn << "Button::MoveTo - 强制停止动画" << op::endl;
            animationRunning = false;
        }
        
        // 重新获取锁
        if (!lock.try_lock_for(std::chrono::milliseconds(1000))) {
            Log << Level::Error << "Button::MoveTo - 无法重新获取动画互斥锁" << op::endl;
            if (onComplete) {
                onComplete();
            }
            return;
        }
    }
    if (enableFluent&&!bools[boolconfig::nosmoothui]){
        // 重置标志
        animationRunning = true;
        stopRequested = false;
        
        // 释放锁，让动画线程可以使用
        lock.unlock();
        
        // 创建一个shared_ptr来捕获this指针，确保对象在线程运行期间不会被销毁        
        std::shared_ptr<Button> self = std::shared_ptr<Button>(this, [](Button*){});  // 自定义删除器，不实际删除对象

        // 创建回调函数的副本，以便在线程中使用
        std::shared_ptr<std::function<void()>> callback = nullptr;
        if (onComplete) {
            callback = std::make_shared<std::function<void()>>(onComplete);
        }

        // 基于速度的平滑移动逻辑
        std::thread([self=std::move(self), targetRegion=region, speed, callback]{
            auto& button = *self;  // 通过引用访问按钮，简化语法
            
            Region startRegion = button.region;  // 保存起始位置
              // 计算总距离（考虑所有四个角的变化）
            float dx1 = (targetRegion.getx() - startRegion.getx()) * WindowInfo.width;
            float dy1 = (targetRegion.gety() - startRegion.gety()) * WindowInfo.height;
            float dx2 = (targetRegion.getxend() - startRegion.getxend()) * WindowInfo.width;
            float dy2 = (targetRegion.getyend() - startRegion.getyend()) * WindowInfo.height;
            
            // 使用最大的距离变化作为移动距离
            float distance1 = std::sqrt(dx1*dx1 + dy1*dy1);  // 左上角的距离
            float distance2 = std::sqrt(dx2*dx2 + dy2*dy2);  // 右下角的距离
            float distance = std::max(distance1, distance2);            // 如果距离太小，直接设置位置并返回
            if (distance < 1.0f) {
                std::unique_lock<std::timed_mutex> animLock(button.animMutex, std::defer_lock);
                if (animLock.try_lock_for(std::chrono::milliseconds(100))) {
                    button.region = targetRegion;
                    button.animationRunning = false;
                } else {
                    Log << Level::Warn << "Button::MoveTo - 无法获取锁设置最终位置" << op::endl;
                    button.animationRunning = false;
                }
                
                // 如果有回调函数，则执行它
                if (callback && *callback) {
                    try {
                        (*callback)();
                    } catch (const std::exception& e) {
                        Log << Level::Error << "Button::MoveTo - 回调函数执行异常: " << e.what() << op::endl;
                    }
                }
                
                return;
            }
            
            // 基于速度计算总时间（毫秒）
            int totalTime = static_cast<int>((distance / speed));
            float steps = 100.0f;  // 移动步数
            float delay = totalTime / steps;  // 每步之间的延迟
            
            startRegion = button.region;  // 保存起始位置
            
            for (int i = 0; i < steps && !button.stopRequested; ++i) {
                // 计算当前步骤应该在的位置
                float progress = float(i) / steps;
                float newX = startRegion.getx() + (targetRegion.getx() - startRegion.getx()) * progress;
                float newXend = startRegion.getxend() + (targetRegion.getxend() - startRegion.getxend()) * progress;
                float newY = startRegion.gety() + (targetRegion.gety() - startRegion.gety()) * progress;
                float newYend = startRegion.getyend() + (targetRegion.getyend() - startRegion.getyend()) * progress;
                newX/=WindowInfo.width;
                newXend/=WindowInfo.width;                newY/=WindowInfo.height;
                newYend/=WindowInfo.height;
                {
                    std::unique_lock<std::timed_mutex> animLock(button.animMutex, std::defer_lock);
                    if (animLock.try_lock_for(std::chrono::milliseconds(50))) {
                        if (!button.stopRequested) {
                            button.region.setx(newX);
                            button.region.setxend(newXend);
                            button.region.sety(newY);
                            button.region.setyend(newYend);
                        }
                    } else {
                        // 如果无法获取锁，记录警告但继续动画
                        Log << Level::Warn << "Button::MoveTo - 动画步骤锁获取超时" << op::endl;
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
            }              // 如果没有被请求停止，确保最终位置精确
            if (!button.stopRequested) {
                std::unique_lock<std::timed_mutex> animLock(button.animMutex, std::defer_lock);
                if (animLock.try_lock_for(std::chrono::milliseconds(100))) {
                    button.region = targetRegion;
                } else {
                    Log << Level::Warn << "Button::MoveTo - 无法获取锁设置最终位置" << op::endl;
                }
            }
            
            button.animationRunning = false;
            if(callback&&*callback) {
                try {
                    (*callback)();
                } catch (const std::exception& e) {
                    Log << Level::Error << "Button::MoveTo - 回调函数执行异常: " << e.what() << op::endl;
                }
            }
        }).detach();
    } 
    else {
        // 如果不使用流畅移动，直接设置位置
        // 如果之前已经获取了锁，直接使用
        if (!lock.owns_lock()) {
            if (!lock.try_lock_for(std::chrono::milliseconds(100))) {
                Log << Level::Error << "Button::MoveTo - 无法获取锁进行直接移动" << op::endl;
                if (onComplete) {
                    onComplete();
                }
                return;
            }
        }
        
        this->region = region;
        if (onComplete) {
            try {
                onComplete();
            } catch (const std::exception& e) {
                Log << Level::Error << "Button::MoveTo - 直接移动回调函数执行异常: " << e.what() << op::endl;
            }
        }
    }
}

void Button::SetFontID(FontID id)
{
    this->fontid = id;
    font = core::Explorer::getInstance()->getFont(fontid);
}

void Button::FadeOut(float duration, unsigned char startAlpha, unsigned char endAlpha, std::function<void()> onComplete)
{    // 先停止可能正在进行的淡出动画
    {
        std::lock_guard<std::timed_mutex> lock(fadeMutex);
        if (fadeAnimationRunning) {
            fadeStopRequested = true;
            // 等待先前的淡出动画线程意识到停止请求
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    // 创建一个shared_ptr来捕获this指针，确保对象在线程运行期间不会被销毁
    std::shared_ptr<Button> self = std::shared_ptr<Button>(this, [](Button*){});  // 自定义删除器，不实际删除对象

    // 创建回调函数的副本，以便在线程中使用
    std::shared_ptr<std::function<void()>> callback = nullptr;
    if (onComplete) {
        callback = std::make_shared<std::function<void()>>(onComplete);
    }

    fadeAnimationRunning = true;
    fadeStopRequested = false;
    currentFadeAlpha = startAlpha;

    // 基于时间的淡出动画逻辑
    std::thread([self=std::move(self), duration, startAlpha, endAlpha, callback]{
        auto& button = *self;  // 通过引用访问按钮，简化语法
          // 如果持续时间太小，直接设置最终alpha值并返回
        if (duration <= 0.0f) {
            std::lock_guard<std::timed_mutex> lock(button.fadeMutex);
            button.currentFadeAlpha = endAlpha;
            button.fadeAnimationRunning = false;
            
            // 如果有回调函数，则执行它
            if (callback && *callback) {
                (*callback)();
            }
            
            return;
        }
        
        // 计算动画参数
        int totalTimeMs = static_cast<int>(duration * 1000);  // 转换为毫秒
        float steps = 60.0f;  // 60步，约60FPS
        float delay = totalTimeMs / steps;  // 每步之间的延迟
        int alphaDiff = static_cast<int>(endAlpha) - static_cast<int>(startAlpha);
        
        for (int i = 0; i <= steps && !button.fadeStopRequested; ++i) {
            // 计算当前步骤应该的alpha值
            float progress = float(i) / steps;
            
            // 使用ease-in-out缓动函数使动画更平滑
            float easedProgress = progress < 0.5f ? 
                2.0f * progress * progress : 
                1.0f - 2.0f * (1.0f - progress) * (1.0f - progress);
            
            unsigned char newAlpha = static_cast<unsigned char>(
                startAlpha + alphaDiff * easedProgress
            );
              {
                std::lock_guard<std::timed_mutex> lock(button.fadeMutex);
                if (!button.fadeStopRequested) {
                    button.currentFadeAlpha = newAlpha;
                }
            }
            
            // 如果这是最后一步，不需要等待
            if (i < steps) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
            }
        }
          // 如果没有被请求停止，确保最终alpha值精确
        if (!button.fadeStopRequested) {
            std::lock_guard<std::timed_mutex> lock(button.fadeMutex);
            button.currentFadeAlpha = endAlpha;
        }
        
        button.fadeAnimationRunning = false;
        
        // 如果有回调函数，则执行它
        if (callback && *callback) {
            (*callback)();
        }
        
    }).detach();
}