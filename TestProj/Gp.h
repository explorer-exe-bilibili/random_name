#pragma once
#include <chrono>
#include<Windows.h>
#include <glm/vec3.hpp>
#include <string>
#include"Shader.h"
#include <GL/gl.h>

class My_Font;

class Gp
{

	HDC hDC;
	HGLRC hRC = NULL;
	void RenderText(std::wstring text, float x, float y, float scale, glm::vec3 color);
	// FPS 相关变量
	int frameCount = 0;
	int lastCount = 0;
	double fps = 0.0;
	std::chrono::time_point<std::chrono::steady_clock> lastTime;
	GLuint base = 0;
	GLuint shaderProgram;
	GLuint VBO, VAO;
	Shader* textShader; // 添加这一行
	void initFont();
	void renderBitmapString(float x, float y, void* font, std::string text);
public:
	~Gp();
	Gp(HWND hwnd);
	bool initOpenGL(HWND hwnd);
	void RenderOpenGL();
	void CleanupOpenGL();
	void InitializeTextRendering();
	void renderBitmapString(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint screenWidth, GLuint screenHeight);
};

