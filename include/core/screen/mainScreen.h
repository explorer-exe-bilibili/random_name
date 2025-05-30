#pragma once
#include "base.h"

namespace screen
{
class MainScreen : public Screen {
    int mode=1;
    std::vector<std::shared_ptr<core::Button>> overlays;
    
    public:
        MainScreen() : Screen(ScreenID::MainMenu) { Screen::init(); init(); }
        ~MainScreen();
        void init() override;
        bool Click(int x, int y) override;
        void Draw() override;
        void enter(int) override;
    private:
        void changeMode(); 
    };
} // namespace screen
