// Canvas.h
#pragma once
#include <glad/glad.h>
#include"Shader.h"
#include "VertexBuffer.h"

class Canvas {
public:
	// 构造函数，创建新的画布，添加窗口宽度和高度参数
	Canvas(int width, int height, int screenWidth, int screenHeight);

	// 析构函数，清理资源
	~Canvas();

	// 开始在画布上绘制
	void Begin();

	// 结束在画布上的绘制，返回窗口绘制
	void End();

	// 将画布内容绘制到屏幕上的指定位置
	void DrawToScreen(float x, float y, float width, float height);

	// 拷贝画布内容（如有需要，可实现为拷贝到另一个纹理）
	void Copy(Canvas& targetCanvas);

	// 获取画布的宽度和高度
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

private:
	// 初始化帧缓冲对象
	void InitFramebuffer();

	// 初始化屏幕四边形
	void InitScreenQuad();
	GLuint fbo; // FBO对象的句柄
	GLuint rbo; // 渲染缓冲区对象的句柄
	GLuint colorBuffer; // 颜色缓冲区对象的句柄
	GLuint depthBuffer; // 深度缓冲区对象的句柄
	GLuint texture; // 生成的纹理对象的句柄
	// 画布的宽度和高度
	int width;
	int height;

	// 窗口的宽度和高度
	int screenWidth;
	int screenHeight;

	// 使用您的缓冲区类替代原始的 OpenGL 对象 ID
	VertexArray quadVAO;
	VertexBuffer quadVBO;

	// 着色器程序，用于绘制屏幕四边形
	static Shader screenShader;
};
