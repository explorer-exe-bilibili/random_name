#pragma once

#include "base.h"
#include "../baseItem/Button.h"
#include "core/nameRandomer.h"

namespace screen {
class ListNameButton: public core::Button {
    core::Region textRegion;
public:
    ListNameButton();
    ~ListNameButton();

    void Draw();
    void SetName(const core::NameEntry& name);
};
class ListNameScreen : public Screen {
    std::vector<std::shared_ptr<ListNameButton>> m_buttons;
public:
    ListNameScreen():Screen(ScreenID::ListName) {ListNameScreen::init();};
    ~ListNameScreen()=default;

    void init() override;
    bool Click(int x,int y) override;
    void Draw() override;
    void enter(int param=0)override;
};
}
