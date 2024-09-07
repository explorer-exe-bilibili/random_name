#include "Gp.h"

#include <filesystem>
#include <regex>

#include"config.h"
#include "mywindows.h"
#include "resources_manager.h"
using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")

void Gp::Load_() {
	ptr=resources_manager::getInstance();
	//bitmaps.clear();
	//bitmaps.push_back(std::make_shared<Bitmap>(config::getpath(FLOATPHOTO).c_str()));
	//for (const auto& i : bitmaps) {
	//	if (i.get()->GetLastStatus() != Ok) {
	//		mywindows::errlog("��ͼƬʧ��");
	//	}
	//	else
	//	{
	//		HBITMAP tmp;
	//		BITMAP tmp_b;
	//		i->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &tmp);
	//		GetObject(tmp, sizeof(BITMAP), &tmp_b);
	//		HBitmaps.push_back(tmp);
	//		BITMAPs.push_back(tmp_b);
	//	}
	//}
	//bitmaps.clear();
}
void Gp::Load()
{
	ptr=resources_manager::getInstance();
	//bitmaps.clear();
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER1).c_str()));//0
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER2).c_str()));//1
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER3).c_str()));//2
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(OVER4).c_str()));//3
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\wish-background.jpg"));//4
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\cardbg.jpg"));//5
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink1.png"));//6
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\pink10.png"));//7
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue1.png"));//8
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\blue10.png"));//9
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\buttom.png"));//10
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\goldenbg.jpg"));//11
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\list.jpg"));//12
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\3list.png"));//13
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\4list.png"));//14
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\5list.png"));//15
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\fulllist.png"));//16
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\exit.png"));//17
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\setting.jpg"));//18
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\settingbuttom.png"));//19
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-bow.png"));//20
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-sword.png"));//21
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-magic.png"));//22
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-claymore.png"));//23
	//bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(L".\\files\\imgs\\bg-polearm.png"));//24


	//for (char t = 1; t <= config::getint(POOL_COUNT); t++)
	//{
	//	std::wstring configName = L"over" + std::to_wstring(t);
	//	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(config::getpath(configName).c_str()));

	//}
	//std::wstring directory = config::getpath(IMAGE_DIRECTORY);
	//std::wregex regex(LR"((\d+)\.\w+)");
	//std::vector<std::pair<int, std::wstring>> files;

	//for (const auto& entry : std::filesystem::directory_iterator(directory))
	//{
	//	if (entry.is_regular_file())
	//	{
	//		std::wsmatch match;
	//		std::wstring filename = entry.path().filename().wstring();
	//		if (std::regex_match(filename, match, regex))
	//		{
	//			int number = std::stoi(match[1].str());
	//			files.emplace_back(number, entry.path().wstring());
	//		}
	//	}
	//}

	//std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
	//	return a.first < b.first;
	//	});

	//for (const auto& file : files)
	//{
	//	bitmaps.push_back(std::make_shared<Gdiplus::Bitmap>(file.second.c_str()));
	//}
	//for (const auto& i : bitmaps) {
	//	if (i.get()->GetLastStatus() != Ok) {
	//		mywindows::errlog("��ͼƬʧ��");
	//	}
	//	else
	//	{
	//		HBITMAP tmp;
	//		BITMAP tmp_B;
	//		i->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &tmp);
	//		GetObject(tmp, sizeof(BITMAP), &tmp_B);
	//		HBitmaps.push_back(tmp);
	//		BITMAPs.push_back(tmp_B);
	//	}
	//}
	//bitmaps.clear();
}
Gp::~Gp() = default;
Gp::Gp(HWND hwnd):hwnd(hwnd)
{
	SizeChanged();
	if (hwnd == mywindows::main_hwnd)
		Load();
	else if (hwnd == mywindows::float_hWnd)
		Load_();
}

void Gp::Flush() {
    // ��ȡ���ڵ��豸������
    HDC hdcWindow = ::GetDC(hwnd);

    // ����һ������DC
    HDC hdcOffscreen = CreateCompatibleDC(hdcWindow);

    // ����һ���봰�ڴ�С��ͬ��λͼ
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, buffer->GetWidth(), buffer->GetHeight());

    // ��λͼѡ������DC
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcOffscreen, hBitmap);

    // �����������������ݻ��Ƶ�����DC��
    Graphics graphicsOffscreen(hdcOffscreen);
    graphicsOffscreen.DrawImage(buffer.get(), 0, 0);

    // ������DC�����ݻ��Ƶ�������
    BitBlt(hdcWindow, 0, 0, buffer->GetWidth(), buffer->GetHeight(), hdcOffscreen, 0, 0, SRCCOPY);

    // �ͷ���Դ
    SelectObject(hdcOffscreen, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcOffscreen);

    // �ͷ��豸������
    ::ReleaseDC(hwnd, hdcWindow);
}

