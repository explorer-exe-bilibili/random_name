#pragma once
#include "Button.h"
#include "Screen.h"

class FirstScreen:Screen
{
    std::vector<Button> overlay;
public:
    FirstScreen();
    ~FirstScreen() override;
    void setGp(Gp* p);
    void paint();
    void resetSize();
    void click(int x, int y);
    static void enter();
private:
    void changeMode(int Mode);
    void resetPoint();
    void regButtons() override;
};
