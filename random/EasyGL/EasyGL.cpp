#include "EasyGL.h"

#include <glad/glad.h>
#include <gl/GL.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "Bitmap.h"
#include "My_Font.h"
#include "../sth2sth.h"

Shader EasyGL::defaultShader;

EasyGL::EasyGL()
= default;

EasyGL::~EasyGL()
{
	Cleanup();
}

bool EasyGL::Init(const HWND hwnd)
{
	// 获取设备上下文
	hDC = GetDC(hwnd);
	if (!hDC)
	{
		std::cerr << "获取设备上下文失败！" << std::endl;
		return false;
	}

	// 设置像素格式
	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR) };
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // 支持双缓冲
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (!pixelFormat)
	{
		std::cerr << "选择像素格式失败！" << std::endl;
		return false;
	}

	if (!SetPixelFormat(hDC, pixelFormat, &pfd))
	{
		std::cerr << "设置像素格式失败！" << std::endl;
		return false;
	}

	// 创建 OpenGL 渲染上下文
	hRC = wglCreateContext(hDC);
	if (!hRC)
	{
		std::cerr << "创建 OpenGL 渲染上下文失败！" << std::endl;
		return false;
	}

	if (!wglMakeCurrent(hDC, hRC))
	{
		std::cerr << "激活当前 OpenGL 渲染上下文失败！" << std::endl;
		return false;
	}

	// 初始化 GLAD
	if (!gladLoadGL())
	{
		std::cerr << "初始化 GLAD 失败！" << std::endl;
		return false;
	}

	// 设置视口
	RECT rect;
	GetClientRect(hwnd, &rect);
	windowWidth = rect.right - rect.left;
	windowHeight = rect.bottom - rect.top;
	glViewport(0, 0, windowWidth, windowHeight);

	// 启用 Alpha 混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	InitDefaultShader();

	isInit = true;
	return true;
}

void EasyGL::Cleanup()
{
	if (hRC)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(hRC);
		hRC = nullptr;
	}
	if (hDC)
	{
		ReleaseDC(WindowFromDC(hDC), hDC);
		hDC = nullptr;
	}
	isInit = false;
}

void EasyGL::BeginRender() const
{
	if (!isInit) return;

	// 清除颜色缓冲区
	glClearColor(clearColor.R, clearColor.G, clearColor.B, clearColor.A);
	glClear(GL_COLOR_BUFFER_BIT);

	// 设置视口
	glViewport(0, 0, windowWidth, windowHeight);
}

void EasyGL::EndRender() const
{
	if (!isInit) return;

	// 交换前后缓冲区
	SwapBuffers(hDC);
}

void EasyGL::Resize(const int width, const int height)
{
	if (!isInit) return;

	windowWidth = width;
	windowHeight = height;
	WHRatio = width / height;

	glViewport(0, 0, windowWidth, windowHeight);
}


void EasyGL::InitDefaultShader()
{
	// 简单的着色器，用于绘制基本图元
	const char* vertexShaderSource = R"(
	#version 330 core
	layout(location = 0) in vec2 aPos;

	uniform vec4 u_Color;

	void main()
	{
		gl_Position = vec4(aPos, 0.0, 1.0);
	}
	)";

	const char* fragmentShaderSource = R"(
	#version 330 core
	out vec4 FragColor;
	uniform vec4 u_Color;

	void main()
	{
		FragColor = u_Color;
	}
	)";

	defaultShader.init(vertexShaderSource, fragmentShaderSource);
	shaders["default"] = defaultShader; // 存储默认着色器
	usingShaderName = "default";
}

glm::vec2 EasyGL::ScreenToNDC(const float x, const float y) const
{
	float ndcX = (x / windowWidth) * 2.0f - 1.0f;
	float ndcY = 1.0f - (y / windowHeight) * 2.0f;
	return {ndcX, ndcY};
}

void EasyGL::DrawLine(const float x1, const float y1, const float x2, const float y2, const color RGBA) const
{
	if (!isInit) return;

	glm::vec2 p1 = ScreenToNDC(x1, y1);
	glm::vec2 p2 = ScreenToNDC(x2, y2);

	float vertices[] = {
		p1.x, p1.y,
		p2.x, p2.y
	};

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices, sizeof(vertices));

	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	glDrawArrays(GL_LINES, 0, 2);
	VertexArray::Unbind();
}

