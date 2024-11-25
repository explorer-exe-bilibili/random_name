#pragma once
#include <Windows.h>
#include <map>
#include <memory>
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

    // 绘制位图
    void DrawBitmap(float x, float y, const Bitmap& bitmap, float angle = 0.0f) const;
    void DrawBitmap(float x, float y, float width, float height, const Bitmap& bitmap, float angle = 0.0f) const;

    // 绘制 VAO
    void DrawVAO(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

    // 设置清除颜色
    void SetClearColor(const color& color);

    // 是否初始化成功
    bool IsInitialized() const { return isInit; }

private:
    HDC hDC = nullptr;       // 设备上下文
    HGLRC hRC = nullptr;     // 渲染上下文
    bool isInit = false;     // 是否初始化成功

    int windowWidth = 800;   // 窗口宽度
    int windowHeight = 600;  // 窗口高度

    color clearColor = { 0.1f, 0.1f, 0.1f, 1.0f }; // 清除颜色
};