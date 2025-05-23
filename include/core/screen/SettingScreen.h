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
    PathSelect
};
enum SettingButtonAction{
    None=0,
    Restart=1<<0,
    ReloadConfig=1<<1,
    ReloadVideo=1<<2,
    ReloadPhoto=1<<3,
    ResetWindowTitle=1<<4,
    IsBitmap=1<<5,
    IsVideo=1<<6,
    IsNameFile=1<<7,
    IsFile=1<<8,
    IsDir=1<<9
};
class SettingButton
{
    std::string name;
    SettingButtonType type;
    core::Region TextRegion;
    core::Region ButtonRegion;
    std::string ConfigName;
    std::shared_ptr<core::Button> button=nullptr;
    std::shared_ptr<core::Button> button2=nullptr;
    core::Font* font=nullptr;
    int page=-1;
    int actions=0;
public:
    SettingButton(const std::string& name, SettingButtonType type, int number, int page);
    ~SettingButton() {}

    void Draw(int currentPage, unsigned char alpha = 255)const;
    bool Click(core::Point point);
    operator bool() const { return page>=0; }
private:
    core::Color selectColor();
    std::string selectPath(const std::vector<FileTypeFilter>& filter={{"所有文件","*.*"}});
    void openFile();
    void checkActions();
};
class SettingScreen : public Screen
{
    std::vector<SettingButton> buttons;
    std::vector<std::string> titles;
    int currentPage=0;
public:
    SettingScreen() : Screen("Settings") {init();}
    ~SettingScreen() {}

    void init() override;
    void enter(int) override;
    void Draw() override;
    bool Click(int x, int y) override;
};
} // namespace screen
