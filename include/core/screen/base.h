#pragma once

#include <string>
#include <vector>
#include <memory>

#include "core/baseItem/Button.h"
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/VideoPlayer.h"


namespace screen{

class Screen {
protected:
    std::string ID;
    std::vector<std::shared_ptr<core::Button>> buttons;
    core::Bitmap* background;
    static bool useVideoBackground;
    static std::shared_ptr<core::VideoPlayer> videoBackground;
    static bool inited;
public:
    Screen(const std::string& ID) : ID(ID) {}
    virtual ~Screen()=default;
    virtual void init()=0;
    void initBackground();
    virtual bool Click(int x, int y);

    virtual void Draw();
    
    std::string getID() const { return ID; }
private:
};

}