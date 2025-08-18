#pragma once

#include "base.h"
#include "../baseItem/Button.h"
#include "core/nameRandomer.h"
#include <vector>

namespace screen {
class ListNameButton: public core::Button {
    core::Region textRegion;
    bool isEnglish;
    std::vector<std::string> words;
    std::vector<core::SubRegion> wordRegions;
public:
    ListNameButton();
    ~ListNameButton();

    void Draw();
    void SetName(const core::NameEntry& name);
};
class ListNameScreen : public Screen {
    std::vector<std::shared_ptr<ListNameButton>> m_buttons;
    core::Region baseRegion;
    void reloadButtonsRegion()override;
public:
    ListNameScreen():Screen(ScreenID::ListName) {ListNameScreen::init();};
    ~ListNameScreen()=default;

    void init() override;
    bool Click(int x,int y) override;
    void Draw() override;
    void enter(int param=0)override;
};
}
