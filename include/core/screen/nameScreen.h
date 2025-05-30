#pragma once

#include "base.h"
#include "../nameRandomer.h"

namespace screen
{
class NameScreen : public Screen {
    std::shared_ptr<core::Font> StarFont;
    std::shared_ptr<core::Font> NameFont;
    unsigned char alpha=0;
public:
    NameScreen():Screen(ScreenID::Name){init();}
    void Draw() override;
    void init() override;
    void enter(int mode) override;
};
}