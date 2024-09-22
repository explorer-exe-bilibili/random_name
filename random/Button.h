#pragma once
#include <functional>
#include <vector>

#include "Gp.h"
#include "bitmaps.h"
#include "Timer.h"

#define CLICK 0
#define DCLICK 1


class Button
{
public:
	struct Breturn
	{
		int xAdd=0, yAdd = 0, xEAdd = 0, yEAdd = 0;
		Button *binded;
	};
private:
	Button *binded=nullptr;
	HFONT* font=nullptr;
	Gp *p=nullptr;
	double x=0, y=0, xE=0, yE=0;
	double* xP, * yP, * xEP, * yEP;
	int xAdd=0, yAdd = 0, xEAdd = 0, yEAdd = 0;
	int TextW=-1,TextH=-1;
	double x2WW=0,y2WH=0,xE2WW=0,yE2WH=0;
	int R= 255, G = 255, B = 255;
	int BmapC = BUTTON;
	int FuncCounts=0;
	bool DisableBmap = 0, DisableStr = 0, Disable = 0, Moving = 0, IsVertical = 0;
	std::wstring text;
	std::vector<std::function<void()>> functions;
	std::string music_string;
	Timer move_timer;
public:
	static bool needFresh;

	Button(int x, int y, int xE, int yE,int BitmapC=BUTTON,std::wstring text=L"");
	Button();
	~Button();



	void click(int condition = CLICK, int x = -1, int y = -1) const;
	void paint() const;


	void setText(const std::wstring& NewText,int DisableBmap=-1);
	void setTextColor(int R, int G, int B,int DisableBmap=-1);
	void setPoint(int x, int y, int xE, int yE);
	void setxy2WWWH(double x2WW, double y2WH, double xE2WW=-1, double yE2WH=-1);
	void setBmapC(int Bmapc,int DisableStr=-1);
	void setFont(HFONT* font,int DisableBmap=-1);
	void setDisableStr(bool newValue);
	void setDisableBmap(bool newValue);
	void setMusic(std::string music_string);
	void setGp(Gp *p);
	void setDisable(bool newValue);
	void setVertical(bool newValue);

	void MoveTo(int x, int y, int xend = -1, int yend = -1, bool smoothly = 1, double xVelocity = 1, double yXelocity = 1);

	void refresh();
	void changeStr(std::wstring NewStr);
	void changeBmap(int NewBmapC);
	void reConnect();
	void disConnect();

	operator bool() const;
	bool operator==(const Button& b) const;


	int bind(std::function<void()> func,int condition = CLICK);



	bool bind(Button* b);
	int bindX(Button* b,int add=0);
	int bindXE(Button* b,int add=0);
	int bindY(Button* b,int add=0);
	int bindYE(Button* b,int add=0);
	int bindXXE(Button *b,int add=0);
	int bindYYE(Button *b,int add=0);



	bool IsDisabled() const;
	bool IsMoving()const;
	Breturn Binded();
	int getx() const;
	int gety() const;
	int getxE() const;
	int getyE() const;
};

