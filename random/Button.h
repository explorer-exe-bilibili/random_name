#pragma once
#include <functional>
#include <vector>
#include <string>
#include <Windows.h>

#include "bitmaps.h"
#include "Timer.h"

enum
{
	CLICK = 0,
	DoubleCLICK = 1
};


class Gp;


enum ButtonClickState
{
	Disabled = 0,
	outOfRange,
	Clicked
};

class Button
{
public:
	struct Breturn
	{
		int xAdd=0, yAdd = 0, xEAdd = 0, yEAdd = 0;
		Button *binded;
	};
protected:
	Button *binded=nullptr;
	HFONT* font=nullptr;
	Gp *p=nullptr;
	double x=0, y=0, xE=0, yE=0;
	double* xP, * yP, * xEP, * yEP;
	int xAdd=0, yAdd = 0, xEAdd = 0, yEAdd = 0;
	int TextW=-1,TextH=-1;
	double x2WW=0,y2WH=0,xE2WW=0,yE2WH=0;
	unsigned char R= 255, G = 255, B = 255;
	int BmapC = BUTTON;
	int FuncCounts=0;
	bool DisableBmap = false, DisableStr = false, Disable = false, Moving = false, IsVertical = false;
	std::wstring text;
	std::vector<std::function<void()>> functions;
	std::string music_string;
	Timer move_timer;
public:
	static bool needFresh;

	Button(int x, int y, int xE, int yE,int BitmapC=BUTTON, const std::wstring& text=L"");
	Button();
	~Button();


	int click(int condition = CLICK, int x = -1, int y = -1) const;
	void paint() const;


	void setText(const std::wstring& NewText,int DisableBmap=-1);
	void setTextColor(int R, int G, int B,int DisableBmap=-1);
	void setPoint(int x, int y, int xE, int yE);
	void setxy2WWWH(double x2WW, double y2WH, double xE2WW=-1, double yE2WH=-1);
	void setBmapC(int Bmapc,int DisableStr=-1);
	void setFont(HFONT* font,int DisableBmap=-1);
	void setDisableStr(bool newValue);
	void setDisableBmap(bool newValue);
	void setMusic(const std::string& music_string);
	void setGp(Gp *p);
	void setDisable(bool newValue);
	void setVertical(bool newValue);

	void MoveTo(int x, int y, int xend = -1, int yend = -1, bool smoothly = true
	            , double xVelocity = 1, double yVelocity = 1, const std::function<void()>& Callback=nullptr);

	void refresh();
	void changeStr(const std::wstring& NewStr);
	void changeBmap(int NewBmapC);
	void reConnect();
	void disConnect();

	Button& operator=(const Button& b);
	explicit operator bool() const;
	bool operator==(const Button& b) const;


	int bind(const std::function<void()>& func,int condition = CLICK);



	bool bind(Button* b);
	int bindX(const Button* b,int add=0);
	int bindXE(const Button* b,int add=0);
	int bindY(const Button* b,int add=0);
	int bindYE(const Button* b,int add=0);
	int bindXXE(const Button *b,int add=0);
	int bindYYE(const Button *b,int add=0);



	[[nodiscard]] bool IsDisabled() const;
	[[nodiscard]] bool IsMoving()const;
	[[nodiscard]] Breturn Binded() const;
	[[nodiscard]] int getx() const;
	[[nodiscard]] int gety() const;
	[[nodiscard]] int getxE() const;
	[[nodiscard]] int getyE() const;
};

