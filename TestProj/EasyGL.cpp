#include "EasyGL.h"

#include <cmath>
#include <iostream>
//#include <GL/glew.h>
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "Bitmap.h"
#include"OpenGLBase.h"
#include "Shader.h"

color& color::operator=(unsigned int RGBA)
{
	R = (RGBA & 0xff) / 255.0f;
	G = ((RGBA >> 8) & 0xff) / 255.0f;
	B = ((RGBA >> 16) & 0xff) / 255.0f;
	A = ((RGBA >> 24) & 0xff) / 255.0f;
	return *this;
}

color::operator unsigned int() const
{
	return (int(A * 255) << 24) + (int(B * 255) << 16) + (int(G * 255) << 8) + int(R * 255);
}

void FpsCounter::newFrame()
{
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) { // 如果超过1秒
		FPS = nbFrames;
		nbFrames = 0;
		lastTime = currentTime;
	}
}

EasyGL::~EasyGL()
{
	CleanupOpenGL();
	for (auto& i : shaders)
	{
		i.second.~Shader();
	}
}

bool EasyGL::init(const HWND hwnd)
{
	this->hwnd = hwnd;
	PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),
	1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA,
	32,
	0,0,0,0,0,0,
	0,
	0,
	0,
	0,0,0,0,
	24,
	8,
	0,
	PFD_MAIN_PLANE,
	0,
	0,0,0
	};
	hDC = GetDC(hwnd);
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (pixelFormat == 0) return false;

	if (!SetPixelFormat(hDC, pixelFormat, &pfd)) return false;

	hRC = wglCreateContext(hDC);
	//if (glewInit() != GLEW_OK) {
	//	std::cerr << "Failed to initialize GLEW." << std::endl;
	//	return false;
	//}
	if (hRC == NULL) return false;

	if (!wglMakeCurrent(hDC, hRC)) return false;
	initOpenGLAfterWindow();
	std::string vertexShader = R"(
	#version 330 core
	layout(location = 0) in vec4 position;
	void main()
	{
		gl_Position = position;
	}
	)";
	std::string fragmentShader = R"(
	#version 330 core
	layout(location = 0) out vec4 color;
	uniform vec4 u_Color;
		void main()
	{
		color = u_Color;
	}
	)";

	defaultShader.init(vertexShader, fragmentShader);
	isInit= true;
	RECT rect;
	GetClientRect(hwnd, &rect);
	WHRatio = double(rect.right-rect.left) / double(rect.bottom-rect.top);
	return true;
}

void EasyGL::CleanupOpenGL()
{
	if (!isInit)return;
	if (hRC) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		hRC = NULL;
	}
	if (hDC) {
		ReleaseDC(NULL, hDC);
		hDC = NULL;
	}
}

void EasyGL::ChangeSize()
{
	if (!isInit)return;
	CleanupOpenGL();
	init(hwnd);
}

void EasyGL::DrawCircle(float x, float y, float radius, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	for (int i = 0; i < 360; i++)
	{
		float angle = i * 3.1415926 / 180;
		glVertex2f(x + radius * cos(angle), y + radius * sin(angle) *WHRatio);
	}
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawRectangle(float x, float y, float width, float height, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_QUADS);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawLine(float x1, float y1, float x2, float y2, color RGBA)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_TRIANGLES);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawPoint(float x, float y, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_POINTS);
	}
	else
	{
		glBegin(GL_POINT);
	}
	glVertex2f(x, y);
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawEllipse(float x, float y, float a, float b, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	for (int i = 0; i < 360; i++)
	{
		float angle = i * 3.1415926 / 180;
		glVertex2f(x + a * cos(angle), y + b * sin(angle) * WHRatio);
	}
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawPolygon(float x[], float y[], int n, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	for (int i = 0; i < n; i++)
	{
		glVertex2f(x[i], y[i]);
	}
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawArc(float x, float y, float radius, float startAngle, float endAngle, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	for (int i = startAngle; i < endAngle; i++)
	{
		float angle = i * 3.1415926 / 180;
		glVertex2f(x + radius * cos(angle), y + radius * sin(angle) * WHRatio);
	}
	glEnd();
	shaders[usingShaderName].use();
}

void EasyGL::DrawSector(float x, float y, float radius, float startAngle, float endAngle, color RGBA, bool filled)
{
	if (!isInit)return;
	defaultShader.use();
	defaultShader.setVec4("u_Color", glm::vec4(RGBA.R, RGBA.G, RGBA.B, RGBA.A));
	if (filled) {
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	glVertex2f(x, y);
	for (int i = startAngle; i < endAngle; i++)
	{
		float angle = i * 3.1415926 / 180;
		glVertex2f(x + radius * cos(angle), y + radius * sin(angle) * WHRatio);
	}
	glVertex2f(x, y);
	glEnd();
	shaders[usingShaderName].use();
}

//void EasyGL::Draw_text(float x, float y, Font font, const char* text, color RGBA, bool filled)
//{
////TODO:文本绘制
//}
//
void EasyGL::DrawBitmap(float x, float y, Bitmap bitmap, float angle)
{
	bitmap.Draw(glm::vec3(x, y, 0), angle);
}

void EasyGL::DrawBitmap(float x, float y, float width, float height, Bitmap bitmap, float angle)
{
	//TODO:位图绘制2
}

void EasyGL::DrawVAO(unsigned int vao, unsigned int point_count) const
{
	if (!isInit)return;
	// 保存当前的着色器程序和 VAO
	int previousVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);

	// 绑定新的着色器程序和 VAO
	GLCall(glBindVertexArray(vao));

	// 绘制操作
	GLCall(glDrawElements(GL_TRIANGLES, point_count, GL_UNSIGNED_INT, nullptr));

	// 恢复之前的着色器程序和 VAO
	glBindVertexArray(previousVAO);

}

void EasyGL::SetShader(const std::string& name, const Shader& new_shader)
{
	try{
		shaders[name] = new_shader;
	}
	catch (std::exception)
	{
		shaders.insert(std::pair<std::string, Shader>(name, new_shader));
	}
}

Shader& EasyGL::GetShader(const std::string& name)
{
	return shaders[name];
}

inline Shader& EasyGL::GetUsingShader()
{
	return shaders[usingShaderName];
}


void EasyGL::useShader(const std::string& name)
{
	usingShaderName = name;
	shaders[usingShaderName].use();
}

void EasyGL::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void EasyGL::flush() const
{
	SwapBuffers(hDC);
}

void EasyGL::initOpenGLBeforeWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void EasyGL::initOpenGLAfterWindow()
{
	//if (glewInit() != GLEW_OK) {
	//	std::cerr << "Failed to initialize GLEW." << std::endl;
	//}
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD." << std::endl;
	}
}
