#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "Gp.h"

#include <codecvt>
#include<GL/gl.h>
#include<GL/GLU.h>
#include <filesystem>
#include "mywindows.h"
#include "explorer.h"
#include "sth2sth.h"
#include "ui.h"
using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


#define  _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
// 渲染中文文本
void RenderText(const std::wstring& text, float x, float y, float scale, const glm::vec3& color) {
	// 启用混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 设置颜色
	glColor3f(color.x, color.y, color.z);

	glEnable(GL_TEXTURE_2D);

	for (auto c = text.begin(); c != text.end(); c++) {
		wchar_t wc = *c;
		auto it = Characters.find(wc);
		if (it == Characters.end()) continue; // 跳过未加载的字符

		Character ch = it->second;

		float xpos = x + ch.BearingX * scale;
		float ypos = y - (ch.SizeY - ch.BearingY) * scale;

		float w = ch.SizeX * scale;
		float h = ch.SizeY * scale;

		// 绘制字符的四边形
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos, ypos);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos + w, ypos);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos + w, ypos + h);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos, ypos + h);
		glEnd();

		// 移动到下一个字符的位置
		x += (ch.Advance >> 6) * scale; // 位移以像素为单位
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

bool Gp::FontInfo::operator==(const FontInfo& newFont) const
{
	if (argb == newFont.argb && font == newFont.font)
		return true;
	else return false;
}

bool Gp::FontInfo::operator!=(const FontInfo& newFont) const
{
	if (argb!=newFont.argb || font != newFont.font)
		return true;
	else return false;
}

Gp::FontInfo::operator bool() const
{
	if (font)
		return true;
	else return false;
}

struct Gp::TextNeeds Gp::getTextNeeds()
{
	hdc = GetDC();
	if (!cachedGraphics)
	{
		TextNeeds.hdc_graphics = make_shared<Graphics>(hdc);
		TextNeeds.hdc_graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
		cachedGraphics = true;
	}
	if (!cachedFont)
	{
		TextNeeds.font = make_shared<Font>(hdc, NowFontInfo.font);
		cachedFont = true;
	}
	if (!cachedBrush)
	{
		TextNeeds.brush = make_shared<SolidBrush>(Color(NowFontInfo.argb));
		cachedBrush = true;
	}
	if(NowFontInfo!=LastFontInfo)
	{
		TextNeeds.brush.reset();
		TextNeeds.font.reset();
		TextNeeds.font = make_shared<Font>(hdc, NowFontInfo.font);
		TextNeeds.brush = make_shared<SolidBrush>(Color(NowFontInfo.argb));
		
		LastFontInfo = NowFontInfo;
	}
	return TextNeeds;
}

void Gp::releaseTextNeeds()
{
	TextNeeds.brush.reset();
	TextNeeds.font.reset();
	TextNeeds.hdc_graphics.reset();
	cachedBrush = false;
	cachedFont = false;
	cachedGraphics = false;
}

void Gp::PaintStaticItems()
{
	std::lock_guard lock(SizeChangeMutex);
	for (auto& i : StaticPaintList)
	{
		if (IsBadReadPtr(i.str.c_str(), sizeof(wchar_t) * i.str.length()))continue;
		if (i.str.empty())continue;
		DrawString(i.str, i.font, i.xDest, i.yDest, i.argb);
	}
}

Gp::~Gp(){
	if (cachedHDC)
		ReleaseDC(hdc);
	delete ptr;
}
Gp::Gp(HWND hwnd) : hwnd(hwnd)
{
	bool a=initOpenGL(hwnd);
}

bool Gp::initOpenGL(HWND hWnd)
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // 支持双缓冲
		PFD_TYPE_RGBA,
		32, // 颜色位数
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24, // 深度缓冲区
		8,  // 模板缓冲区
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	hDC = ::GetDC(hWnd);
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (pixelFormat == 0) return false;

	if (!SetPixelFormat(hDC, pixelFormat, &pfd)) return false;

	hRC = wglCreateContext(hDC);
	if (hRC == NULL) return false;

	if (!wglMakeCurrent(hDC, hRC)) return false;

	// 初始化OpenGL设置
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 设置视口
	RECT rect;
	GetClientRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	glViewport(0, 0, width, height);

	// 设置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);

	// 设置模型视图矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadIdentity();

	// 设置视图矩阵
	gluLookAt(0.0f, 0.0f, 5.0f,   // 眼睛位置
		0.0f, 0.0f, 0.0f,   // 注视点
		0.0f, 1.0f, 0.0f);  // 上方向
	// 设置正交投影用于2D渲染
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, rect.right, rect.bottom, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// 初始化字体
	initFont();

	//// 初始化FPS计时
	//frameCount = 0;
	//fps = 0.0;
	lastTime = std::chrono::steady_clock::now();

	return true;
}

