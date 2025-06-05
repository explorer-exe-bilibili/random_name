#pragma once

#include "base.h"

namespace screen {
class ListNameScreen : public Screen {
public:
    ListNameScreen():Screen(ScreenID::ListName) {ListNameScreen::init();};
    ~ListNameScreen();

    void init() override;
    void Click(int x,int y) override;
    void Draw() override;
    void enter() override;
};
}
