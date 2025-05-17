#include "core/baseItem/Button.h"

#include "core/explorer.h"
#include "core/log.h"
#include "core/Drawer.h"

using namespace core;

Button::Button(const std::string& text, int FontID, const Region& region, const std::shared_ptr<core::Bitmap>& bitmap) :
 text(text), FontID(FontID), region(region), bitmap(bitmap){}

Button::Button(const Button& button) :
    region(button.region),
    enableText(button.enableText),
    enable(button.enable),
    enableBitmap(button.enableBitmap),
    text(button.text),
    bitmap(button.bitmap),
    ClickFunc(button.ClickFunc),
    FontID(button.FontID) {}

Button& Button::operator=(const Button& button) {
    if (this != &button) {
        region = button.region;
        enableText = button.enableText;
        enable = button.enable;
        enableBitmap = button.enableBitmap;
        text = button.text;
        bitmap = button.bitmap;
        ClickFunc = button.ClickFunc;
        FontID = button.FontID;
    }
    return *this;
}

Button::~Button() {
    // 请求停止所有可能运行的动画线程
    stopRequested = true;
    
    // 等待一小段时间，让线程有机会响应停止请求
    if (animationRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // 清理资源
}

void Button::Draw() {
    // Draw the button
    if(Debugging){
        Drawer::getInstance()->DrawSquare(region, Color(255, 0, 0, 255));
    }
    if (enableBitmap && bitmap) {
        bitmap->Draw(region);
    }
    if (enableText) {
        if (font) {
            font->RenderTextBetween(text, region, 1.0f, color);
        }
    }
    
}
bool core::Button::OnClick(Point point)
{
    if (point.getx() >= region.getx() && point.getx() <= region.getxend() && 
    point.gety() >= region.gety() && point.gety() <= region.getyend())
    {
        Log << "Button "<<text<<" clicked" << op::endl;
        if (ClickFunc) ClickFunc();
        return true;
    }
    return false;
}

void Button::MoveTo(const Region& region, const bool enableFluent, const int time)
{
    // 先停止可能正在进行的动画
    {
        std::lock_guard<std::mutex> lock(animMutex);
        if (animationRunning) {
            stopRequested = true;
            // 等待先前的动画线程意识到停止请求
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    if (enableFluent)
    {
        // 创建一个shared_ptr来捕获this指针，确保对象在线程运行期间不会被销毁
        std::shared_ptr<Button> self = std::shared_ptr<Button>(this, [](Button*){});  // 自定义删除器，不实际删除对象

        animationRunning = true;
        stopRequested = false;

        // Smooth movement logic here
        std::thread([self=std::move(self), targetRegion=region, time]{
            auto& button = *self;  // 通过引用访问按钮，简化语法
            float steps = 100.0f;  // Number of steps for smooth movement
            float delay = float(time) / steps;  // Delay between each step
            
            Region startRegion = button.region;  // 保存起始位置
            
            for (int i = 0; i < steps && !button.stopRequested; ++i) {
                // 计算当前步骤应该在的位置
                float progress = float(i) / steps;
                float newX = startRegion.getx() + (targetRegion.getx() - startRegion.getx()) * progress;
                float newY = startRegion.gety() + (targetRegion.gety() - startRegion.gety()) * progress;

                {
                    std::lock_guard<std::mutex> lock(button.animMutex);
                    if (!button.stopRequested) {
                        button.region.setx(newX);
                        button.region.sety(newY);
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
            }
            
            // 如果没有被请求停止，确保最终位置精确
            if (!button.stopRequested) {
                std::lock_guard<std::mutex> lock(button.animMutex);
                button.region = targetRegion;
            }
            
            button.animationRunning = false;
        }).detach();
    }
    else
    {
        // 如果不使用流畅移动，直接设置位置
        std::lock_guard<std::mutex> lock(animMutex);
        this->region = region;
    }
}

void Button::SetFontID(int FontID)
{    this->FontID = FontID;
    if (FontID == 0) {
        font = core::Explorer::getInstance()->getFont(FontID);
    }
}