void Gp::SizeChanged()
{
	// ��ȡ���ڵĿͻ�����С
	RECT rc;
	GetClientRect(mywindows::main_hwnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	// ���´���һ���봰�ڿͻ�����С��ͬ��Bitmap��Ϊ����������
	buffer = make_shared<Bitmap>(width, height, PixelFormat32bppARGB);
	// ���´���Graphics�������ڻ��Ƶ�����������
	graphic = make_shared<Graphics>(buffer.get());

}

void Gp::Paint(const int xDest, const int yDest, Bitmap* image, const int wDest = 0, const int hDest = 0)
{
	if (wDest != 0 && hDest != 0) {
		graphic.get()->DrawImage(image, xDest, yDest, wDest, hDest);
	}
	else {
		graphic.get()->DrawImage(image, xDest, yDest);
	}
}
void Gp::Paint(const int xDest, const int yDest, Bitmap* image)
{
	graphic.get()->DrawImage(image, xDest, yDest);
}
void Gp::Paint(const int xDest, const int yDest, const int wDest, const int hDest, const int number)
{
	//graphic.get()->DrawImage(bitmaps[number].get(), xDest, yDest, wDest, hDest);
	HDC hdc = GetDC();
	HDC hdcMem = CreateCompatibleDC(hdc);
	//SelectObject(hdcMem, HBitmaps[number]);
	//int w = BITMAPs[number].bmWidth;
	//int h = BITMAPs[number].bmHeight;
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP* t_bitmap = ptr->getBitmap(number);
	int w = t_bitmap->bmWidth;
	int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// ���û�Ϻ���
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255; // 255 = ��͸����0 = ��ȫ͸��
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // ʹ��Դͼ���Alphaͨ��

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	ReleaseDC(hdc);
	DeleteDC(hdcMem);
}
void Gp::Paint(const int xDest, const int yDest, const int number)
{
	//graphic.get()->DrawImage(bitmaps[number].get(), xDest, yDest);
	HDC hdc = GetDC();
	HDC hdcMem = CreateCompatibleDC(hdc);
	//SelectObject(hdcMem, HBitmaps[number]);
	//int w = BITMAPs[number].bmWidth;
	//int h = BITMAPs[number].bmHeight;
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP *t_bitmap = ptr->getBitmap(number);
	int w = t_bitmap->bmWidth;
	int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// ���û�Ϻ���
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = 255; // 255 = ��͸����0 = ��ȫ͸��
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // ʹ��Դͼ���Alphaͨ��

	AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
	ReleaseDC(hdc);
	DeleteDC(hdcMem);
}

//void Gp::DrawString(std::wstring str, const HFONT font, const int x, const int y, const int R, const int G, const int B)
//{
//	HDC hdc;
//	hdc= GetDC();
//	SetBkMode(hdc, TRANSPARENT);
//	SelectObject(hdc, font);
//	SetTextColor(hdc, RGB(R, G, B));
//	TextOut_(hdc, x, y, str.c_str());
//	ReleaseDC(hdc);
//}
//
//void Gp::DrawString(std::string str, const HFONT font, const int x, const int y, const int R, const int G, const int B)
//{
//	HDC hdc;
//	hdc = this->GetDC();
//	SetBkMode(hdc, TRANSPARENT);
//	SelectObject(hdc, font);
//	SetTextColor(hdc, RGB(R, G, B));
//	TextOutA(hdc, x, y, str.c_str(), str.size());
//	ReleaseDC(hdc);
//}

void Gp::DrawString(std::wstring str, const HFONT font, const int x, const int y, const int R, const int G, const int B) {
	HDC hdc = GetDC();
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, font);
	SetTextColor(hdc, RGB(R, G, B));

	// ����һ��GDI+ Graphics����
	Graphics graphics(hdc);
	// �����ı���ȾģʽΪ�����
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// ����һ��GDI+ Font����
	Font gdiPlusFont(hdc, font);
	// ����һ��GDI+ SolidBrush����
	SolidBrush brush(Color(255, R, G, B));

	// �����ı�
	graphics.DrawString(str.c_str(), -1, &gdiPlusFont, PointF(x, y), &brush);

	ReleaseDC(hdc);
}

void Gp::DrawString(std::string str, const HFONT font, const int x, const int y, const int R, const int G, const int B) {
	HDC hdc = GetDC();
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, font);
	SetTextColor(hdc, RGB(R, G, B));

	// ����һ��GDI+ Graphics����
	Graphics graphics(hdc);
	// �����ı���ȾģʽΪ�����
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// ����һ��GDI+ Font����
	Font gdiPlusFont(hdc, font);
	// ����һ��GDI+ SolidBrush����
	SolidBrush brush(Color(255, R, G, B));

	// �����ı�
	std::wstring wstr(str.begin(), str.end());
	graphics.DrawString(wstr.c_str(), -1, &gdiPlusFont, PointF(x, y), &brush);

	ReleaseDC(hdc);
}

HDC Gp::GetDC()
{
	return graphic.get()->GetHDC();
}

void Gp::ReleaseDC(const HDC hdc)
{
	graphic.get()->ReleaseHDC(hdc);
}
