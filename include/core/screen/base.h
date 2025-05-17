#pragma once

#include <string>
#include <vector>
#include <memory>

#include "core/baseItem/Button.h"

namespace screen{

class Screen {
protected:
    std::string ID;
    std::vector<std::shared_ptr<core::Button>> buttons;
    std::shared_ptr<core::Bitmap> background;
public:
    Screen(const std::string& ID) : ID(ID) {}
    virtual ~Screen()=default;
    virtual void init()=0;
    virtual bool Click(int x, int y);

    virtual void Draw();
    
    std::string getID() const { return ID; }
    
};

}