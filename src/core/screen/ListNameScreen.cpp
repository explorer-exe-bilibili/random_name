#include "core/screen/ListNameScreen.h"

using namespace core;
using namespace screen;

void ListNameScreen::init() {
    background=Explorer::getInstance()->getBitmap(BitmapID::NameBg);
    buttons.resize(10);
    for(int i = 0; i < buttons.size(); ++i) {
        buttons[i] = std::make_shared<Button>();
    }
    
}

void ListNameScreen::Click(int x, int y) {
    // 点击处理代码
}

void ListNameScreen::Draw() {
    // 绘制代码
}

void ListNameScreen::enter() {
    // 进入代码
}