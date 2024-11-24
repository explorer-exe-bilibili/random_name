////#include<Windows.h>
////#include<gdiplus.h>
////
////#include "Gp.h"
////#include "resource.h"
////using namespace std;
////
////Gp* p = nullptr;
////namespace mywindows {
////	HWND main_hwnd;
////	int WW = 800;
////	int WH = 600;
////}
////void paint() {
////	PAINTSTRUCT ps;
////	HDC hdc = BeginPaint(mywindows::main_hwnd, &ps);
////	if (p!=nullptr)
////	p->RenderOpenGL();
////	EndPaint(mywindows::main_hwnd, &ps);
////	InvalidateRect(mywindows::main_hwnd, NULL, FALSE);
////}
////
////void size() {
////	RECT rect;
////	GetClientRect(mywindows::main_hwnd, &rect);
////	mywindows::WW = rect.right - rect.left;
////	mywindows::WH = rect.bottom - rect.top;
////	if (p == nullptr)return;
////	p->CleanupOpenGL();
////	p->initOpenGL(mywindows::main_hwnd);
////}
////
////void destroy() {
////	PostQuitMessage(0);
////}
////
////LRESULT CALLBACK WinSunProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
////	switch (uMsg) {//通过判断消息进行消息响应
////	case WM_PAINT:paint(); break;
////	case WM_SIZE:size(); break;
////	case WM_DESTROY:destroy(); break;
////	case WM_ERASEBKGND:
////		// 阻止Windows默认擦除背景，返回非零值表示已处理
////		return 1;
////	default:
////		return DefWindowProc(hwnd, uMsg, wParam, lParam);//对不感兴趣的消息进行缺省处理，必须有该代码，否则程序有问题
////	}
////	return 0;
////}
////int WINAPI WinMain(
////	_In_ const HINSTANCE hInstance,      // handle to current instance
////	_In_opt_ HINSTANCE hPrevInstance,  // handle to previous instance
////	_In_ LPSTR lpCmdLine,          // command line
////	_In_ int nCmdShow              )// show state
//// {
////	WNDCLASS wndcls; //创建一个窗体类
////	wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
////	wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
////	wndcls.hbrBackground = HBRUSH(GetStockObject(NULL_BRUSH));//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
////	wndcls.hCursor = LoadCursorW(nullptr, IDC_ARROW);//设置光标
////	wndcls.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_SMALL));//设置窗体左上角的图标
////	wndcls.hInstance = GetModuleHandle(nullptr);;//设置窗体所属的应用程序实例
////	wndcls.lpfnWndProc = WinSunProc;//WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
////	wndcls.lpszClassName = L"main";//设置窗体的类名
////	wndcls.lpszMenuName = nullptr;//设置窗体的菜单,没有，填NULL
////	wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
////	RegisterClass(&wndcls);//向操作系统注册窗体
////	mywindows::main_hwnd = CreateWindowW(L"main",L"test", WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_THICKFRAME, 0, 0, mywindows::WW, mywindows::WH, NULL, NULL, GetModuleHandle(NULL), NULL);
////	ShowWindow(mywindows::main_hwnd, SW_SHOW);
////	p = new Gp(mywindows::main_hwnd);
////	MSG msg;
////	while (GetMessageW(&msg, nullptr, NULL, NULL)) {
////		TranslateMessage(&msg);
////		DispatchMessageW(&msg);
////	}
////	return msg.wParam;
////}
///
///

#include <glad/glad.h>
#include <iostream>
#include <Windows.h>

#include<string>
#include <thread>

#include "Bitmap.h"
#include "EasyGL.h"
#include "Shader.h"
#include "VertexBuffer.h"

using namespace std;


