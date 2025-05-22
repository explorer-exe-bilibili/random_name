#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "core/baseItem/Button.h"
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/VideoPlayer.h"


namespace screen{

class Screen {
protected:
    std::string ID;
    std::vector<std::shared_ptr<core::Button>> buttons;
    core::Bitmap* background;
    static bool useVideoBackground;
    static std::shared_ptr<core::VideoPlayer> videoBackground;
    static bool inited;
    static std::string currentScreenID;
    static std::shared_ptr<Screen> currentScreen;
    static std::map<std::string, std::shared_ptr<Screen>> screens;
public:
    Screen(const std::string& ID) : ID(ID) {}
    virtual ~Screen(){background=nullptr;};
    virtual void init()=0;
    void initBackground();
    virtual bool Click(int x, int y);

    virtual void Draw();
    virtual void enter()=0;
    virtual void enter2(){};    virtual void exit(){};

    std::string getID() const { return ID; }
    static std::shared_ptr<Screen> getCurrentScreen() { return currentScreen; }

    // 屏幕管理系统功能
    static void RegisterScreen(const std::string& id, std::shared_ptr<Screen> screen);
    static bool SwitchToScreen(const std::string& id);
private:
};

}