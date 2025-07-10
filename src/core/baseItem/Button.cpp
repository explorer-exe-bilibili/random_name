#include "core/baseItem/Button.h"

#include "core/explorer.h"
#include "core/log.h"
#include "core/Drawer.h"
#include "core/Config.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>

using namespace core;

bool Button::enableAspectRatioSnap = true; // 默认开启吸附功能
float Button::aspectRatioSnapThreshold = 0.2f;
bool Button::enableCenterSnap = true; // 是否启用居中吸附功能
float Button::centerSnapThreshold = 10.0f; // 居中吸附阈值（像素）
bool Button::enableCustomSnap = true; // 自定义吸附功能默认开启
float Button::customSnapThreshold = 8.0f; // 自定义吸附阈值（像素）

Button::Button(const std::string& text, FontID fontid, const Region& region, Bitmap** bitmapPtr) :
 text(text), fontid(fontid), region(region), bitmapPtr(bitmapPtr), fontPtr(nullptr) {
     UpdateImageAspectRatio();
 }

Button::Button(const Button& button) :
    region(button.region),
    enableText(button.enableText),
    enable(button.enable),
    enableBitmap(button.enableBitmap),
    text(button.text),
    bitmapPtr(button.bitmapPtr),
    fontPtr(button.fontPtr),
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
        fontPtr = button.fontPtr;
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
    if (enableText && fontPtr && *fontPtr) {
        Color tmp=color;
        tmp.a=finalAlpha;
        if(isCentered)(*fontPtr)->RenderTextBetween(text, region, fontScale, tmp);
        else (*fontPtr)->RenderText(text, region.getx(), region.gety(), fontScale, tmp);
    }
    
}
bool core::Button::OnClick(Point point)
{
    if (point.getx() >= region.getx() && point.getx() <= region.getxend() && 
    point.gety() >= region.gety() && point.gety() <= region.getyend())
    {
        Log << Level::Info << "Button "<<text<<" clicked" << op::endl;
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
    this->fontPtr = core::Explorer::getInstance()->getFontPtr(id);
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

// 编辑模式方法实现
void Button::SetEditMode(bool enable) {
    editModeEnabled = enable;
    UpdateEditHandles();
}

void Button::SetOnEditComplete(std::function<void(const Region&)> callback) {
    onEditComplete = callback;
}

void Button::UpdateEditHandles() {
    if (!editModeEnabled) {
        editHandles.clear();
        return;
    }
    editHandles.clear();
    float handleSize = editHandleSize;
    float halfH = handleSize / 2.0f;
    float l = region.getx();
    float r = region.getxend();
    float t = region.gety();
    float b = region.getyend();
    float cx = (l + r) * 0.5f;
    float cy = (t + b) * 0.5f;
    // 角落
    editHandles.emplace_back(Region(l-halfH, t-halfH, l+halfH, t+halfH, false), EditMode::ResizeTopLeft);
    editHandles.emplace_back(Region(r-halfH, t-halfH, r+halfH, t+halfH, false), EditMode::ResizeTopRight);
    editHandles.emplace_back(Region(l-halfH, b-halfH, l+halfH, b+halfH, false), EditMode::ResizeBottomLeft);
    editHandles.emplace_back(Region(r-halfH, b-halfH, r+halfH, b+halfH, false), EditMode::ResizeBottomRight);
    // 边框中点
    editHandles.emplace_back(Region(cx-halfH, t-halfH, cx+halfH, t+halfH, false), EditMode::ResizeTop);
    editHandles.emplace_back(Region(cx-halfH, b-halfH, cx+halfH, b+halfH, false), EditMode::ResizeBottom);
    editHandles.emplace_back(Region(l-halfH, cy-halfH, l+halfH, cy+halfH, false), EditMode::ResizeLeft);
    editHandles.emplace_back(Region(r-halfH, cy-halfH, r+halfH, cy+halfH, false), EditMode::ResizeRight);
}

EditMode Button::GetEditModeAt(Point point) const {
    if (!editModeEnabled) return EditMode::None;
    for (const auto& h : editHandles) {
        if (IsPointInHandle(point, h)) return h.mode;
    }
    if (point.getx() >= region.getx() && point.getx() <= region.getxend() &&
        point.gety() >= region.gety() && point.gety() <= region.getyend()) {
        return EditMode::Move;
    }
    return EditMode::None;
}

bool Button::IsPointInHandle(Point point, const EditHandle& handle) const {
    return point.getx() >= handle.region.getx() && point.getx() <= handle.region.getxend() &&
           point.gety() >= handle.region.gety() && point.gety() <= handle.region.getyend();
}

bool Button::OnEditMouseDown(Point point) {
    if (!editModeEnabled) return false;
    if (!enable) return false; // 如果按钮不可用，忽略编辑事件
    
    // 检查是否点击了编辑手柄
    for (const auto& handle : editHandles) {
        if (IsPointInHandle(point, handle)) {
            Log << "Button " << text << " edit handle clicked: " << int(handle.mode) << op::endl;
            currentEditMode = handle.mode;
            isDragging = true;
            dragStartPoint = point;
            originalRegion = region;
            return true;
        }
    }
    
    // 检查是否点击了按钮内部（移动模式）
    if (point.getx() >= region.getx() && point.getx() <= region.getxend() && 
        point.gety() >= region.gety() && point.gety() <= region.getyend()) {
        Log << "Button " << text << " edit move started" << op::endl;
        currentEditMode = EditMode::Move;
        isDragging = true;
        dragStartPoint = point;
        originalRegion = region;
        return true;
    }
    
    return false;
}

bool Button::OnEditMouseMove(Point point) {
    if (!editModeEnabled) return false;
    if (!enable) return false; // 如果按钮不可用，忽略编辑事件

    if (isDragging && currentEditMode != EditMode::None) {
        UpdateRegionFromEdit(point);
        UpdateEditHandles();
        return true;
    }
    
    return false;
}

bool Button::OnEditMouseUp(Point point) {
    if (!editModeEnabled) return false;
    if (!enable) return false; // 如果按钮不可用，忽略编辑事件
    
    if (isDragging && currentEditMode != EditMode::None) {
        Log << "Button " << text << " edit completed" << op::endl;
        isDragging = false;
        if (onEditComplete) onEditComplete(region);
        currentEditMode = EditMode::None;
        return true;
    }
    
    return false;
}

void Button::UpdateRegionFromEdit(Point cur) {
    float dx = cur.getx() - dragStartPoint.getx();
    float dy = cur.gety() - dragStartPoint.gety();
    float dxr = dx, dyr = dy;
    if (region.isScreenRatio()) { dxr = dx/WindowInfo.width; dyr = dy/WindowInfo.height; }
    Region nr = originalRegion;
    
    // 检查是否是1:1比例模式
    bool was1to1 = originalRegion.isAspectRatio1to1();
    
    switch (currentEditMode) {
        case EditMode::Move:
            nr.setx(originalRegion.getOriginX()+dxr);
            nr.sety(originalRegion.getOriginY()+dyr);
            nr.setxend(originalRegion.getOriginXEnd()+dxr);
            if (!was1to1) {
                nr.setyend(originalRegion.getOriginYEnd()+dyr);
            }
            break;
        case EditMode::ResizeTopLeft:
            nr.setx(originalRegion.getOriginX()+dxr);
            nr.sety(originalRegion.getOriginY()+dyr);
            if (was1to1) {
                // 1:1比例模式下，保持正方形
                float newWidth = originalRegion.getOriginW() - dxr;
                float newHeight = newWidth;
                nr.setxend(nr.getOriginX() + newWidth);
            }
            break;
        case EditMode::ResizeTopRight:
            nr.setxend(originalRegion.getOriginXEnd()+dxr);
            nr.sety(originalRegion.getOriginY()+dyr);
            if (was1to1) {
                // 1:1比例模式下，保持正方形
                float newWidth = originalRegion.getOriginW() + dxr;
                float newHeight = newWidth;
                nr.sety(nr.getOriginYEnd() - newHeight);
            }
            break;
        case EditMode::ResizeBottomLeft:
            nr.setx(originalRegion.getOriginX()+dxr);
            if (was1to1) {
                // 1:1比例模式下，保持正方形
                float newWidth = originalRegion.getOriginW() - dxr;
                nr.setxend(nr.getOriginX() + newWidth);
            } else {
                nr.setyend(originalRegion.getOriginYEnd()+dyr);
            }
            break;
        case EditMode::ResizeBottomRight:
            nr.setxend(originalRegion.getOriginXEnd()+dxr);
            if (!was1to1) {
                nr.setyend(originalRegion.getOriginYEnd()+dyr);
            }
            break;
        case EditMode::ResizeLeft:
            nr.setx(originalRegion.getOriginX()+dxr);
            if (was1to1) {
                // 1:1比例模式下，调整宽度时保持正方形
                float newWidth = originalRegion.getOriginW() - dxr;
                nr.setxend(nr.getOriginX() + newWidth);
            }
            break;
        case EditMode::ResizeRight:
            nr.setxend(originalRegion.getOriginXEnd()+dxr);
            break;
        case EditMode::ResizeTop:
            if (!was1to1) {
                nr.sety(originalRegion.getOriginY()+dyr);
            }
            break;
        case EditMode::ResizeBottom:
            if (!was1to1) {
                nr.setyend(originalRegion.getOriginYEnd()+dyr);
            }
            break;
        default: break;
    }
    
    // 图像原比例吸附逻辑（仅在非1:1比例模式下应用）
    if (!was1to1 && enableAspectRatioSnap && hasValidImageAspectRatio) {
        float currentAspectRatio = nr.getRatio(); // 使用像素比例
        if (ShouldSnapToAspectRatio(currentAspectRatio)) {
            ApplyAspectRatioSnap(nr);
            Log << Level::Debug << "Button " << text << " snapped to original aspect ratio: " 
                << originalImageAspectRatio << op::endl;
        }
    }

    // 自定义吸附点逻辑（优先级最高，仅在移动模式下应用）
    float snapX = -1.0f, snapY = -1.0f;
    if (currentEditMode == EditMode::Move && enableCustomSnap) {
        if (ShouldSnapToCustomPoints(nr, snapX, snapY)) {
            ApplyCustomSnap(nr, snapX, snapY);
            isSnappedToCustom = true;
            isSnappedToCenter = false; // 自定义吸附优先于居中吸附
            Log << Level::Debug << "Button " << text << " snapped to custom points (X:" 
                << snapX << ", Y:" << snapY << ")" << op::endl;
        } else {
            isSnappedToCustom = false;
        }
    } else {
        isSnappedToCustom = false;
    }

    // 居中自动吸附逻辑（仅在移动模式下应用，且未触发自定义吸附）
    if (currentEditMode == EditMode::Move && enableCenterSnap && !isSnappedToCustom) {
        if (ShouldSnapToCenter(nr)) {
            ApplyCenterSnap(nr);
            isSnappedToCenter = true;
            Log << Level::Debug << "Button " << text << " snapped to center" << op::endl;
        } else {
            isSnappedToCenter = false;
        }
    } else {
        isSnappedToCenter = false;
    }
    
    // 确保保持1:1比例标志
    if (was1to1) {
        nr.setAspectRatio1to1(true);
    }
    
    float minW = minWidth, minH = minHeight;
    if (region.isScreenRatio()) { minW = minWidth/WindowInfo.width; minH = minHeight/WindowInfo.height; }
    
    // 对于1:1比例，确保最小尺寸是正方形
    if (was1to1) {
        float minSize = std::max(minW, minH);
        if (nr.getOriginW() < minSize) {
            if (currentEditMode==EditMode::ResizeLeft || currentEditMode==EditMode::ResizeTopLeft || currentEditMode==EditMode::ResizeBottomLeft) {
                nr.setx(nr.getOriginXEnd()-minSize);
            } else {
                nr.setxend(nr.getOriginX()+minSize);
            }
        }
    } else {
        // 非1:1比例的原有逻辑
        if (nr.getOriginW() < minW) {
            if (currentEditMode==EditMode::ResizeLeft || currentEditMode==EditMode::ResizeTopLeft || currentEditMode==EditMode::ResizeBottomLeft) {
                nr.setx(nr.getOriginXEnd()-minW);
            } else {
                nr.setxend(nr.getOriginX()+minW);
            }
        }
        if (nr.getOriginH() < minH) {
            if (currentEditMode==EditMode::ResizeTop || currentEditMode==EditMode::ResizeTopLeft || currentEditMode==EditMode::ResizeTopRight) {
                nr.sety(nr.getOriginYEnd()-minH);
            } else {
                nr.setyend(nr.getOriginY()+minH);
            }
        }
    }
    
    region = nr;
    ClampRegion();
}

void Button::ClampRegion() {
    float maxX = region.isScreenRatio()?1.0f:WindowInfo.width;
    float maxY = region.isScreenRatio()?1.0f:WindowInfo.height;
    if (region.getOriginX() < 0) {
        float off = -region.getOriginX(); region.setx(0); region.setxend(region.getOriginXEnd()+off);
    }
    if (region.getOriginY() < 0) {
        float off = -region.getOriginY(); region.sety(0); region.setyend(region.getOriginYEnd()+off);
    }
    if (region.getOriginXEnd() > maxX) {
        float off = region.getOriginXEnd()-maxX; region.setxend(maxX); region.setx(region.getOriginX()-off);
    }
    if (region.getOriginYEnd() > maxY) {
        float off = region.getOriginYEnd()-maxY; region.setyend(maxY); region.sety(region.getOriginY()-off);
    }
}

void Button::DrawEditOverlay() {
    if (!editModeEnabled) return;
    if (!enable) return;
    Drawer* d = Drawer::getInstance();
    
    // 根据吸附状态选择边框颜色
    Color bc = editBorderColor;
    
    // 优先显示自定义吸附状态
    if (enableCustomSnap && isSnappedToCustom) {
        // 自定义吸附激活时使用青色边框
        bc = Color(0, 255, 255, 200);
    } else if (enableCenterSnap && isSnappedToCenter) {
        // 居中吸附激活时使用紫色边框
        bc = Color(128, 0, 128, 200);
    } else if (enableAspectRatioSnap && hasValidImageAspectRatio) {
        float currentAspectRatio = region.getRatio(); // 使用像素比例
        if (ShouldSnapToAspectRatio(currentAspectRatio)) {
            // 吸附激活时使用绿色边框
            bc = Color(0, 255, 0, 200);
        } else {
            // 吸附启用但未激活时使用橙色边框
            bc = Color(255, 165, 0, 200);
        }
    } else {
        bc.a = 200;
    }
    
    float bw = editBorderWidth;
    Region lb(region.getx()-bw/2, region.gety()-bw, region.getx()+bw/2, region.getyend()+bw, false);
    Region rb(region.getxend()-bw/2, region.gety()-bw, region.getxend()+bw/2, region.getyend()+bw, false);
    Region tb(region.getx()-bw, region.gety()-bw/2, region.getxend()+bw, region.gety()+bw/2, false);
    Region bb(region.getx()-bw, region.getyend()-bw/2, region.getxend()+bw, region.getyend()+bw/2, false);
    d->DrawSquare(lb, bc, true);
    d->DrawSquare(rb, bc, true);
    d->DrawSquare(tb, bc, true);
    d->DrawSquare(bb, bc, true);
    
    // 绘制自定义吸附点辅助线
    if (enableCustomSnap && isDragging && currentEditMode == EditMode::Move) {
        Color customGuideLineColor = Color(0, 255, 255, 128); // 半透明青色
        float lineWidth = 1.0f;
        
        // 按钮中心点
        float buttonCenterX = (region.getx() + region.getxend()) * 0.5f;
        float buttonCenterY = (region.gety() + region.getyend()) * 0.5f;
        
        // 绘制X轴吸附点的垂直辅助线
        for (float snapX : customSnapX) {
            snapX = snapX * WindowInfo.width; // 转换为像素坐标
            float distanceX = std::abs(buttonCenterX - snapX);
            if (distanceX <= customSnapThreshold * 2) {
                Region vLine(snapX - lineWidth/2, 0, snapX + lineWidth/2, WindowInfo.height, false);
                d->DrawSquare(vLine, customGuideLineColor, true);
            }
        }
        
        // 绘制Y轴吸附点的水平辅助线
        for (float snapY : customSnapY) {
            snapY = snapY * WindowInfo.height; // 转换为像素坐标
            float distanceY = std::abs(buttonCenterY - snapY);
            if (distanceY <= customSnapThreshold * 2) {
                Region hLine(0, snapY - lineWidth/2, WindowInfo.width, snapY + lineWidth/2, false);
                d->DrawSquare(hLine, customGuideLineColor, true);
            }
        }
    }

    // 绘制居中吸附辅助线
    if (enableCenterSnap && isDragging && currentEditMode == EditMode::Move) {
        Color guideLineColor = Color(128, 0, 128, 128); // 半透明紫色
        float lineWidth = 1.0f;
        
        // 屏幕中心点
        float screenCenterX = WindowInfo.width * 0.5f;
        float screenCenterY = WindowInfo.height * 0.5f;
        
        // 按钮中心点
        float buttonCenterX = (region.getx() + region.getxend()) * 0.5f;
        float buttonCenterY = (region.gety() + region.getyend()) * 0.5f;
        
        // 计算距离
        float distanceX = std::abs(buttonCenterX - screenCenterX);
        float distanceY = std::abs(buttonCenterY - screenCenterY);
        
        // 如果接近水平中心，绘制垂直辅助线
        if (distanceX <= centerSnapThreshold * 2) {
            Region vLine(screenCenterX - lineWidth/2, 0, screenCenterX + lineWidth/2, WindowInfo.height, false);
            d->DrawSquare(vLine, guideLineColor, true);
        }
        
        // 如果接近垂直中心，绘制水平辅助线
        if (distanceY <= centerSnapThreshold * 2) {
            Region hLine(0, screenCenterY - lineWidth/2, WindowInfo.width, screenCenterY + lineWidth/2, false);
            d->DrawSquare(hLine, guideLineColor, true);
        }
    }
    
    Color hc = editHandleColor;
    Color hb = Color(255,255,255,255);
    for (const auto& h : editHandles) {
        if (h.visible) {
            d->DrawSquare(h.region, hc, true);
            d->DrawSquare(h.region, hb, false);
        }
    }
}

void Button::resetRegion() {
    if (!regionConfig.empty()) {
        this->region = Config::getInstance()->getRegion(regionConfig);
        this->MoveTo(this->region);
    }
    UpdateEditHandles();
}

// 图像原比例吸附功能实现
void Button::UpdateImageAspectRatio() {
    hasValidImageAspectRatio = false;
    originalImageAspectRatio = 1.0f;

    if (bitmapPtr && *bitmapPtr) {
        unsigned int width = (*bitmapPtr)->getWidth();
        unsigned int height = (*bitmapPtr)->getHeight();
        if (width > 0 && height > 0) {
            originalImageAspectRatio = static_cast<float>(width) / static_cast<float>(height);
            hasValidImageAspectRatio = true;
            Log << Level::Info << "Button " << text << " image aspect ratio updated: " 
                << originalImageAspectRatio << " (" << width << "x" << height << ")" << op::endl;
        }
    } else if (bitmapid != BitmapID::Unknown && core::Explorer::getInstance()->isBitmapLoaded(bitmapid)) {
        // 尝试从bitmapid获取bitmap
        Bitmap** tmpPtr = core::Explorer::getInstance()->getBitmapPtr(bitmapid);
        if (tmpPtr && *tmpPtr) {
            unsigned int width = (*tmpPtr)->getWidth();
            unsigned int height = (*tmpPtr)->getHeight();
            if (width > 0 && height > 0) {
                originalImageAspectRatio = static_cast<float>(width) / static_cast<float>(height);
                hasValidImageAspectRatio = true;
                Log << Level::Info << "Button " << text << " image aspect ratio updated from ID: " 
                    << originalImageAspectRatio << " (" << width << "x" << height << ")" << op::endl;
            }
        }
    }
}

bool Button::ShouldSnapToAspectRatio(float currentAspectRatio) const {
    if (!enableAspectRatioSnap || !hasValidImageAspectRatio) {
        return false;
    }
    
    float diff = std::abs(currentAspectRatio - originalImageAspectRatio);
    return diff <= aspectRatioSnapThreshold;
}

void Button::ApplyAspectRatioSnap(Region& targetRegion) const {
    if (!hasValidImageAspectRatio) {
        return;
    }
    
    float currentWidth = targetRegion.getWidth();   // 像素宽度
    float currentHeight = targetRegion.getHeight(); // 像素高度
    
    // 根据编辑模式确定调整策略
    switch (currentEditMode) {
        case EditMode::ResizeTopLeft:
            // 左上角调整：保持右下角不变，根据宽度调整高度
            {
                float targetHeight = currentWidth / originalImageAspectRatio;
                float newTop = targetRegion.getyend() - targetHeight;
                targetRegion.sety(targetRegion.isScreenRatio() ? newTop / WindowInfo.height : newTop);
            }
            break;
        case EditMode::ResizeTopRight:
            // 右上角调整：保持左下角不变，根据宽度调整高度
            {
                float targetHeight = currentWidth / originalImageAspectRatio;
                float newTop = targetRegion.getyend() - targetHeight;
                targetRegion.sety(targetRegion.isScreenRatio() ? newTop / WindowInfo.height : newTop);
            }
            break;
        case EditMode::ResizeBottomLeft:
            // 左下角调整：保持右上角不变，根据宽度调整高度
            {
                float targetHeight = currentWidth / originalImageAspectRatio;
                float newBottom = targetRegion.gety() + targetHeight;
                targetRegion.setyend(targetRegion.isScreenRatio() ? newBottom / WindowInfo.height : newBottom);
            }
            break;
        case EditMode::ResizeBottomRight:
            // 右下角调整：保持左上角不变，根据宽度调整高度
            {
                float targetHeight = currentWidth / originalImageAspectRatio;
                float newBottom = targetRegion.gety() + targetHeight;
                targetRegion.setyend(targetRegion.isScreenRatio() ? newBottom / WindowInfo.height : newBottom);
            }
            break;
        case EditMode::ResizeLeft:
            // 左边调整：根据新宽度计算高度，保持中心位置
            {
                float targetHeight = currentWidth / originalImageAspectRatio;
                float centerY = (targetRegion.gety() + targetRegion.getyend()) * 0.5f;
                float halfHeight = targetHeight * 0.5f;
                if (targetRegion.isScreenRatio()) {
                    halfHeight /= WindowInfo.height;
                }
                targetRegion.sety(centerY - halfHeight);
                targetRegion.setyend(centerY + halfHeight);
            }
            break;
        case EditMode::ResizeRight:
            // 右边调整：根据新宽度计算高度，保持中心位置
            {
                float targetHeight = currentWidth / originalImageAspectRatio;
                float centerY = (targetRegion.gety() + targetRegion.getyend()) * 0.5f;
                float halfHeight = targetHeight * 0.5f;
                if (targetRegion.isScreenRatio()) {
                    halfHeight /= WindowInfo.height;
                }
                targetRegion.sety(centerY - halfHeight);
                targetRegion.setyend(centerY + halfHeight);
            }
            break;
        case EditMode::ResizeTop:
            // 上边调整：根据当前高度计算应有的宽度，保持水平中心
            {
                float targetWidth = currentHeight * originalImageAspectRatio;
                float centerX = (targetRegion.getx() + targetRegion.getxend()) * 0.5f;
                float halfWidth = targetWidth * 0.5f;
                if (targetRegion.isScreenRatio()) {
                    halfWidth /= WindowInfo.width;
                }
                targetRegion.setx(centerX - halfWidth);
                targetRegion.setxend(centerX + halfWidth);
            }
            break;
        case EditMode::ResizeBottom:
            // 下边调整：根据当前高度计算应有的宽度，保持水平中心
            {
                float targetWidth = currentHeight * originalImageAspectRatio;
                float centerX = (targetRegion.getx() + targetRegion.getxend()) * 0.5f;
                float halfWidth = targetWidth * 0.5f;
                if (targetRegion.isScreenRatio()) {
                    halfWidth /= WindowInfo.width;
                }
                targetRegion.setx(centerX - halfWidth);
                targetRegion.setxend(centerX + halfWidth);
            }
            break;
        default:
            // 默认调整底部
            {
                float targetHeight = currentWidth / originalImageAspectRatio;
                float newBottom = targetRegion.gety() + targetHeight;
                targetRegion.setyend(targetRegion.isScreenRatio() ? newBottom / WindowInfo.height : newBottom);
            }
            break;
    }
}

// 居中自动吸附功能实现
bool Button::ShouldSnapToCenter(const Region& targetRegion) const {
    if (!enableCenterSnap) {
        return false;
    }
    
    // 计算按钮中心点（像素坐标）
    float buttonCenterX = (targetRegion.getx() + targetRegion.getxend()) * 0.5f;
    float buttonCenterY = (targetRegion.gety() + targetRegion.getyend()) * 0.5f;
    
    // 计算屏幕中心点（像素坐标）
    float screenCenterX = WindowInfo.width * 0.5f;
    float screenCenterY = WindowInfo.height * 0.5f;
    
    // 计算距离
    float distanceX = std::abs(buttonCenterX - screenCenterX);
    float distanceY = std::abs(buttonCenterY - screenCenterY);
    
    // 检查是否在吸附阈值内（水平或垂直方向任一满足即可）
    return (distanceX <= centerSnapThreshold) || (distanceY <= centerSnapThreshold);
}

void Button::ApplyCenterSnap(Region& targetRegion) const {
    // 计算按钮中心点（像素坐标）
    float buttonCenterX = (targetRegion.getx() + targetRegion.getxend()) * 0.5f;
    float buttonCenterY = (targetRegion.gety() + targetRegion.getyend()) * 0.5f;
    
    // 计算屏幕中心点（像素坐标）
    float screenCenterX = WindowInfo.width * 0.5f;
    float screenCenterY = WindowInfo.height * 0.5f;
    
    // 计算按钮尺寸
    float buttonWidth = targetRegion.getWidth();
    float buttonHeight = targetRegion.getHeight();
    
    // 计算距离
    float distanceX = std::abs(buttonCenterX - screenCenterX);
    float distanceY = std::abs(buttonCenterY - screenCenterY);
    
    // 如果水平距离在阈值内，吸附到水平中心
    if (distanceX <= centerSnapThreshold) {
        float newLeft = screenCenterX - buttonWidth * 0.5f;
        float newRight = screenCenterX + buttonWidth * 0.5f;
        
        if (targetRegion.isScreenRatio()) {
            targetRegion.setx(newLeft / WindowInfo.width);
            targetRegion.setxend(newRight / WindowInfo.width);
        } else {
            targetRegion.setx(newLeft);
            targetRegion.setxend(newRight);
        }
    }
    
    // 如果垂直距离在阈值内，吸附到垂直中心
    if (distanceY <= centerSnapThreshold) {
        float newTop = screenCenterY - buttonHeight * 0.5f;
        float newBottom = screenCenterY + buttonHeight * 0.5f;
        
        if (targetRegion.isScreenRatio()) {
            targetRegion.sety(newTop / WindowInfo.height);
            targetRegion.setyend(newBottom / WindowInfo.height);
        } else {
            targetRegion.sety(newTop);
            targetRegion.setyend(newBottom);
        }
    }
}

// 自定义吸附点管理方法
void Button::AddCustomSnapX(float x) {
    // 检查是否已经存在这个X值
    auto it = std::find_if(customSnapX.begin(), customSnapX.end(), 
        [x](float existing) { return std::abs(existing - x) < 0.01f; });

    if (it == customSnapX.end()) {
        customSnapX.push_back(x);
        std::sort(customSnapX.begin(), customSnapX.end());
        Log << Level::Debug << "Added custom snap X: " << x << op::endl;
    }
}

void Button::AddCustomSnapY(float y) {
    // 检查是否已经存在这个Y值
    auto it = std::find_if(customSnapY.begin(), customSnapY.end(), 
        [y](float existing) { return std::abs(existing - y) < 0.01f; });
    
    if (it == customSnapY.end()) {
        customSnapY.push_back(y);
        std::sort(customSnapY.begin(), customSnapY.end());
        Log << Level::Debug << "Added custom snap Y: " << y << op::endl;
    }
}

void Button::RemoveCustomSnapX(float x) {
    auto it = std::remove_if(customSnapX.begin(), customSnapX.end(), 
        [x](float existing) { return std::abs(existing - x) < 0.01f; });
    
    if (it != customSnapX.end()) {
        customSnapX.erase(it, customSnapX.end());
        Log << Level::Debug << "Removed custom snap X: " << x << op::endl;
    }
}

void Button::RemoveCustomSnapY(float y) {
    auto it = std::remove_if(customSnapY.begin(), customSnapY.end(), 
        [y](float existing) { return std::abs(existing - y) < 0.01f; });
    
    if (it != customSnapY.end()) {
        customSnapY.erase(it, customSnapY.end());
        Log << Level::Debug << "Removed custom snap Y: " << y << op::endl;
    }
}

void Button::ClearCustomSnapX() {
    customSnapX.clear();
    Log << Level::Debug << "Cleared all custom snap X points" << op::endl;
}

void Button::ClearCustomSnapY() {
    customSnapY.clear();
    Log << Level::Debug << "Cleared all custom snap Y points" << op::endl;
}

// 自定义吸附点检测和应用方法
bool Button::ShouldSnapToCustomPoints(const Region& targetRegion, float& snapX, float& snapY) const {
    if (!enableCustomSnap) {
        return false;
    }
    
    // 计算按钮中心点（像素坐标）
    float buttonCenterX = (targetRegion.getx() + targetRegion.getxend()) * 0.5f;
    float buttonCenterY = (targetRegion.gety() + targetRegion.getyend()) * 0.5f;
    
    bool hasSnapX = false, hasSnapY = false;
    snapX = -1.0f;
    snapY = -1.0f;
    
    // 检查X轴吸附点
    float minDistanceX = customSnapThreshold + 1.0f;
    for (float x : customSnapX) {
        x = x * WindowInfo.width; // 转换为像素坐标
        float distance = std::abs(buttonCenterX - x);
        if (distance <= customSnapThreshold && distance < minDistanceX) {
            minDistanceX = distance;
            snapX = x;
            hasSnapX = true;
        }
    }
    
    // 检查Y轴吸附点
    float minDistanceY = customSnapThreshold + 1.0f;
    for (float y : customSnapY) {
        y = y * WindowInfo.height; // 转换为像素坐标
        float distance = std::abs(buttonCenterY - y);
        if (distance <= customSnapThreshold && distance < minDistanceY) {
            minDistanceY = distance;
            snapY = y;
            hasSnapY = true;
        }
    }
    
    return hasSnapX || hasSnapY;
}

void Button::ApplyCustomSnap(Region& targetRegion, float snapX, float snapY) const {
    // 计算按钮尺寸
    float buttonWidth = targetRegion.getWidth();
    float buttonHeight = targetRegion.getHeight();
    
    // 如果有X轴吸附点，调整水平位置
    if (snapX >= 0.0f) {
        float newLeft = snapX - buttonWidth * 0.5f;
        float newRight = snapX + buttonWidth * 0.5f;
        
        if (targetRegion.isScreenRatio()) {
            targetRegion.setx(newLeft / WindowInfo.width);
            targetRegion.setxend(newRight / WindowInfo.width);
        } else {
            targetRegion.setx(newLeft);
            targetRegion.setxend(newRight);
        }
    }
    
    // 如果有Y轴吸附点，调整垂直位置
    if (snapY >= 0.0f) {
        float newTop = snapY - buttonHeight * 0.5f;
        float newBottom = snapY + buttonHeight * 0.5f;
        
        if (targetRegion.isScreenRatio()) {
            targetRegion.sety(newTop / WindowInfo.height);
            targetRegion.setyend(newBottom / WindowInfo.height);
        } else {
            targetRegion.sety(newTop);
            targetRegion.setyend(newBottom);
        }
    }
}