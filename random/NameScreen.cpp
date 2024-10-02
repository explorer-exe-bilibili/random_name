#include "NameScreen.h"
#include "set-json.h"
#include "MenuScreen.h"
#include "FirstScreen.h"
#include "getname.h"
#include "NameButton.h"
#include "sth2sth.h"
#include"bitmaps.h"
#include"configitem.h"
#include "Gp.h"
#include"mywindows.h"
#include"ui.h"

Log RandomList("name.txt", true);

enum ButtonName
{
	SKIP = 0,
	addName = 1
};

HBITMAP CreateTransparentTextBitmap(const std::wstring& text, float width, float height, HFONT hFont);

getname* name = nullptr;

NameScreen::NameScreen(Gp* p)
{
	setGp(p);
	NameScreen::regButtons();
	name = getname::getInstance();
}
NameScreen::NameScreen()
{
	p = nullptr;
	NameScreen::regButtons();
	name = getname::getInstance();
}
NameScreen::~NameScreen()
{
	p = nullptr;
	number = 0;
	step = 0;
	is5star = false;
	is4star = false;
	icon_star = nullptr;
}
void NameScreen::regButtons()
{
	Button tmp;
	tmp.setxy2WWWH(0.82, 0.045, 0.88, 0.08);
	tmp.setText(L"跳过>>");
	tmp.refresh();
	tmp.setFont(&ui::text_mid);
	tmp.bind([this] {menu(); });
	tmp.setTextColor(255, 255, 255, 1);
	buttons.push_back(tmp);
	tmp.setxy2WWWH(0.4, 0.8, 0.52, 0.836);
	tmp.setText(L"增加此名字几率");
	tmp.refresh();
	tmp.setFont(&ui::text_mid);
	tmp.bind([this] {AddName(); });
	tmp.setTextColor(255, 230, 34, 1);
	buttons.push_back(tmp);
	nameButton = std::make_shared<NameButton>();
	nameButton->setPoint(0, 0, mywindows::WW, mywindows::WH);
	nameButton->setGp(p);
	for(auto & i:buttons)
	{
		i.reConnect();
		i.setGp(p);
	}
}

