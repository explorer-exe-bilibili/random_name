#pragma once 
#include "base.h"
#include "core/baseItem/Base.h"
#include "core/log.h"
#include <chrono>

namespace screen
{
namespace settingScreen
{
struct FileTypeFilter {
    std::string description;  // 描述，如"图片文件"
    std::vector<const char*> pattern;      // 模式，如"*.png"
};
enum class SettingButtonType{
    Switch,
    Textbox,
    ColorSelect,
    PathSelect,
    FileSelect,
    RegionEditor,     // 新增：区域编辑器
    RegionPreset      // 新增：区域预设管理
};
enum SettingButtonAction{
    None=0,
    Restart=1<<0,
    ReloadConfig=1<<1,
    ReloadVideo=1<<2,
    ReloadPhoto=1<<3,
    ResetWindowTitle=1<<4,
    CountBetween=1<<5
};
enum class FileType{
    All=0,
    Video,
    Picture,
    NameFile,
    Sound,
    Audio,
    Font,
    Unknown
};

// Region编辑器拖拽模式
enum class DragMode {
    None,
    Move,           // 移动整个区域
    ResizeLeft,     // 调整左边界
    ResizeRight,    // 调整右边界
    ResizeTop,      // 调整上边界
    ResizeBottom,   // 调整下边界
    ResizeTopLeft,  // 调整左上角
    ResizeTopRight, // 调整右上角
    ResizeBottomLeft,  // 调整左下角
    ResizeBottomRight  // 调整右下角
};
class sItem{
public:
    // 默认构造函数，确保字符串成员正确初始化
    sItem() : fileType(FileType::All), 
              action(SettingButtonAction::None), 
              type(SettingButtonType::Switch),
              bitmapID(core::BitmapID::Unknown),
              audioID(core::AudioID::Unknown),
              fontID(core::FontID::Unknown),
              videoID(core::VideoID::Unknown),
              color(core::Color(0,0,0,255)),
              name(""),
              configName(""),
              fileChooseWindowName(""),
              bitmapName(""),
              outOfLimitOutPut(""),
              minCount(0),
              maxCount(0),
              regionCategory(core::RegionName::SMALL_WINDOW),
              relatedRegions() {}
    
    // 拷贝构造函数，确保字符串正确复制
    sItem(const sItem& other) : fileType(other.fileType),
                                action(other.action),
                                type(other.type),
                                bitmapID(other.bitmapID),
                                fontID(other.fontID),
                                videoID(other.videoID),
                                color(other.color),
                                name(other.name),
                                configName(other.configName),
                                fileChooseWindowName(other.fileChooseWindowName),
                                bitmapName(other.bitmapName),
                                audioID(other.audioID),
                                outOfLimitOutPut(other.outOfLimitOutPut),
                                minCount(other.minCount),
                                maxCount(other.maxCount),
                                regionCategory(other.regionCategory),
                                relatedRegions(other.relatedRegions) {}
    
    // 赋值操作符
    sItem& operator=(const sItem& other) {
        if (this != &other) {
            fileType = other.fileType;
            action = other.action;
            type = other.type;
            bitmapID = other.bitmapID;
            fontID = other.fontID;
            videoID = other.videoID;
            color = other.color;
            name = other.name;
            configName = other.configName;
            fileChooseWindowName = other.fileChooseWindowName;
            bitmapName = other.bitmapName;
            audioID = other.audioID;
            outOfLimitOutPut = other.outOfLimitOutPut;
            minCount = other.minCount;
            maxCount = other.maxCount;
            regionCategory = other.regionCategory;
            relatedRegions = other.relatedRegions;
        }
        return *this;
    }
    
    FileType fileType=FileType::All;
    SettingButtonAction action=SettingButtonAction::None;
    SettingButtonType type=SettingButtonType::Switch;
    core::BitmapID bitmapID=core::BitmapID::Unknown;
    core::FontID fontID=core::FontID::Unknown;
    core::VideoID videoID=core::VideoID::Unknown;
    core::Color color=core::Color(0,0,0,255);
    std::string name;
    std::string configName;
    std::string fileChooseWindowName;
    std::string bitmapName;
    core::AudioID audioID;
    std::string outOfLimitOutPut;
    int minCount=0;
    int maxCount=0;
    
    // Region编辑器相关属性
    core::RegionName regionCategory = core::RegionName::SMALL_WINDOW; // Region类别
    std::vector<std::string> relatedRegions;     // 相关的Region列表
};
class SettingButton
{
    sItem item;
    std::string showText;
    core::Region TextRegion;
    core::Region ButtonRegion;
    std::shared_ptr<core::Button> button=nullptr;
    std::shared_ptr<core::Button> button2=nullptr;
    core::Font** font=nullptr;
    int number=0;
    int page=-1;
    
    // 内嵌文本框相关
    bool isTextboxEditing = false;
    std::string editingText;
    core::Region editingTextRegion;
    size_t cursorPosition = 0;
    std::chrono::steady_clock::time_point lastCursorBlink;
    bool showCursor = true;
    
public:
    SettingButton(sItem item,int number, int page);
    ~SettingButton()=default;

    void Draw(int currentPage, unsigned char alpha = 255)const;
    bool Click(core::Point point,int page);
    bool HandleKeyInput(char key); // 处理键盘输入
    bool HandleUnicodeInput(const std::string& utf8_char); // 处理Unicode字符输入
    bool IsEditing() const { return isTextboxEditing; } // 检查是否正在编辑
    void FinishEditing(); // 完成编辑并保存
    void CancelEditing(); // 取消编辑
    operator bool() const { return page>=0; }
private:
    core::Color selectColor();
    std::string selectFile();
    std::string selectPath();
    void openFile();
    void checkActions();
};
}
class SettingScreen : public Screen
{
    std::vector<std::shared_ptr<settingScreen::SettingButton>> s_buttons;
    std::vector<std::string> titles;
    int currentPage=0;
    int pages=-1;
      // 页面切换动画相关
    enum class TransitionType {
        Fade,       // 淡入淡出
        Slide       // 滑动
    };
    
    bool isTransitioning = false;
    int targetPage = 0;
    std::chrono::steady_clock::time_point transitionStartTime;
    float transitionDuration = 0.3f; // 动画持续时间（秒）
    float currentPageAlpha = 1.0f;   // 当前页面的透明度
    float targetPageAlpha = 0.0f;    // 目标页面的透明度
    TransitionType transitionType = TransitionType::Fade;  // 动画类型
    float slideOffset = 0.0f;        // 滑动偏移量
    
    void loadButtons();
    void changePage(bool forward);
    void updatePageTransition(); // 更新页面切换动画
    float calculateTransitionAlpha(bool isCurrentPage); // 计算页面透明度
    void reloadButtonsRegion() override;
public:
    SettingScreen() : Screen(ScreenID::Settings) {init();}
    ~SettingScreen() {}

    void init() override;
    void enter(int) override;
    void Draw() override;
    bool Click(int x, int y) override;
    bool HandleKeyInput(char key) override; // 处理键盘输入
    bool HandleUnicodeInput(const std::string& utf8_char) override; // 处理Unicode字符输入
    void FinishAllTextEditing(); // 完成所有文本编辑
      // 页面切换动画配置
    void setTransitionDuration(float duration) { transitionDuration = duration; }
    float getTransitionDuration() const { return transitionDuration; }
    bool isPageTransitioning() const { return isTransitioning; }
    void setTransitionType(TransitionType type) { transitionType = type; }
    TransitionType getTransitionType() const { return transitionType; }
};
} // namespace screen