void Gp::initFont()
{
	HFONT font = CreateFontA(
		16,                        // 高度
		0,                         // 宽度
		0,                         // 款角
		0,                         // 文字方向
		FW_NORMAL,                 // 字重
		FALSE,                     // 斜体
		FALSE,                     // 下划线
		FALSE,                     // 删除线
		ANSI_CHARSET,              // 字符集
		OUT_TT_PRECIS,             // 输出精度
		CLIP_DEFAULT_PRECIS,       // 裁剪精度
		ANTIALIASED_QUALITY,       // 输出质量
		FF_DONTCARE | DEFAULT_PITCH, // 家族和间距
		"Arial");                  // 字体名称

	SelectObject(hDC, font);
	base = glGenLists(256);
	wglUseFontBitmaps(hDC, 0, 256, base);
	DeleteObject(font);
}

void Gp::renderBitmapString(float x, float y, void* font, const char* string)
{
	glRasterPos2f(x, y);
	const char* c;
	for (c = string; *c != '\0'; c++) {
		glCallList(base + (unsigned char)*c);
	}
}

void Gp::RenderOpenGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 更新FPS计数
	frameCount++;
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = currentTime - lastTime;
	if (elapsed.count() >= 1.0) {
		fps = (frameCount-lastCount) / elapsed.count();
		lastCount = frameCount;
		lastTime = currentTime;
	}



	// 禁用深度测试以确保文本显示在前
	glDisable(GL_DEPTH_TEST);

	// 设置文本颜色
	glColor3f(1.0f, 1.0f, 1.0f); // 白色

	// 渲染FPS文本
	wstring fpswString = L"FPS:" + to_wstring(fps);
	DrawString(fpswString, ui::text, 10, 20,ARGB(255,255,0,0));
	// 恢复投影和模型视图矩阵
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	SwapBuffers(hDC);
}

void Gp::Flush() {
	PaintStaticItems();
	SwapBuffers(hDC);
}

void Gp::SizeChanged()
{
	std::lock_guard lock(SizeChangeMutex);
	if (cachedHDC)
		ReleaseDC(hdc);
	// 获取窗口的客户区大小
	RECT rc;
	GetClientRect(mywindows::main_hwnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	// 重新创建一个与窗口客户区大小相同的Bitmap作为离屏缓冲区
	buffer = make_shared<Bitmap>(width, height, PixelFormat32bppARGB);
	// 重新创建Graphics对象用于绘制到离屏缓冲区
	graphic = make_shared<Graphics>(buffer.get());
	releaseTextNeeds();
	cachedBrush = false;
	cachedFont = false;
	cachedGraphics = false;
	cachedHDC = false;
	StaticPaintList.clear();
}

void Gp::Paint(const int xDest, const int yDest, Bitmap* image, const int wDest = 0, const int hDest = 0)
{
	std::lock_guard lock(SizeChangeMutex);
	ReleaseDC(hdc);
	if (wDest != 0 && hDest != 0) {
		graphic->DrawImage(image, xDest, yDest, wDest, hDest);
	}
	else {
		graphic->DrawImage(image, xDest, yDest);
	}
}
void Gp::Paint(const int xDest, const int yDest, Bitmap* image)
{
	std::lock_guard lock(SizeChangeMutex);
	ReleaseDC(hdc);
	graphic->DrawImage(image, xDest, yDest);
}
void Gp::Paint(int xDest, int yDest, int wDest, int hDest, int number, unsigned char alpha_count)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	const HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP* t_bitmap = ptr->getBitmap(number);
	const int w = t_bitmap->bmWidth;
	const int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}
void Gp::Paint(int xDest, int yDest, int number, unsigned char alpha_count)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	const HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = ptr->getHBitmap(number);
	const BITMAP *t_bitmap = ptr->getBitmap(number);
	const int w = t_bitmap->bmWidth;
	const int h = t_bitmap->bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, w, h, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}
