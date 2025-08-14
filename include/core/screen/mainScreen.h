#pragma once
#include "base.h"

namespace screen
{
class MainScreen : public Screen {
    int mode=1;
    std::vector<std::shared_ptr<core::Button>> overlays;

    void reloadButtonsRegion() override;
    void changeLanguage_()override;

    public:
        MainScreen() : Screen(ScreenID::MainMenu) { Screen::init(); init(); }
        ~MainScreen();
        void init() override;
        bool Click(int x, int y) override;
        void Draw() override;
        void enter(int) override;
        void setMode(int mode) { this->mode = mode; changeMode(); }
    private:
        void changeMode(); 
};
} // namespace screen
