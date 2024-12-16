// TestProj.cpp

#include <Windows.h>
#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>

#include "EasyGL.h"
#include "Bitmap.h"
#include "My_Font.h"

// 全局变量
EasyGL easyGL;
Bitmap bitmap;
VertexArray* va;
VertexBuffer* vb;
IndexBuffer* ib;
Shader shader;
Font* font;

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	static int height=0;
	static int width=0;
	switch (uMsg)
	{
	case WM_CREATE:
		{
			// 初始化 OpenGL
			if (!easyGL.Init(hwnd))
			{
				std::cerr << "OpenGL 初始化失败！" << std::endl;
				PostQuitMessage(0);
			}

			// 初始化 Bitmap
			Bitmap::init();
			if (!bitmap.LoadFromFile("test.png"))
			{
				std::cerr << "加载图片失败！" << std::endl;
				PostQuitMessage(0);
			}
			// 定义顶点和索引数据（位置和颜色）
			float vertices[] = {
				// 位置           // 颜色
				-0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // 左上（红色）
				 0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // 右上（绿色）
				 0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // 右下（蓝色）
				-0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f   // 左下（黄色）
			};
			unsigned int indices[] = {
				0, 1, 2,
				2, 3, 0
			};
			va = new VertexArray;
			// 创建并绑定 VAO
			va->Bind();

			// 创建 VertexBuffer 并填充数据
			vb = new VertexBuffer(vertices, sizeof(vertices));

			// 创建 IndexBuffer 并填充数据
			ib = new IndexBuffer(indices, sizeof(indices) / sizeof(unsigned int));

			// 设置顶点属性
			va->AddBuffer(*vb, 0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);                   // 位置属性
			va->AddBuffer(*vb, 1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float))); // 颜色属性

			// 解绑 VAO
			VertexArray::Unbind();

			// 创建并编译着色器
			std::string vertexShaderSource = R"(
	#version 330 core
	layout(location = 0) in vec3 aPos;    // 位置
	layout(location = 1) in vec3 aColor;  // 颜色

	out vec3 ourColor; // 传递给片段着色器

	void main()
	{
		gl_Position = vec4(aPos, 1.0);
		ourColor = aColor;
	}
	)";

			std::string fragmentShaderSource = R"(
	#version 330 core
	in vec3 ourColor; // 来自顶点着色器的颜色

	out vec4 FragColor;

	void main()
	{
		FragColor = vec4(ourColor, 1.0);
	}
	)";
			shader = Shader(vertexShaderSource,fragmentShaderSource );
			easyGL.SetShader("default", shader);
			easyGL.SetClearColor(color(0.2, 0.3, 0.2, 1.0));
			font = new Font("D:/Users/explorer/Source/Repos/random_name/files/ttfs/QS.ttf",0);
	}
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);

		easyGL.BeginRender();
		easyGL.DrawLine(200, 200, 1000, 200, color(1, 0, 1)); // 绘制线段
		easyGL.DrawLine(200, 250, 1000, 250, color(1, 0, 1)); // 绘制线段
		easyGL.DrawRectangle(200, 200, 500, 500, color(1.0, 0.0, 0.0), 0);
		easyGL.DrawString("hello啊？", *font, 200, 200, 50, color(123, 245, 235));
		easyGL.DrawVerticalString("hello啊？", *font, 200, 200, 50, color(123, 245, 235));
		font->RenderText(L"hello啊？", 200, 200, 1, glm::vec3(1.0, 1.0, 1.0));
		font->RendChar(L'a', 0, 0, 1, glm::vec3(0, 0, 1.0));
		easyGL.EndRender();

		EndPaint(hwnd, &ps);
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	}

	case WM_SIZE: {
		// 处理窗口大小调整
		RECT rect;
		GetClientRect(hwnd, &rect);

		height = rect.bottom - rect.top;
		width = rect.right - rect.left;
		easyGL.Resize(width, height);
		font->resize(width, height);
	}
		return 0;

	case WM_DESTROY:
		easyGL.Cleanup(); // 清理 OpenGL 资源
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

int main()
{

	//设置OpenGL版本为3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 注册窗口类
	WNDCLASS wc = {};
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"OpenGLWindowClass";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	// 创建窗口
	HWND hwnd = CreateWindowEx(
		0,
		L"OpenGLWindowClass",
		L"OpenGL Rendering",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	if (hwnd == NULL)
	{
		std::cerr << "创建窗口失败！" << std::endl;
		return -1;
	}

	ShowWindow(hwnd, SW_SHOW);

	// 消息循环
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}