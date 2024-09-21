#pragma once
#include "Button.h"
#include "Gp.h"
class FirstScreen
{
    Gp* p;
    Button b[10];
    bool firstpaint = true;
    std::vector<Button> overlay;
public:
    FirstScreen(Gp *p_);
    FirstScreen();
    ~FirstScreen();
    void setGp(Gp* p_);
    void paint();
    void repaint();
    void resetPoint();
    void click(int x, int y);
private:
    void changeMode(int Mode);
    void initButtons();
    void enter();
};