void Gp::Paint(int xDest, int yDest, int wDest, int hDest, HBITMAP hbitmap, unsigned char alpha_count)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	const HDC hdcMem = CreateCompatibleDC(hdc);
	const HBITMAP t_hbtiamp = hbitmap;
	BITMAP t_bitmap;
	GetObject(t_hbtiamp, sizeof(BITMAP), &t_bitmap);
	const int w = t_bitmap.bmWidth;
	const int h = t_bitmap.bmHeight;
	SelectObject(hdcMem, t_hbtiamp);
	// 设置混合函数
	BLENDFUNCTION blendFunc;
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.BlendFlags = 0;
	blendFunc.SourceConstantAlpha = alpha_count; // 255 = 不透明，0 = 完全透明
	blendFunc.AlphaFormat = AC_SRC_ALPHA; // 使用源图像的Alpha通道

	AlphaBlend(hdc, xDest, yDest, wDest, hDest, hdcMem, 0, 0, w, h, blendFunc);
	DeleteDC(hdcMem);
}

void Gp::DrawString(const std::wstring& str, const HFONT font, const int x, const int y,uint32_t argb) {
	// 解析 ARGB 值
	float a = ((argb >> 24) & 0xFF) / 255.0f;
	float r = ((argb >> 16) & 0xFF) / 255.0f;
	float g = ((argb >> 8) & 0xFF) / 255.0f;
	float b = (argb & 0xFF) / 255.0f;
	// 保存当前矩阵状态
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// 获取窗口大小
	RECT rect;
	GetClientRect(mywindows::main_hwnd, &rect);
	gluOrtho2D(0, rect.right, rect.bottom, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// 设置文本颜色
	glColor4f(r, g, b, a);

	// 禁用深度测试以确保文本显示在前
	glDisable(GL_DEPTH_TEST);

	string str_ = sth2sth::wstr2stru(str);
	// 渲染字符串
	glRasterPos2f(x, y);
	for (char c : str_) {
		glCallList(base + static_cast<unsigned char>(c));
	}
	// 恢复状态
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Gp::DrawString(const std::string& str, const HFONT font, const int x, const int y,uint32_t argb) {
	// 解析 ARGB 值
	float a = ((argb >> 24) & 0xFF) / 255.0f;
	float r = ((argb >> 16) & 0xFF) / 255.0f;
	float g = ((argb >> 8) & 0xFF) / 255.0f;
	float b = (argb & 0xFF) / 255.0f;
	// 保存当前矩阵状态
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// 获取窗口大小
	RECT rect;
	GetClientRect(mywindows::main_hwnd, &rect);
	gluOrtho2D(0, rect.right, rect.bottom, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// 设置文本颜色
	glColor4f(r, g, b, a);

	// 禁用深度测试以确保文本显示在前
	glDisable(GL_DEPTH_TEST);

	// 渲染字符串
	glRasterPos2f(x, y);
	for (char c : str) {
		glCallList(base + static_cast<unsigned char>(c));
	}
	// 恢复状态
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Gp::DrawStringBetween(const std::wstring& str, HFONT font, int x, int y, int xend, int yend, uint32_t argb){
	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	std::lock_guard lock(SizeChangeMutex);
	const RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(str.c_str(), -1, TextNeeds.font.get(), layoutRect, &format, TextNeeds.brush.get());
}

void Gp::DrawstringBetween(const std::string& str, HFONT font, int x, int y, int xend, int yend,uint32_t argb)
{
	std::lock_guard lock(SizeChangeMutex);
	// 创建一个GDI+ RectF对象，表示文本绘制的区域
	const RectF layoutRect(x, y, xend - x, yend - y);

	// 创建一个GDI+ StringFormat对象，用于设置文本格式
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 绘制文本
	const std::wstring wstr(str.begin(), str.end());
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), layoutRect, &format, TextNeeds.brush.get());

}

void Gp::DrawVerticalString(const std::wstring& str, const HFONT font, const int x, const int y,uint32_t argb) {
	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 绘制每个字符
	int yOffset = y;
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawVerticalString(const std::string& str, const HFONT font, const int x, const int y,uint32_t argb) {

	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 绘制每个字符
	int yOffset = y;
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawVerticalStringBetween(const std::wstring& str, const HFONT font, const int x, const int y, const int xend, const int yend,uint32_t argb) {
	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 计算字符串的总高度
	int totalHeight = 0;
	for (const wchar_t& ch : str) {
		totalHeight += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get());
	}

	// 计算起始y偏移量，使字符串在垂直方向居中
	int yOffset = y + (yend - y - totalHeight) / 2;

	// 计算起始x偏移量，使字符串在水平方向居中
	const int xOffset = x + (xend - x - TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get())) / 2;

	// 绘制每个字符
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(),
			PointF(xOffset, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawVerticalStringBetween(const std::string& str, const HFONT font, const int x, const int y, const int xend, const int yend,uint32_t argb) {
	std::lock_guard lock(SizeChangeMutex);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 计算字符串的总高度
	int totalHeight = 0;
	for (const wchar_t& ch : str) {
		totalHeight += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get());
	}

	// 计算起始y偏移量，使字符串在垂直方向居中
	int yOffset = y + (yend - y - totalHeight) / 2;

	// 计算起始x偏移量，使字符串在水平方向居中
	const int xOffset = x + (xend - x - TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get())) / 2;

	// 绘制每个字符
	for (const wchar_t& ch : str) {
		std::wstring wstr(1, ch);
		TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), 
			PointF(xOffset, yOffset), TextNeeds.brush.get());
		yOffset += TextNeeds.font->GetHeight(TextNeeds.hdc_graphics.get()); // 更新y偏移量
	}
}

