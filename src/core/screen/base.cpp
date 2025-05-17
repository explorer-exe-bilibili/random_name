#include "core/screen/base.h"

using namespace screen;
using namespace core;

void Screen::Draw() {
    if(background)
        background->Draw(Region(0, 0, WindowInfo.width, WindowInfo.height));
    for (const auto& button : buttons) {
        if(button)
            button->Draw();
    }
}

bool Screen::Click(int x, int y) {
    bool return_value = false;
    for (const auto& button : buttons) {
            if(button->OnClick(Point(x, y)))
            {
                return_value = true;
            }
    }
    return return_value;
}