void EasyGL::DrawRectangle(float x, float y, float width, float height, color RGBA, bool filled)const
{
	if (!isInit) return;

	glm::vec2 p1 = ScreenToNDC(x, y);
	glm::vec2 p2 = ScreenToNDC(x + width, y + height);

	float vertices[] = {
		p1.x, p1.y,  // 左下角
		p2.x, p1.y,  // 右下角
		p2.x, p2.y,  // 右上角
		p1.x, p2.y   // 左上角
	};

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices, sizeof(vertices));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)0);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	if (filled)
	{
		// 使用索引绘制填充矩形
		unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
		IndexBuffer ib(indices, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	else
	{
		// 使用 glDrawArrays 绘制未填充矩形
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}
	VertexArray::Unbind();
}

void EasyGL::DrawCircle(const float x, const float y, const float radius, const color RGBA, const bool filled) const
{
	if (!isInit) return;

	constexpr int segments = 100;
	std::vector<float> vertices;

	glm::vec2 center = ScreenToNDC(x, y);
	float radiusX = radius / (windowWidth / 2.0f);
	float radiusY = radius / (windowHeight / 2.0f);

	for (int i = 0; i <= segments; ++i)
	{
		float angle = 2.0f * 3.1415926f * i / segments;
		float dx = cos(angle) * radiusX;
		float dy = sin(angle) * radiusY;
		vertices.push_back(center.x + dx);
		vertices.push_back(center.y + dy);
	}

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	if (filled)
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 1);
	}
	else
	{
		glDrawArrays(GL_LINE_LOOP, 0, segments);
	}
	VertexArray::Unbind();
}

void EasyGL::DrawEllipse(const float x, const float y, const float a, const float b, const color RGBA, const bool filled) const
{
	if (!isInit) return;

	constexpr int segments = 100;
	std::vector<float> vertices;

	glm::vec2 center = ScreenToNDC(x, y);
	float radiusX = a / (windowWidth / 2.0f);
	float radiusY = b / (windowHeight / 2.0f);

	for (int i = 0; i <= segments; ++i)
	{
		float angle = 2.0f * 3.1415926f * i / segments;
		float dx = cos(angle) * radiusX;
		float dy = sin(angle) * radiusY;
		vertices.push_back(center.x + dx);
		vertices.push_back(center.y + dy);
	}

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	if (filled)
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 1);
	}
	else
	{
		glDrawArrays(GL_LINE_LOOP, 0, segments);
	}
	VertexArray::Unbind();
}

void EasyGL::DrawTriangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const color RGBA, const bool filled) const
{
	if (!isInit) return;

	glm::vec2 p1 = ScreenToNDC(x1, y1);
	glm::vec2 p2 = ScreenToNDC(x2, y2);
	glm::vec2 p3 = ScreenToNDC(x3, y3);

	float vertices[] = {
		p1.x, p1.y,
		p2.x, p2.y,
		p3.x, p3.y
	};

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices, sizeof(vertices));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	if (filled)
	{
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	else
	{
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	}
	VertexArray::Unbind();
}

void EasyGL::DrawPoint(const float x, const float y, const color RGBA) const
{
	if (!isInit) return;

	glm::vec2 p = ScreenToNDC(x, y);

	float vertices[] = { p.x, p.y };

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices, sizeof(vertices));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 设置点的大小
	glPointSize(5.0f);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	glDrawArrays(GL_POINTS, 0, 1);
	VertexArray::Unbind();
}

void EasyGL::DrawPolygon(const float* x, const float* y, const int n, const color RGBA, const bool filled) const
{
	if (!isInit) return;
	if (n < 3) return;

	std::vector<float> vertices;
	for (int i = 0; i < n; ++i)
	{
		glm::vec2 p = ScreenToNDC(x[i], y[i]);
		vertices.push_back(p.x);
		vertices.push_back(p.y);
	}

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	if (filled)
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, n);
	}
	else
	{
		glDrawArrays(GL_LINE_LOOP, 0, n);
	}
	VertexArray::Unbind();
}

