#pragma once
#include "base.h"

namespace screen
{
class MainScreen : public Screen {
    public:
        MainScreen() : Screen("Main") { init(); }
        void init() override;
        bool Click(int x, int y) override;
    };
} // namespace screen
