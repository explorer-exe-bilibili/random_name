//#include<Windows.h>
//#include<gdiplus.h>
//
//#include "Gp.h"
//#include "resource.h"
//using namespace std;
//
//Gp* p = nullptr;
//namespace mywindows {
//	HWND main_hwnd;
//	int WW = 800;
//	int WH = 600;
//}
//void paint() {
//	PAINTSTRUCT ps;
//	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
//	if (p!=nullptr)
//	p->RenderOpenGL();
//	EndPaint(mywindows::main_hwnd, &ps);
//	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
//}
//
//void size() {
//	RECT rect;
//	GetClientRect(mywindows::main_hwnd, &rect);
//	mywindows::WW = rect.right - rect.left;
//	mywindows::WH = rect.bottom - rect.top;
//	if (p == nullptr)return;
//	p->CleanupOpenGL();
//	p->initOpenGL(mywindows::main_hwnd);
//}
//
//void destroy() {
//	PostQuitMessage(0);
//}
//
//LRESULT CALLBACK WinSunProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
//	switch (uMsg) {//通过判断消息进行消息响应
//	case WM_PAINT:paint(); break;
//	case WM_SIZE:size(); break;
//	case WM_DESTROY:destroy(); break;
//	case WM_ERASEBKGND:
//		// 阻止Windows默认擦除背景，返回非零值表示已处理
//		return 1;
//	default:
//		return DefWindowProc(hwnd, uMsg, wParam, lParam);//对不感兴趣的消息进行缺省处理，必须有该代码，否则程序有问题
//	}
//	return 0;
//}
//int WINAPI WinMain(
//	_In_ const HINSTANCE hInstance,      // handle to current instance
//	_In_opt_ HINSTANCE hPrevInstance,  // handle to previous instance
//	_In_ LPSTR lpCmdLine,          // command line
//	_In_ int nCmdShow              )// show state
// {
//	WNDCLASS wndcls; //创建一个窗体类
//	wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
//	wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
//	wndcls.hbrBackground = HBRUSH(GetStockObject(NULL_BRUSH));//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
//	wndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//设置光标
//	wndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_SMALL));//设置窗体左上角的图标
//	wndcls.hInstance = GetModuleHandle(nullptr);;//设置窗体所属的应用程序实例
//	wndcls.lpfnWndProc = WinSunProc;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
//	wndcls.lpszClassName = L"main";//设置窗体的类名
//	wndcls.lpszMenuName = nullptr;//设置窗体的菜单,没有，填NULL
//	wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
//	RegisterClass(&wndcls);//向操作系统注册窗体
//	mywindows::main_hwnd = CreateWindowW(L"main",L"test", WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_THICKFRAME, 0, 0, mywindows::WW, mywindows::WH, NULL, NULL, GetModuleHandle(NULL), NULL);
//	ShowWindow(mywindows::main_hwnd, SW_SHOW);
//	p = new Gp(mywindows::main_hwnd);
//	MSG msg;
//	while (GetMessageW(&msg, nullptr, NULL, NULL)) {
//		TranslateMessage(&msg);
//		DispatchMessageW(&msg);
//	}
//	return msg.wParam;
//}
#include <iostream>
#include <Windows.h>
#include <GL/glew.h>

#include<string>
#include <thread>

using namespace std;

#define ASSERT(x) if (!(x)) __debugbreak()

#define GLCall(a) GLClearError();\
	a;\
	GLCheckError();\
	cout<<#a<<__FILE__<<__LINE__<<endl;

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError() {
	while (GLenum error = glGetError()) {
		cout << "[OpenGL Error] (" << error << ")" << endl;
		__debugbreak();
	}
}

static unsigned int CompileShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}


int main()
{
	// 初始化窗口
	WNDCLASS wc = {};
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"Test";
	RegisterClass(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, L"Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 800, 600, 0, 0, GetModuleHandle(NULL), 0);
	// 初始化OpenGL
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
	HDC hdc = GetDC(hwnd);
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
	HGLRC hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);
	// 初始化GLEW
	glewInit();

	// 创建着色器
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
	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);
	// 渲染
	float position[] = {
		-0.5f,-0.5f,
		-0.5f,0.5f,
		0.5f,-0.5f,
		0.5f,0.5f,
	};
	unsigned int indices[] = {
		0,1,2,
		1,2,3
	};
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4, position, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int ibo;//索引缓冲对象
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	int colorLocation = glGetUniformLocation(shader, "u_Color");
	ASSERT(colorLocation != -1);
	glUniform4f(colorLocation, 0.2f, 0.3f, 0.8f, 1.0f);
	MSG msg;
	float add_r = 0.5;
	float r = 0.8f;
	while (GetMessageW(&msg, nullptr, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		glClear(GL_COLOR_BUFFER_BIT);
		GLCall(glUniform4f(colorLocation, r, 0.3f, 0.5f, 1.0f));
		GLClearError();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		GLCheckError();
		SwapBuffers(hdc);
		if (r > 1.0f)
		{
			add_r = -0.05f;
		}
		else if (r<0)
		{
			add_r = 0.05f;
		}
		r += add_r;
		cout << r << endl;
		InvalidateRect(hwnd, NULL, false);
		Sleep(1);
	}
	glDeleteProgram(shader);
}
