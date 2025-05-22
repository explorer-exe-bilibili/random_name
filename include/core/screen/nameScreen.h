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
    NameScreen(std::string name):Screen(name){init();}
    void Draw() override;
    void init() override;
    void enter() override;
    void enter2() override;
};
}