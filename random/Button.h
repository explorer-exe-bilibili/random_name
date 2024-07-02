#pragma once
#include <functional>
#include <vector>

#include "Gp.h"
#include "bitmaps.h"

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
	int x=0, y=0, xE=0, yE=0;
	int* xP=&x, * yP=&y, * xEP=&xE, * yEP=&yE;
	int xAdd=0, yAdd = 0, xEAdd = 0, yEAdd = 0;
	int TextW=-1,TextH=-1;
	double x2WW=0,y2WH=0,xE2WW=0,yE2WH=0;
	int R= 255, G = 255, B = 255;
	int BmapC = BUTTON;
	int FuncCounts=0;
	bool DisableBmap=0,DisableStr=0;
	std::wstring text;
	std::vector<std::function<void()>> functions;
public:
	Button(int x, int y, int xE, int yE,int BitmapC=BUTTON,std::wstring text=L"");
	Button();
	~Button();



	void click(int condition = CLICK, int x = -1, int y = -1);
	void paint() const;


	void setText(const std::wstring& NewText,int DisableBmap=-1);
	void setTextColor(int R, int G, int B,int DisableBmap=-1);
	void setPoint(int x, int y, int xE, int yE);
	void setxy2WWWH(double x2WW, double y2WH, double xE2WW=-1, double yE2WH=-1);
	void setBmapC(int Bmapc,int DisableStr=-1);
	void setFont(HFONT* font,int DisableBmap=-1);
	void setDisableStr(bool newValue);
	void setDisableBmap(bool newValue);
	void setGp(Gp *p);

	void refreash();
	void changeStr(std::wstring NewStr);
	void changeBmap(int NewBmapC);

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



	Breturn Binded();
	int getx() const;
	int gety() const;
	int getxE() const;
	int getyE() const;
};

