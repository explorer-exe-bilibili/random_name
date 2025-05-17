#include "core/screen/mainScreen.h"

#include "core/log.h"
#include "core/explorer.h"

using namespace screen;
using namespace core;

void MainScreen::init() {
    // 初始化按钮和背景
    auto button = std::make_shared<Button>("Click Me", 0, Region(0.1,0.2,0.2,0.3));
    button->SetClickFunc([]() {
        Log << Level::Info << "Button clicked!" << op::endl;
    });
    button->SetEnableBitmap(false);
    button->SetFontID(0);
    button->SetColor({0,0,0,255});
    buttons.push_back(button);
    
    background = core::Explorer::getInstance()->getBitmap("1");
}

bool MainScreen::Click(int x, int y) {
    // 调用基类的Click方法
    return Screen::Click(x, y);
}