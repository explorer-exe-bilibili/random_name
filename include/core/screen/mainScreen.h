#pragma once
#include "base.h"

namespace screen
{
class MainScreen : public Screen {
    int mode=1;
    std::vector<std::shared_ptr<core::Button>> overlays;
    
    public:
        MainScreen() : Screen("Main") { initBackground(); init(); }
        ~MainScreen();
        void init() override;
        bool Click(int x, int y) override;
        void Draw() override;
    private:
        void changeMode(); 
    };
} // namespace screen
