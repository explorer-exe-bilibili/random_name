#pragma once 
#include "base.h"
#include "core/baseItem/Base.h"

namespace screen
{
struct FileTypeFilter {
    std::string description;  // 描述，如"图片文件"
    std::string pattern;      // 模式，如"*.png"
};
enum class SettingButtonType{
    Switch,
    Textbox,
    ColorSelect,
    PathSelect,
    FileSelect
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
    Audio,
    Font,
    Unknown
};
class sItem{
public:
    // 默认构造函数，确保字符串成员正确初始化
    sItem() : fileType(FileType::All), 
              action(SettingButtonAction::None), 
              type(SettingButtonType::Switch),
              bitmapID(core::BitmapID::Unknown),
              fontID(core::FontID::Unknown),
              videoID(core::VideoID::Unknown),
              color(core::Color(0,0,0,255)),
              name(""),
              configName(""),
              fileChooseWindowName(""),
              bitmapName(""),
              audioID(""),
              outOfLimitOutPut(""),
              minCount(0),
              maxCount(0) {}
    
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
                                maxCount(other.maxCount) {}
    
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
    std::string audioID;
    std::string outOfLimitOutPut;
    int minCount=0;
    int maxCount=0;
};
class SettingButton
{
    sItem item;
    core::Region TextRegion;
    core::Region ButtonRegion;
    std::shared_ptr<core::Button> button=nullptr;
    std::shared_ptr<core::Button> button2=nullptr;
    core::Font* font=nullptr;
    int number=0;
    int page=-1;
public:
    SettingButton(sItem item,int number, int page);
    ~SettingButton() {Log << core::Level::Info << "SettingButton析构" << op::endl;}

    void Draw(int currentPage, unsigned char alpha = 255)const;
    bool Click(core::Point point);
    operator bool() const { return page>=0; }
private:
    core::Color selectColor();
    std::string selectFile();
    std::string selectPath();
    void openFile();
    void checkActions();
    void updateConfig();
};
class SettingScreen : public Screen
{
    std::vector<SettingButton> s_buttons;
    std::vector<std::string> titles;
    int currentPage=0;
    int pages=-1;
    void loadButtons();
    void changePage(bool forward);
public:
    SettingScreen() : Screen(ScreenID::Settings) {init();}
    ~SettingScreen() {}

    void init() override;
    void enter(int) override;
    void Draw() override;
    bool Click(int x, int y) override;
};
} // namespace screen