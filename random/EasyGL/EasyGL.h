#pragma once
#include <Windows.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Shader.h"


class VertexArray;
class IndexBuffer;
class Shader;
class Font;
class Bitmap;
class Buffer;
class ElementBuffer;
class color
{
public:
	float R, G, B, A;
	color(const double r, const double g, const double b, const double a = 1) :R(float(r)), G(float(g)), B(float(b)), A(float(a)) {}
	color(const int r, const int g, const int b, const int a = 255)
		:R(float(r) / 255.0f), G(float(g) / 255.0f), B(float(b) / 255.0f), A(float(a) / 255.0f) {}
	color(const unsigned int argb)
		:A(float((argb >> 24) & 0xFF) / 255.0f),
		R(float((argb >> 16) & 0xFF) / 255.0f),
		G(float((argb >> 8) & 0xFF) / 255.0f),
		B(float(argb & 0xFF) / 255.0f){}
	color() : R(0), G(0), B(0), A(0) {}
	color& operator=(unsigned int ARGB)
	{
		A = float((ARGB >> 24) & 0xFF) / 255.0f;
		R = float((ARGB >> 16) & 0xFF) / 255.0f;
		G = float((ARGB >> 8) & 0xFF) / 255.0f;
		B = float(ARGB & 0xFF) / 255.0f;
		return *this;
	};
	operator unsigned int() const
	{
		return (unsigned int(A * 255) << 24)
		| (unsigned int(R * 255) << 16)
		| (unsigned int(G * 255) << 8)
		| unsigned int(B * 255);
	};
	operator glm::vec3()const
	{
		return glm::vec3(R, G, B);
	};
};
class FpsCounter
{
    double lastTime =0;
	int nbFrames = 0;
	std::atomic<int> FPS = 0;
public:
	void newFrame();
	unsigned int getFps() const { return FPS; }
};

class EasyGL
{
public:
    EasyGL();
    ~EasyGL();

    // 初始化 OpenGL 上下文
    bool Init(HWND hwnd);
    // 清理资源
    void Cleanup();

    // 开始渲染
    void BeginRender() const;
    // 结束渲染
    void EndRender() const;
    // 调整视口大小
    void Resize(int width, int height);

    //绘制圆形
	void DrawCircle(float x, float y, float radius, color RGBA, bool filled = 1) const;
	//绘制矩形
	void DrawRectangle(float x, float y, float width, float height, color RGBA, bool filled = 1) const;
	//绘制线段
	void DrawLine(float x1, float y1, float x2, float y2, color RGBA) const;
	//绘制三角形
	void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, color RGBA, bool filled = 1) const;
	//绘制点
	void DrawPoint(float x, float y, color RGBA) const;
	//绘制椭圆
	void DrawEllipse(float x, float y, float a, float b, color RGBA, bool filled = 1) const;
	//绘制多边形
	void DrawPolygon(const float* x, const float* y, int n, color RGBA, bool filled = 1) const;
	//绘制弧线
	void DrawArc(float x, float y, float radius, float startAngle, float endAngle, color RGBA) const;
	//绘制扇形
	void DrawSector(float x, float y, float radius, float startAngle, float endAngle, color RGBA, bool filled = 1) const;
	//绘制文字
	void Draw_textRAW(float x, float y, int size, Font& font, const std::wstring& text, color RGBA) const;
    // 绘制位图
    void DrawBitmap(float x, float y, const Bitmap& bitmap, float angle = 0.0f) const;
    void DrawBitmap(float x, float y, float width, float height, const Bitmap& bitmap, float angle = 0.0f) const;

    // 绘制 VAO
    void DrawVAO(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

	void DrawString(const std::string& str, Font& font, int x, int y, int size, color RGBA) const;
	void DrawString(const std::wstring& str, Font& font, int x, int y, int size, color RGBA)const;
	void DrawStringBetween(const std::wstring& str, Font& font, int x, int y, int xend, int yend, int size, color RGBA) const;
	void DrawStringBetween(const std::string& str, Font& font, int x, int y, int xend, int yend, int size, color RGBA) const;
	void DrawVerticalString(const std::wstring& str, Font& font, int x, int y, int size, color RGBA) const;
	void DrawVerticalString(const std::string& str, Font& font, int x, int y, int size, color RGBA) const;
	void DrawVerticalStringBetween(const std::wstring& str, Font& font, int x, int y, int xend, int yend, int size, color RGBA) const;
	void DrawVerticalStringBetween(const std::string& str, Font& font, int x, int y, int xend, int yend, int size, color RGBA) const;

    // 设置清除颜色
    void SetClearColor(const color& color);

    // 是否初始化成功
    bool IsInitialized() const { return isInit; }


	void SetShader(const std::string& name, const Shader& new_shader);
	Shader& GetShader(const std::string& name);
	Shader& GetUsingShader();
	void useShader(const std::string& name);

private:
    HDC hDC = nullptr;       // 设备上下文
    HGLRC hRC = nullptr;     // 渲染上下文
    bool isInit = false;     // 是否初始化成功

    int windowWidth = 800;   // 窗口宽度
    int windowHeight = 600;  // 窗口高度

    color clearColor = { 0.1f, 0.1f, 0.1f, 1.0f }; // 清除颜色

	std::map<std::string, Shader> shaders; // 着色器
	std::string usingShaderName = "default"; // 当前使用的着色器名称
	double WHRatio = 0; // 窗口宽高比
	static Shader defaultShader; // 默认着色器
	FpsCounter fpsCounter; // 帧率计数器

	glm::vec2 ScreenToNDC(float x, float y) const;
	void InitDefaultShader();

};

inline void EasyGL::SetShader(const std::string& name, const Shader& new_shader)
{
	shaders[name] = new_shader;
}

inline Shader& EasyGL::GetShader(const std::string& name)
{
	return shaders[name];
}

inline Shader& EasyGL::GetUsingShader()
{
	return shaders[usingShaderName];
}

inline void EasyGL::useShader(const std::string& name)
{
	usingShaderName = name;
	shaders[usingShaderName].use();
}