void NameScreen::enter()
{
	ui::ScreenMode = namescreen;
	is4star=false;
	is5star=false;
	step=0;
	if (number >= 245)
	{
		for (char i = 0; i < config::getint(POOL_COUNT); i++)
			name->ReRandom(i);
		number = 0;
	}
	for (const int tmp1 = number + 10; number < tmp1; number++) {items[10 - (tmp1 - number)] = name->items[ui::mode - 1][number];}
	NameBitmapThread = new std::thread([this] {
		for(const auto&i:nameBitmaps)DeleteObject(i);
		nameBitmaps.clear();
		for (auto& item : items)
			nameBitmaps.push_back(CreateTransparentTextBitmap(item.name,mywindows::WW * 0.3, mywindows::WW * 0.3, ui::text_big));
		});
	NameBitmapThread->detach();
	if (ui::mode == config::getint(SPECIAL))buttons[addName].setDisable(false);
	else buttons[addName].setDisable(true);
	RandomList << RandomList.pt() << "[" << items[0].type << "类型]" <<
		"[" << items[0].star << "星]" << "[" << sth2sth::LWStostr(items[0].name.c_str()) << "]"
		<< RandomList.nl();
	if (!set2::offMusic)mciSendString(L"stop bgm", nullptr, 0, nullptr);
	changedStep();
}
void NameScreen::ShowName1() {
	enter();
	buttons[SKIP].setDisable(true);
	if (items[0].star >= 5)is5star = true;
	if (items[0].star == 4)is4star = true;
	if (!set2::offVideo) {
		if (is4star == 0 AND is5star == 0)
			explorer::getInstance()->PlayVideo(SIGNALSTAR3);
		if (is4star == 1 AND is5star == 0)
			explorer::getInstance()->PlayVideo(SIGNALSTAR4);
		if (is5star == 1)
			explorer::getInstance()->PlayVideo(SIGNALSTAR5);
	}
	isBall10 = false;
	mywindows::log("finish ball 1,number is %d", number);
}
void NameScreen::ShowName10() {
	enter();
	buttons[SKIP].setDisable(false);
	for (const auto& i : items)if (i.star >= 5)is5star = true;
	if (!set2::offVideo) {
		if (is5star == 1)explorer::getInstance()->PlayVideo(GROUPSTAR5);
		else explorer::getInstance()->PlayVideo(GROUPSTAR4);
	}
	isBall10 = true;
	mywindows::log("finish ball 10,number is %d", number);
}
void NameScreen::menu() {
	step = 0;
	isBall10 = false;
	ui::ScreenModeChanged = true;
	ui::MS->enter();
}
void NameScreen::paint() const
{
	const int x = mywindows::WW * 0.25;
	const int w = mywindows::WW * 0.5;
	const int y = (mywindows::WH - w)/2;
	const int h = w;
	p->Paint(0, 0, mywindows::WW, mywindows::WH, cardbackground);
	p->Paint(x, y, w, h, items[step].type + 21);
	p->DrawString(items[step].name, ui::text, mywindows::WW * 0.175, mywindows::WH * 8 / 13, 250, 250, 250);
	buttons[SKIP].paint();
	buttons[addName].paint();
	while (!nameButton->IsOK()) { Sleep(1); }
	nameButton->paint();
	ui::ScreenModeChanged = false;
	InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
}
void NameScreen::resetPoint()
{
	Screen::resetPoint();
	nameButton->setPoint(0, 0, mywindows::WW, mywindows::WH);
}
void NameScreen::setGp(Gp* p)
{
	this->p = p;
	nameButton->setGp(p);
	Screen::setGp(p);
}
void NameScreen::click(const int x, const int y)
{
	mciSendString(L"stop star3", nullptr, 0, nullptr);
	mciSendString(L"stop star4", nullptr, 0, nullptr);
	mciSendString(L"stop star5", nullptr, 0, nullptr);
	mciSendString(L"stop starfull", nullptr, 0, nullptr);
	bool clicked=false;
	for(auto &i:buttons)if(i.click(CLICK,x, y)==Clicked)clicked=true;
	p->ClearStaticPaintItem();
	if (clicked)return;
	if(isBall10){
		step++;
		if(step>=10){menu();return;}
		changedStep();
		return;
	}
	quit();
}
void NameScreen::printStars(const int star_number) const
{
	std::thread StarPainter([this, star_number]()
	{
		const int nowStep=step;
		Gp::StaticPaintInfo StarInfo={0,0,220,184,14,STAR,icon_star};
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		if (star_number == 6)std::this_thread::sleep_for(std::chrono::milliseconds(150));
		if(star_number!=6)
			for (int i = 0; i < star_number; i++)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				StarInfo.xDest=mywindows::WW*(0.177+i*0.01);
				StarInfo.yDest=mywindows::WH*31/52;
				if (nowStep != step)return;
				if (ui::ScreenMode != namescreen)return;
				p->AddStaticPaintItem(StarInfo);
				InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
			}
		else
			for (int i = 0; i < 1500; i++)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				const int x = name->randomIntegerBetween(0, mywindows::WW * 0.99);
				const int y = name->randomIntegerBetween(0, mywindows::WH * 0.99);
				StarInfo.xDest = x;
				StarInfo.yDest = y;
				if (nowStep != step)return;
				if (ui::ScreenMode != namescreen)return;
				p->AddStaticPaintItem(StarInfo);
				InvalidateRect(mywindows::main_hwnd, nullptr, FALSE);
			}
	});
	StarPainter.detach();
}
void NameScreen::AddName() const
{
	std::ofstream File;
	if (ui::mode == 1)File.open(config::getpath(NAMES1), std::ios::app);
	if (ui::mode == 2)File.open(config::getpath(NAMES2), std::ios::app);
	if (ui::mode == 3)File.open(config::getpath(NAMES3), std::ios::app);
	if (ui::mode == 4)File.open(config::getpath(NAMES4), std::ios::app);
	std::wstring t;
	int sr;
	if (isBall10) {
		sr = items[step - 1].star;
		t = items[step - 1].name;
	}
	else {
		sr = items[step].star;
		t = items[step].name;
	}
	t += L"-";
	t += std::to_wstring(sr);
	const std::string a = sth2sth::wstr2stru(t);
	File << '\n' << a;
	MessageBoxW(nullptr, t.c_str(), L"info", MB_OK);
	File.close();
}
void NameScreen::quit() const
{
	if (!set2::offMusic) {
		mciSendString(L"stop star3", nullptr, 0, nullptr);
		mciSendString(L"stop star4", nullptr, 0, nullptr);
		mciSendString(L"stop star5", nullptr, 0, nullptr);
		mciSendString(L"stop starfull", nullptr, 0, nullptr);
	}
	if(isBall10)return;
	ui::FS->enter();
}
std::vector<NameItem> NameScreen::getShowedItems() const
{
	std::vector<NameItem> showedItems;
	for (int star = 6; star >= 3; star--)
		for (const auto& item : items)
			if (star == item.star)showedItems.push_back(item);
	return showedItems;
}
void NameScreen::changedStep() const
{
	if(ui::ScreenMode!=namescreen)return;
	if(step>=10)return;
	printStars(items[step].star);
	const explorer* ptr = explorer::getInstance();
	switch (items[step].star)
	{case 3: ptr->PlayMusic("star3");break;
	case 4: ptr->PlayMusic("star4");break;
	case 5: ptr->PlayMusic("star5");break;
	case 6: ptr->PlayMusic("starfull");break;
	default:break;}
	while(nameBitmaps.size()<step+1)std::this_thread::sleep_for(std::chrono::milliseconds(1));
	nameButton->setHBITMAP(nameBitmaps[step]);
	nameButton->setPoint(0,0,mywindows::WW,mywindows::WH);
	const int x = mywindows::WW * 0.4;
	const int y = mywindows::WH * 0.3;
	const int xend = mywindows::WW * 0.6;
	const int yend = mywindows::WH * 0.6;
	nameButton->MoveTo(x, y, xend, yend, true, mywindows::WW*0.05, mywindows::WH*0.05);
}
HBITMAP CreateTransparentTextBitmap(const std::wstring& text, float width, float height, HFONT hFont) {
	// 初始化GDI+
	using namespace Gdiplus;
	unsigned char b = char(config::getint(TEXTB));
	unsigned char g = char(config::getint(TEXTG));
	unsigned char r = char(config::getint(TEXTR));
	// 创建GDI+位图
	Bitmap bitmap(INT(width), INT(height), PixelFormat32bppARGB);
	Graphics graphics(&bitmap);

	// 设置背景为透明
	graphics.Clear(Color(0, 0, 0, 0));

	// 设置文本格式
	HDC hdc = graphics.GetHDC();
	Font font(hdc, hFont);
	graphics.ReleaseHDC(hdc);
	SolidBrush brush(Color(255, r, g, b));
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	std::wstring Char;
	// 绘制文本
	RectF rect(0, 0, width,height);
	switch (text.size()) {
	case 0:break;
	case 2:
		rect = { width / 4,0,width * 3 / 4,height / 2 };
		Char = text.c_str()[0];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		rect = { width / 4,height / 4,width * 3 / 4,height };
		Char = text.c_str()[1];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		break;
	case 3:
		rect = { width / 2,0,width,height / 2 };
		Char = text.c_str()[0];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		rect = { 0,height / 3,width / 2,height * 3 / 2};
		Char = text.c_str()[1];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		rect = { width / 2,height / 2,width,height };
		Char = text.c_str()[2];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		break;
	case 4:
		rect = { 0,0,width / 2,height / 2 };
		Char = text.c_str()[0];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		rect = { 0,height / 2,width / 2,height};
		Char = text.c_str()[1];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		rect = { width / 2,0,width,height / 2 };
		Char = text.c_str()[2];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		rect = { width / 2,height / 2,width,height };
		Char = text.c_str()[3];
		graphics.DrawString(Char.c_str(), -1, &font, rect, &format, &brush);
		break;
	default:
		graphics.DrawString(text.c_str(), -1, &font, rect, &format, &brush);
		break;
	}

	// 将GDI+位图转换为HBITMAP
	HBITMAP hBitmap;
	bitmap.GetHBITMAP(Color(0, 0, 0, 0), &hBitmap);
	
	return hBitmap;
}