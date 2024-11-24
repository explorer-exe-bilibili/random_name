#pragma once
#include <Windows.h>
#include <map>
#include <memory>
#include <string>

#include "Shader.h"


class Shader;
class Font;
class Bitmap;
class Buffer;
class ElementBuffer;
class color
{
public:
	float R, G, B, A;
	color(float r, float g, float b, float a) :R(r), G(g), B(b), A(a) {}
	color() :R(0), G(0), B(0), A(0) {}
	color& operator=(unsigned int RGBA);
	operator unsigned int() const;
};
class FpsCounter
{
	// 帧率计算变量
    double lastTime =0;
	int nbFrames = 0;
	std::atomic<int> FPS = 0;
public:
	void newFrame();
	unsigned int getFps() const { return FPS; }
};

class EasyGL
{
	bool isInit = false;
	double WHRatio;
	HDC hDC;
	HGLRC hRC = NULL;
	HWND hwnd;
	std::map<std::string ,Shader> shaders;
	std::string usingShaderName;
	Shader defaultShader;
public:
	EasyGL()=default;
	~EasyGL();
	bool init(HWND hwnd);
	void CleanupOpenGL();
	void ChangeSize();
	//绘制圆形
	void DrawCircle(float x, float y, float radius, color RGBA, bool filled=1);
	//绘制矩形
	void DrawRectangle(float x, float y, float width, float height, color RGBA, bool filled=1);
	//绘制线段
	void DrawLine(float x1, float y1, float x2, float y2, color RGBA);
	//绘制三角形
	void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, color RGBA, bool filled=1);
	//绘制点
	void DrawPoint(float x, float y, color RGBA, bool filled=1);
	//绘制椭圆
	void DrawEllipse(float x, float y, float a, float b, color RGBA, bool filled=1);
	//绘制多边形
	void DrawPolygon(float x[], float y[], int n, color RGBA, bool filled=1);
	//绘制弧线
	void DrawArc(float x, float y, float radius, float startAngle, float endAngle, color RGBA, bool filled=1);
	//绘制扇形
	void DrawSector(float x, float y, float radius, float startAngle, float endAngle, color RGBA, bool filled=1);
	//绘制文字
	void Draw_text(float x, float y,Font font, const char* text, color RGBA, bool filled=1);
	//绘制位图
	void DrawBitmap(float x, float y, Bitmap bitmap, float angle = 0);
	void DrawBitmap(float x, float y, float width, float height, Bitmap bitmap, float angle = 0);
	//绘制已有vao
	void DrawVAO(unsigned int vao,unsigned int point_count) const;

	void SetShader(const std::string& name, const Shader& new_shader);
	Shader& GetShader(const std::string& name);
	Shader& GetUsingShader();
	void useShader(const std::string& name);

	static void clear();
	void flush() const;

	static void initOpenGLBeforeWindow();
	static void initOpenGLAfterWindow();
};

