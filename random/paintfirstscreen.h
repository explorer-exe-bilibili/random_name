#pragma once
#include<Windows.h>
#include"Gp.h"
class paintfirstscreen
{
public:
	static bool firsttime;
	static void paintoverlay(Gp *p);
	static void printfirstmenu(Gp *p);
	static void repaint();
private:
	static bool firstpaint;
};