int main()
{
	EasyGL::initOpenGLBeforeWindow();
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
	EasyGL::initOpenGLAfterWindow();
	gladLoadGL();
	//Bitmap::init();
	EasyGL easyGL;
	easyGL.init(hwnd);
	cout << glGetString(GL_VERSION) << endl;
	//// 创建着色器
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
	easyGL.SetShader("default", Shader(vertexShader, fragmentShader));
	easyGL.useShader("default");
	//// 渲染
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


	//VertexArray va;
	//VertexBuffer vb(position, sizeof(float) * 2 * 4);
	//IndexBuffer ib(indices, 6);
	//ib.BindVertexBuffer("position", vb);
	//ib.useVertexBuffer("position");
	//va.AddBuffer(vb, ib);
	//va.unbind();
	Bitmap bitmap;
	bitmap.LoadFromFile("test.jpg");
	//VertexBuffer::enableIndexBuffer(2, 2, GL_FLOAT, 2 * sizeof(float), 0);

	FpsCounter fpsCounter;
	
	MSG msg;
	float add_r = 0.005f;
	float r = 0.8f;
	while (GetMessageW(&msg, nullptr, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		EasyGL::clear();
		//easyGL.GetShader("default").setVec4("u_Color", glm::vec4(r, 0.3, 1-r, 1));
		//easyGL.DrawVAO(va.getRendererID(),6);
		easyGL.DrawCircle(0, 0, 0.5, color(0, r, 1, 1), 0);
		bitmap.Draw(glm::vec3(-0.5, -0.5, 0));
		easyGL.DrawRectangle(0, 0, 0.5, 0.5, color(1, 0, 0, 1), 1);
		easyGL.flush();
		fpsCounter.newFrame();
		if (fpsCounter.getFps() > 0)
			cout <<"fps:"<< fpsCounter.getFps() << endl;
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
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}




//// C++
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <stb_image.h>
//#include <iostream>
//
//// 引入GLM头文件
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "Bitmap.h"
//
//// 顶点着色器源码
//const char* vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;
//
//uniform mat4 transform;
//
//out vec2 TexCoord;
//
//void main()
//{
//    gl_Position = transform * vec4(aPos, 1.0);
//    TexCoord = aTexCoord;
//}
//)";
//
//// 片段着色器源码
//const char* fragmentShaderSource = R"(
//#version 330 core
//out vec4 FragColor;
//
//in vec2 TexCoord;
//
//// 纹理采样器
//uniform sampler2D texture1;
//
//void main()
//{
//    FragColor = texture(texture1, TexCoord);
//}
//)";
//
//int main() {
//    // 初始化GLFW
//    if (!glfwInit()) {
//        std::cerr << "Failed to initialize GLFW" << std::endl;
//        return -1;
//    }
//
//    // 设置OpenGL版本 (3.3 Core)
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // 创建窗口
//    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Image with Position", nullptr, nullptr);
//    if (!window) {
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//
//    // 初始化GLAD
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        return -1;
//    }
//
//    // 设置视口
//    glViewport(0, 0, 800, 600);
//    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
//        glViewport(0, 0, width, height);
//        });
//	Bitmap::init();
//	Bitmap bitmap;
//	bitmap.LoadFromFile("test.jpg");
//	Bitmap bitmap2;
//	bitmap2.LoadFromFile("test.jpg");
//
//    // 帧率计算变量
//    double lastTime = glfwGetTime();
//    int nbFrames = 0;
//
//    // 位置变量
//    float x = 0.0f;
//    float y = 0.0f;
//
//    // 上一帧时间
//    double previousTime = glfwGetTime();
//    // 渲染循环
//// 在渲染循环中添加旋转
//    while (!glfwWindowShouldClose(window)) {
//        // 帧率计算
//        double currentTime = glfwGetTime();
//        nbFrames++;
//        double deltaTime = currentTime - previousTime;
//        previousTime = currentTime;
//        if (currentTime - lastTime >= 1.0) { // 如果超过1秒
//            std::cout << "FPS: " << nbFrames << std::endl;
//            nbFrames = 0;
//            lastTime += 1.0;
//            x += 0.01;
//			y += 0.01;
//        }
//
//        // 处理输入
//        //glProcessInput(window, x, y, static_cast<float>(deltaTime));
//
//        // 清空颜色缓冲
//        glClear(GL_COLOR_BUFFER_BIT);
//        bitmap.Draw(glm::vec3(x, y, 0.0f), 0);
//        bitmap2.Draw(glm::vec3(1 - x, 1 - y, 0));
//
//        // 交换缓冲区和查询IO事件
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//}