#pragma once

#include <string>
#include <vector>
#include <memory>

#include "core/baseItem/Button.h"

namespace core{

class Screen {
    std::string ID;
    std::vector<std::shared_ptr<core::Button>> buttons;
public:
    Screen(const std::string& ID) : ID(ID) { init(); }
    virtual ~Screen()=default;
    virtual void init()=0;
    virtual bool Click(int x, int y)=0;

    virtual void Draw();
    
    std::string getID() const { return ID; }
    
};

}