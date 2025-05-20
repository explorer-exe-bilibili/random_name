#pragma once

#include "base.h"

namespace screen
{
class NameScreen : public Screen {
    std::shared_ptr<core::Font> StarFont;
    std::shared_ptr<core::Font> NameFont;
public:
    NameScreen(std::string name):Screen(name){init();}
    void Draw() override;
    void init() override;
};
}