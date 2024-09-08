#pragma once
#include "Button.h"
#include "Gp.h"
class FirstScreen
{
public:
    Gp *p;
    FirstScreen(Gp *p_);
    FirstScreen();
    ~FirstScreen();
    bool firsttime = true;
    void setGp(Gp* p_);
    void paint();
    void repaint();
    void resetPoint();
    void click(int x, int y);
private:
    bool firstpaint = true;
    Button b[10];
    void paintoverlay();
    void initButtons();
    void enter();
};