void EasyGL::DrawArc(const float x, const float y, const float radius, const float startAngle, const float endAngle, const color RGBA) const
{
	if (!isInit) return;

	constexpr int segments = 100;
	std::vector<float> vertices;

	glm::vec2 center = ScreenToNDC(x, y);
	float radiusX = radius / (windowWidth / 2.0f);
	float radiusY = radius / (windowHeight / 2.0f);

	float startRad = glm::radians(startAngle);
	float endRad = glm::radians(endAngle);
	float angleStep = (endRad - startRad) / segments;

	for (int i = 0; i <= segments; ++i)
	{
		float angle = startRad + i * angleStep;
		float dx = cos(angle) * radiusX;
		float dy = sin(angle) * radiusY;
		vertices.push_back(center.x + dx);
		vertices.push_back(center.y + dy);
	}

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	glDrawArrays(GL_LINE_STRIP, 0, segments + 1);
	VertexArray::Unbind();
}

void EasyGL::DrawSector(const float x, const float y, const float radius, const float startAngle, const float endAngle, const color RGBA, const bool filled) const
{
	if (!isInit) return;

	constexpr int segments = 100;
	std::vector<float> vertices;

	glm::vec2 center = ScreenToNDC(x, y);
	float radiusX = radius / (windowWidth / 2.0f);
	float radiusY = radius / (windowHeight / 2.0f);

	float startRad = glm::radians(startAngle);
	float endRad = glm::radians(endAngle);
	float angleStep = (endRad - startRad) / segments;

	// 起始点为中心
	if (filled)
	{
		vertices.push_back(center.x);
		vertices.push_back(center.y);
	}

	for (int i = 0; i <= segments; ++i)
	{
		float angle = startRad + i * angleStep;
		float dx = cos(angle) * radiusX;
		float dy = sin(angle) * radiusY;
		vertices.push_back(center.x + dx);
		vertices.push_back(center.y + dy);
	}

	// 创建 VAO 和 VBO
	VertexArray va;
	VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

	// 使用默认着色器
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));

	// 绘制
	va.Bind();
	if (filled)
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
	}
	else
	{
		glDrawArrays(GL_LINE_STRIP, 0, segments + 1);
	}
	VertexArray::Unbind();
}

void EasyGL::Draw_textRAW(const float x, const float y, const int size, Font& font, const std::wstring& text, const color RGBA) const
{
	if (!isInit) return;
	float scale = size / font.GetFontSize();
	// 渲染文本
	font.RenderText(text, x, y, scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));
}

void EasyGL::DrawString(const std::string& str, Font& font, const int x, const int y, const int size, const color RGBA) const
{
	if (!isInit) return;
	float scale = float(size) / font.GetFontSize();
	auto wstr = sth2sth::str2wstr(str);
	// 渲染文本
	font.RenderText(wstr, static_cast<float>(x), windowHeight-y-font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));
}

void EasyGL::DrawString(const std::wstring& str, Font& font, const int x, const int y, const int size, const color RGBA) const
{
	if (!isInit) return;
	float scale = size / font.GetFontSize();
	// 渲染文本
	font.RenderText(str, static_cast<float>(x), windowHeight - y - font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));
}

void EasyGL::DrawStringBetween(const std::wstring& str, Font& font, int x, int y, int xend, int yend, int size, color RGBA) const
{
	if (!isInit) return;
	int width = 0, tmp_width = 0;
	int height = 0, tmp_height = 0;
	int MaxWidth = xend - x;
	int MaxHeight = yend - y;
	float scale = size / font.GetFontSize();
	// 计算文本宽度和高度
	for (size_t i = 0; i < str.length(); ++i)
	{
		width += size;
		if (width > MaxWidth)
		{
			tmp_width = MaxWidth;
			width = 0;
			height += size;
			if (height > MaxHeight)
			{
				break;
			}
		}
	}
	width = std::max(tmp_width, width);
	height = std::min(tmp_height, height);

	int posX = x + (MaxWidth - width) / 2;
	int posY = y + (MaxHeight - height) / 2;
	posY = windowHeight - posY;
	int nowWidth = 0;
	int nowHeight = 0;
	for (size_t i = 0; i < str.length(); ++i)
	{
		wchar_t c = str[i];
		// 渲染单个字符
		font.RendChar(c, posX, posY - font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));
		// 更新位置，向右移动
		posX += size;
		nowWidth += size;
		// 如果超过窗口宽度，换行
		if (nowWidth > MaxWidth)
		{
			posX = x + (MaxWidth - width) / 2;
			posY -= size;
			nowHeight += size;
			nowWidth = 0;
			if (nowHeight > MaxHeight)break;
		}
	}
}

