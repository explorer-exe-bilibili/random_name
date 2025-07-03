#pragma once

#include "../render/Texture.h"
#include "Bitmap.h"
#include "Font.h"
#include <functional>
#include <mutex>
#include <atomic>
#include <memory>
#include "../explorer.h"
#include "../Config.h"

namespace core {
class Button {
public:
    Button(const std::string& text="", FontID fontid=FontID::Default, const Region& region=Region(), Bitmap** bitmapPtr=nullptr);
    Button(const Button& button);
    Button& operator=(const Button& button);
    ~Button();

    void Draw(unsigned char alpha=255);
    bool OnClick(Point point);
    void MoveTo(const Region& region, const bool enableFluent=false, const float speed=50.0f, std::function<void()> onComplete=nullptr);
    void FadeOut(float duration, unsigned char startAlpha=255, unsigned char endAlpha=0, std::function<void()> onComplete=nullptr);
    void SetClickFunc(std::function<void()> func) {this->ClickFunc = func;}


    void SetText(const std::string& text) {this->text = text;}
    void SetBitmap(const std::string& bitmapID){
        if(core::Explorer::getInstance()->isBitmapLoaded(bitmapID)) {
            this->bitmapPtr = core::Explorer::getInstance()->getBitmapPtr(bitmapID);
        }
    }
    void SetBitmap(BitmapID id) {
        this->bitmapid = id;
        if(core::Explorer::getInstance()->isBitmapLoaded(id)) {
            this->bitmapPtr = core::Explorer::getInstance()->getBitmapPtr(id);
        }
    }
    void SetRegion(const Region& region) {this->region = region;}
    void SetRegionStr(const std::string& name){this->regionConfig=name;resetRegion();}
    void SetFontID(FontID id);
    void SetAudioID(AudioID id) {this->audioid = id;}
    void SetTextCenterd(bool isCentered){this->isCentered = isCentered;}
    void SetColor(const Color& color) {this->color = color;}
    void SetFillColor(const Color& color) {this->fillColor = color;}
    void SetFontScale(float scale) { this->fontScale = scale; }

    void SetEnableText(bool enable) {this->enableText = enable;}
    void SetEnable(bool enable) {this->enable = enable;}
    void SetEnableBitmap(bool enable) {this->enableBitmap = enable;}
    void SetEnableFill(bool enable) {this->enableFill = enable;}

    void resetRegion() {if(!regionConfig.empty())this->region=Config::getInstance()->getRegion(regionConfig);}
    Region getRegion() const { return region; }
protected:
    Region region;
    Color color=Color(255, 255, 255, 255);
    Color fillColor=Color(0, 0, 0, 255);
    bool enableText=true;
    bool enable=true;
    bool enableBitmap=true;
    bool enableFill=false;
    bool isCentered=true; // 是否居中显示文本
    std::string text="";
    std::string regionConfig="";
    Bitmap** bitmapPtr = nullptr; // 用于自动更新的指针
    std::function<void()> ClickFunc;
    BitmapID bitmapid=BitmapID::Unknown;
    FontID fontid=FontID::Default;
    AudioID audioid=AudioID::click;
    float fontSize=0;
    float fontScale=1.0f;
    Font** fontPtr = nullptr; // 用于自动更新的字体指针
    // 线程安全相关成员
    std::timed_mutex animMutex;                   // 线程同步互斥锁（支持超时）
    std::atomic<bool> animationRunning{false}; // 标记动画是否在运行
    std::atomic<bool> stopRequested{false}; // 请求停止标志
    
    // 淡出动画相关成员
    std::atomic<bool> fadeAnimationRunning{false}; // 标记淡出动画是否在运行
    std::atomic<bool> fadeStopRequested{false}; // 请求停止淡出动画标志
    std::atomic<unsigned char> currentFadeAlpha{255}; // 当前淡出透明度值
    mutable std::timed_mutex fadeMutex;           // 淡出动画线程同步互斥锁（支持超时）
};
}