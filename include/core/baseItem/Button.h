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
#include <vector>

// 编辑模式相关枚举
enum class EditMode {
    None,           // 不在编辑模式
    Move,           // 移动模式
    ResizeTopLeft,  // 调整左上角
    ResizeTopRight, // 调整右上角
    ResizeBottomLeft,  // 调整左下角
    ResizeBottomRight, // 调整右下角
    ResizeLeft,     // 调整左边
    ResizeRight,    // 调整右边
    ResizeTop,      // 调整上边
    ResizeBottom    // 调整下边
};

// 编辑手柄结构
struct EditHandle {
    core::Region region;
    EditMode mode;
    bool visible;
    EditHandle() : region(), mode(EditMode::None), visible(false) {}
    EditHandle(const core::Region& r, EditMode m) : region(r), mode(m), visible(true) {}
};

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
    void SetRegion(const Region& region) {this->region = region; UpdateEditHandles();}
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

    // 编辑模式相关方法
    void SetEditMode(bool enable);
    bool IsEditModeEnabled() const {return editModeEnabled;}
    void SetEditHandleSize(float size) {editHandleSize = size;UpdateEditHandles();}
    void SetEditHandleColor(const Color& color) {editHandleColor = color;}
    void SetEditBorderColor(const Color& color) {editBorderColor = color;}
    void SetEditBorderWidth(float width) {editBorderWidth = width;}
    void SetOnEditComplete(std::function<void(const Region&)> callback);
    bool OnEditMouseDown(Point point);
    bool OnEditMouseMove(Point point);
    bool OnEditMouseUp(Point point);
    void DrawEditOverlay();
    EditMode GetEditModeAt(Point point) const;

    void resetRegion() {if(!regionConfig.empty())this->region=Config::getInstance()->getRegion(regionConfig); UpdateEditHandles();}
    Region GetRegion() const { return region; }
    std::string GetText() const { return text; }
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

    // 编辑模式相关成员
    bool editModeEnabled = false;
    EditMode currentEditMode = EditMode::None;
    bool isDragging = false;
    Point dragStartPoint;
    Region originalRegion;
    std::vector<EditHandle> editHandles;
    float editHandleSize = 8.0f;
    Color editHandleColor = Color(0, 120, 215, 255);
    Color editBorderColor = Color(0, 120, 215, 255);
    float editBorderWidth = 2.0f;
    std::function<void(const Region&)> onEditComplete;
    float minWidth = 10.0f;
    float minHeight = 10.0f;
    // 编辑模式辅助方法
    void UpdateEditHandles();
    void UpdateRegionFromEdit(Point currentPoint);
    bool IsPointInHandle(Point point, const EditHandle& handle) const;
    void ClampRegion();
};
}