void EasyGL::DrawStringBetween(const std::string& str, Font& font, const int x, const int y, const int xend, const int yend, const int size, const color RGBA) const
{
	if (!isInit) return;
	auto wstr = sth2sth::str2wstr(str);
	int width = 0, tmp_width = 0;
	int height = 0, tmp_height = 0;
	int MaxWidth = xend - x;
	int MaxHeight = yend - y;
	float scale = size / font.GetFontSize();
	// 计算文本宽度和高度
	for (size_t i=0;i<wstr.length();++i)
	{
		width += size;
		if (width>MaxWidth)
		{
			tmp_width = MaxWidth;
			width = 0;
			height += size;
			if (height > MaxHeight)
			{
				break;
			}
		}
	}
	width=std::max(tmp_width, width);
	height = std::min(tmp_height, height);

	int posX = x + (MaxWidth - width) / 2;
	int posY = y + (MaxHeight - height) / 2;
	posY = windowHeight - posY;
	int nowWidth = 0;
	int nowHeight = 0;
	for (size_t i = 0; i < wstr.length(); ++i)
	{
		wchar_t c = wstr[i];
		// 渲染单个字符
		font.RendChar(c, posX, posY - font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));
		// 更新位置，向右移动
		posX += size;
		nowWidth += size;
		// 如果超过窗口宽度，换行
		if (nowWidth > MaxWidth)
		{
			posX = x + (MaxWidth - width) / 2;
			posY -= size;
			nowHeight += size;
			nowWidth = 0;
			if (nowHeight > MaxHeight)break;
		}
	}
}

void EasyGL::DrawVerticalString(const std::wstring& str, Font& font, const int x, const int y, const int size, const color RGBA) const
{
	if (!isInit) return;
	float scale = float(size) / font.GetFontSize();

	// 渲染竖排文本
	float posY = static_cast<float>(windowHeight - y);
	int line = 0;;

	for (long long i = str.length() - 1; i >= 0; --i) {        // 如果超过窗口高度，换列
		posY -= size;
		if (posY - size < 0)
		{
			posY = static_cast<float>(windowHeight - y);
			line++;
		}
	}
	posY = static_cast<float>(windowHeight - y);
	float posX = static_cast<float>(x);
	font.RenderText(std::to_wstring(line), 0, 0, 1, glm::vec3(1, 1, 1));
	for (size_t i = 0; i < str.length(); ++i)

	{
		wchar_t c = str[i];
		// 渲染单个字符
		font.RendChar(c, posX, posY - font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));

		// 更新位置，向下移动
		posY -= size;

		// 如果超过窗口高度，换列
		if (posY - size < 0)
		{
			posY = static_cast<float>(windowHeight - y);
			posX += size;
		}
	}
}

void EasyGL::DrawVerticalString(const std::string& str, Font& font, const int x, const int y, const int size, const color RGBA) const
{
	if (!isInit) return;
	float scale = float(size) / font.GetFontSize();

	// 渲染竖排文本
	float posY = static_cast<float>(windowHeight - y);
	int line = 0;
	auto wstr = sth2sth::str2wstr(str);

	for (long long i = wstr.length() - 1; i >= 0; --i){        // 如果超过窗口高度，换列
		posY -= size;
		if (posY - size < 0)
		{
			posY = static_cast<float>(windowHeight - y);
			line++;
		}
	}
	posY = static_cast<float>(windowHeight - y);
	float posX = static_cast<float>(x);
	font.RenderText(std::to_wstring(line), 0, 0, 1, glm::vec3(1, 1, 1));
	for (size_t i = 0; i < wstr.length(); ++i)

	{
		wchar_t c = wstr[i];
		// 渲染单个字符
		font.RendChar(c, posX, posY - font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));

		// 更新位置，向下移动
		posY -= size;

		// 如果超过窗口高度，换列
		if (posY - size < 0)
		{
			posY = static_cast<float>(windowHeight - y);
			posX += size;
		}
	}
}

