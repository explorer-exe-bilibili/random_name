#pragma once

#include "../render/Texture.h"
#include "Bitmap.h"
#include "Font.h"
#include <functional>
#include <mutex>
#include <atomic>
#include <memory>

namespace core {
class Button {
public:
    Button(const std::string& text="", int FontID=0, const Region& region=Region(), const std::shared_ptr<core::Bitmap>& bitmap=std::make_shared<core::Bitmap>());
    Button(const Button& button);
    Button& operator=(const Button& button);
    ~Button();

    void Draw();
    bool OnClick(Point point);
    void MoveTo(const Region& region, const bool enableFluent=false ,const int time=0);
    void SetClickFunc(std::function<void()> func) {this->ClickFunc = func;}


    void SetText(const std::string& text) {this->text = text;}
    void SetBitmap(const std::shared_ptr<core::Bitmap>& bitmap) {this->bitmap = bitmap;}
    void SetRegion(const Region& region) {this->region = region;}
    void SetFontID(int FontID);
    void SetFont(const std::shared_ptr<core::Font>& font) {this->font = font;}
    void SetColor(const Color& color) {this->color = color;}

    void SetEnableText(bool enable) {this->enableText = enable;}
    void SetEnable(bool enable) {this->enable = enable;}
    void SetEnableBitmap(bool enable) {this->enableBitmap = enable;}
private:
    Region region;
    Color color=Color(255, 255, 255, 255);
    bool enableText=true;
    bool enable=true;
    bool enableBitmap=true;
    std::string text="";
    std::shared_ptr<core::Bitmap> bitmap;
    std::function<void()> ClickFunc;
    int FontID=0;
    float fontSize=0;
    std::shared_ptr<core::Font> font;
    // 线程安全相关成员
    std::mutex animMutex;                   // 线程同步互斥锁
    std::atomic<bool> animationRunning{false}; // 标记动画是否在运行
    std::atomic<bool> stopRequested{false}; // 请求停止标志
};
}