void Gp::DrawChar(const wchar_t ch, HFONT font, int x, int y,uint32_t argb)
{
	std::lock_guard lock(SizeChangeMutex);
	const std::wstring wstr(1, ch);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::DrawChar(const char ch, HFONT font, int x, int y,uint32_t argb)
{
	std::lock_guard lock(SizeChangeMutex);
	const std::wstring wstr(1, ch);
	NowFontInfo = { argb,font };
	const auto TextNeeds = getTextNeeds();
	// 绘制文本
	TextNeeds.hdc_graphics->DrawString(wstr.c_str(), -1, TextNeeds.font.get(), PointF(x, y), TextNeeds.brush.get());
}

void Gp::AddStaticPaintItem(const StaticPaintInfo& Item)
{
	std::lock_guard lock(SizeChangeMutex);
	StaticPaintList.push_back(Item);
}

void Gp::ClearStaticPaintItem()
{
	std::lock_guard lock(SizeChangeMutex);
	StaticPaintList.clear();
}

HDC Gp::GetDC()
{
	if (cachedHDC)
		return hdc;
	else
	{
		cachedHDC = true;
		return graphic->GetHDC();
	}
}

void Gp::ReleaseDC(const HDC hdc)
{
	if (cachedHDC) {
		graphic->ReleaseHDC(hdc);
		cachedHDC = false;
	}
}

void Gp::DrawSquare(int xDest, int yDest, int xEnd, int yEnd, uint32_t color, bool filled)
{
	std::lock_guard lock(SizeChangeMutex);
	hdc = GetDC();
	if (filled) {
		// 绘制实心矩形
		const HBRUSH hBrush = CreateSolidBrush(color&0x00FFFFFF); // 红色实心刷子
		const HBRUSH hOldBrush = HBRUSH(SelectObject(hdc, hBrush));
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}
	else {
		// 绘制空心矩形
		const HPEN hPen = CreatePen(PS_SOLID, 2, color&0x00FFFFFF); // 蓝色画笔
		const HPEN hOldPen = HPEN(SelectObject(hdc, hPen));
		const HBRUSH hNullBrush = HBRUSH(SelectObject(hdc, GetStockObject(NULL_BRUSH)));
		Rectangle(hdc, xDest, yDest, xEnd, yEnd);
		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hNullBrush);
		DeleteObject(hPen);
	}
}

void Gp::LockWindowSize() const
{
	// 获取当前窗口样式
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	// 移除可调整大小和最大化按钮的样式
	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	// 设置新的窗口样式
	SetWindowLong(hwnd, GWL_STYLE, style);
	// 强制窗口重新绘制以应用新的样式
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void Gp::UnlockWindowSize() const
{
	// 获取当前窗口样式
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	// 添加可调整大小和最大化按钮的样式
	style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
	// 设置新的窗口样式
	SetWindowLong(hwnd, GWL_STYLE, style);
	// 强制窗口重新绘制以应用新的样式
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}
