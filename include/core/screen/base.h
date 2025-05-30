#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "core/baseItem/Button.h"
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Video/VideoPlayer.h"


namespace screen{
enum class ScreenID {
    MainMenu,
    Settings,
    Name,
    Video
};

class Screen {
protected:
    std::vector<std::shared_ptr<core::Button>> buttons;
    core::Bitmap* background;
    ScreenID ID;
    static bool useVideoBackground;
    static core::VideoPlayer* videoBackground;
    static bool inited;
    static ScreenID currentScreenID;
    static std::shared_ptr<Screen> currentScreen;
    static std::map<ScreenID, std::shared_ptr<Screen>> screens;
public:
    Screen(ScreenID ID) : ID(ID) {}
    virtual ~Screen(){background=nullptr;};
    virtual void init();
    virtual bool Click(int x, int y);

    virtual void Draw();
    virtual void enter(int param)=0;
    virtual void exit(){};

    ScreenID getID() const { return ID; }
    static std::shared_ptr<Screen> getCurrentScreen() { return currentScreen; }

    // 屏幕管理系统功能
    static void RegisterScreen(ScreenID id, std::shared_ptr<Screen> screen);
    static bool SwitchToScreen(ScreenID id);
private:
};

}