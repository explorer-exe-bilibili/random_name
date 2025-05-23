#include "core/screen/SettingScreen.h"
#include "core/log.h"

using namespace screen;
using namespace core;

void SettingScreen::init() {
    background=core::Explorer::getInstance()->getBitmap(BitmapID::Background);
    buttons.clear();
}

void SettingScreen::Draw() {
    if(background)
        background->Draw({0, 0, 1, 1});
    for(const auto& button : buttons) {
        if(button)
            button.Draw(currentPage);
    }
    if(currentPage<titles.size()){
        core::Font* font=core::Explorer::getInstance()->getFont(FontID::Normal);
        font->RenderTextBetween(titles[currentPage], {0,0.1,1,0.2}, 0.7f, Color(255, 255, 255,255));
    }
}

void SettingScreen::enter(int) {
    Log << Level::Info << "进入设置界面" << op::endl;
}

bool SettingScreen::Click(int x, int y) {
    if(Screen::Click(x, y))return true;
    for(auto& button : buttons) if(button)if(button.Click(Point(x, y,false)))return true;
    return false;
}