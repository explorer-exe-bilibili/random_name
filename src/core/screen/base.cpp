#include "core/screen/base.h"

using namespace core;

void Screen::Draw() {
    for (const auto& button : buttons) {
        button->Draw();
    }
}