void EasyGL::DrawVerticalStringBetween
(const std::wstring& str, Font& font, const int x, const int y, const int xend, const int yend, const int size, const color RGBA) const
{
	if (!isInit) return;
	float scale = float(size) / font.GetFontSize();

	// 计算可用宽度和高度
	float maxWidth = static_cast<float>(xend - x);
	float maxHeight = static_cast<float>(yend - y);

	float height = 0, temp_height = 0;
	float width = 50;
	for (size_t i = 0; i < str.length(); ++i)
	{
		height += size;
		// 如果超过窗口高度，换列
		if (height > maxHeight)
		{
			temp_height = maxHeight;
			height = 0;
			width += size;
		}
	}
	width = std::min(width, maxWidth);
	height = std::max(height, temp_height);

	float posX = float(x) + (maxWidth - width) / 2;
	float posY = float(y) + (maxHeight - height) / 2;
	posY = windowHeight - posY;
	float nowWidth = 0, nowHeight = 0;
	for (size_t i = 0; i < str.length(); ++i)
	{
		wchar_t c = str[i];

		// 渲染单个字符
		font.RendChar(c, posX, posY - font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));
		//EndRender();
		// 更新位置，向下移动
		posY -= size;
		nowHeight += size;
		// 如果超过最大高度，换列
		if (nowHeight > height)
		{
			posY = float(y) + (maxHeight - height) / 2;
			posY = windowHeight - posY;
			posX += size;
			nowWidth += size;
			nowHeight = 0;
			// 如果超过最大宽度，停止绘制
			if (nowHeight > width)
				break;
		}
	}
}

void EasyGL::DrawVerticalStringBetween
(const std::string& str, Font& font, const int x, const int y, const int xend, const int yend, const int size, const color RGBA) const
{
	if (!isInit) return;
	auto wstr = sth2sth::str2wstr(str);
	float scale = float(size) / font.GetFontSize();

	// 计算可用宽度和高度
	float maxWidth = static_cast<float>(xend - x);
	float maxHeight = static_cast<float>(yend - y);

	float height = 0, temp_height = 0;
	float width = 50;
	for (size_t i = 0; i < wstr.length(); ++i)
	{
		height += size;
		// 如果超过窗口高度，换列
		if (height>maxHeight)
		{
			temp_height = maxHeight;
			height = 0;
			width += size;
		}
	}
	width = std::min(width, maxWidth);
	height = std::max(height, temp_height);

	float posX = float(x) + (maxWidth - width) / 2;
	float posY = float(y) + (maxHeight - height) / 2;
	posY = windowHeight - posY;
	float nowWidth = 0, nowHeight = 0;
	for (size_t i = 0; i < wstr.length(); ++i)
	{
		wchar_t c = wstr[i];

		// 渲染单个字符
		font.RendChar(c, posX, posY - font.GetFontSize(), scale, glm::vec3(RGBA.R, RGBA.G, RGBA.B));
		//EndRender();
		// 更新位置，向下移动
		posY -= size; 
		nowHeight += size;
		// 如果超过最大高度，换列
		if (nowHeight>height)
		{
			posY = float(y) + (maxHeight - height) / 2;
			posY = windowHeight - posY;
			posX += size;
			nowWidth += size;
			nowHeight = 0;
			// 如果超过最大宽度，停止绘制
			if (nowHeight>width)
				break;
		}
	}
}

void EasyGL::DrawBitmap(const float x, const float y, const Bitmap& bitmap, const float angle) const
{
	if (!isInit) return;

	// 默认绘制大小为纹理大小，这里可以根据需要调整
	float width = 100.0f;
	float height = 100.0f;
	DrawBitmap(x, y, width, height, bitmap, angle);
}

void EasyGL::DrawBitmap(const float x, const float y, const float width, const float height, const Bitmap& bitmap, const float angle) const
{
	if (!isInit) return;

	// 将屏幕坐标转换为 OpenGL 坐标（NDC）
	auto screenToNDC = [this](const float sx, const float sy) {
		float ndcX = (sx / windowWidth) * 2.0f - 1.0f;
		float ndcY = 1.0f - (sy / windowHeight) * 2.0f;
		return glm::vec3(ndcX, ndcY, 0.0f);
		};

	// 左上角和右下角坐标
	glm::vec3 topLeft = screenToNDC(x, y);
	glm::vec3 bottomRight = screenToNDC(x + width, y + height);

	// 绘制位图
	bitmap.Draw(topLeft, bottomRight, angle);
}

void EasyGL::DrawVAO(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	if (!isInit) return;

	// 使用指定的着色器
	shader.use();

	// 绑定 VAO 和 IBO
	va.Bind();
	ib.Bind();

	// 绘制操作
	glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);

	// 解绑 VAO 和 IBO
	VertexArray::Unbind();
	IndexBuffer::Unbind();
}

void EasyGL::SetClearColor(const color& color)
{
	clearColor = color;
}

