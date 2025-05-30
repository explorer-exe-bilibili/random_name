#include "core/screen/base.h"
#include "core/baseItem/Video/VideoPlayer.h"
#include "core/Config.h"
#include "core/log.h"

using namespace screen;
using namespace core;

bool Screen::inited = false;
std::shared_ptr<Screen> Screen::currentScreen = nullptr;
ScreenID Screen::currentScreenID = ScreenID::MainMenu;
std::map<ScreenID, std::shared_ptr<Screen>> Screen::screens;
bool Screen::useVideoBackground = false;
core::VideoPlayer* Screen::videoBackground = nullptr;

void Screen::Draw() {
    Bitmap* currentFrame=nullptr;
    if(useVideoBackground){
        currentFrame=videoBackground->getCurrentFrame().get();
    }
    else if(background)currentFrame=background;
    if(currentFrame){
        currentFrame->CreateTextureFromBuffer();
        currentFrame->Draw({0,0,1,1},1);
    }
    for (const auto& button : buttons) {
        if(button)
            button->Draw();
    }
}

void Screen::init() {
    if (inited) return;
    inited = true;
    useVideoBackground = Config::getInstance()->getBool(USE_VIDEO_BACKGROUND);
    if (useVideoBackground) {
        videoBackground = Explorer::getInstance()->getVideo(VideoID::Background);
    }
}

bool Screen::Click(int x, int y) {
    bool return_value = false;
    for (const auto& button : buttons) {
        if(button)
            if(button->OnClick(Point(x, y, false)))
            {
                return_value = true;
            }
    }
    return return_value;
}

void Screen::RegisterScreen(ScreenID id, std::shared_ptr<Screen> screen) {
    if (!screens.contains(id)) {
        screens[id] = screen;
    } else {
        Log << Level::Error << "Screen with ID " << static_cast<int>(id) << " already exists." << op::endl;
    }
}

bool Screen::SwitchToScreen(ScreenID id) {
    currentScreenID = id;
    if (screens.contains(id)) {
        currentScreen = screens[id];
        currentScreen->enter(0);
        return true;
    }
    return false;
}