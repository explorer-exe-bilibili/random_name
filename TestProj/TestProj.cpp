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
	float vertices[] = {
		// 位置          // 颜色
		 0.5f,  0.5f,    1.0f, 0.0f, 0.0f,  // 右上
		 0.5f, -0.5f,    0.0f, 1.0f, 0.0f,  // 右下
		-0.5f, -0.5f,    0.0f, 0.0f, 1.0f,  // 左下
		-0.5f,  0.5f,    1.0f, 1.0f, 0.0f   // 左上
	};
	unsigned int indices[] = {
		0,1,2,
		1,2,3
	};

	VertexArray va;
	VertexBuffer vb(vertices, sizeof(vertices));
	IndexBuffer ib(indices, 6);

	va.Bind();

	// 位置属性
	va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
	// 颜色属性
	va.AddBuffer(vb, 1, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	ib.Bind();
	Bitmap::init();
	Bitmap bitmap;
	bitmap.LoadFromFile("test.jpg");

	FpsCounter fpsCounter;
	
	MSG msg;
	float add_r = 0.005f;
	float r = 0.8f;
	while (GetMessageW(&msg, nullptr, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		EasyGL::clear();
		//easyGL.GetShader("default").setVec4("u_Color", glm::vec4(r, 0.3, 1-r, 1));
		// 渲染循环中
		va.Bind();
		ib.Bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
		easyGL.DrawCircle(0, 0, 0.5, color(0, r, 1, 1), 0);
		bitmap.Draw(glm::vec3(0,0, 0),glm::vec3(1,1,0),45);
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
//// 处理键盘输入，调整x和y的位置
//void processInput(GLFWwindow* window, float& x, float& y, float deltaTime)
//{
//    float speed = 0.5f * deltaTime; // 移动速度
//
//    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
//        y += speed;
//    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
//        y -= speed;
//    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
//        x -= speed;
//    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
//        x += speed;
//}
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
//    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Image with FPS and Movement", nullptr, nullptr);
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
//
//    // 编译顶点着色器
//    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//    glCompileShader(vertexShader);
//    // 检查编译错误
//    int success;
//    char infoLog[512];
//    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//        std::cerr << "顶点着色器编译失败:\n" << infoLog << std::endl;
//    }
//
//    // 编译片段着色器
//    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//    glCompileShader(fragmentShader);
//    // 检查编译错误
//    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//        std::cerr << "片段着色器编译失败:\n" << infoLog << std::endl;
//    }
//
//    // 链接着色器程序
//    GLuint shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//    // 检查链接错误
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//    if (!success) {
//        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//        std::cerr << "着色器程序链接失败:\n" << infoLog << std::endl;
//    }
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//
//    // 设置顶点数据和缓冲，并配置顶点属性
//    float vertices[] = {
//        // 位置          // 纹理坐标
//         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 右上
//         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 右下
//        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 左下
//        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // 左上
//    };
//    unsigned int indices[] = {
//        0, 1, 3, // 第一三角形
//        1, 2, 3  // 第二三角形
//    };
//    GLuint VBO, VAO, EBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//
//    glBindVertexArray(VAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    // 位置属性
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    // 纹理坐标属性
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//
//    // 加载并创建纹理
//    GLuint texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    // 设置纹理参数
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    // 加载图片
//    int width, height, nrChannels;
//    stbi_set_flip_vertically_on_load(true); // 翻转图片
//    unsigned char* data = stbi_load("test.jpg", &width, &height, &nrChannels, 0);
//    if (data) {
//        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//    }
//    else {
//        std::cerr << "Failed to load texture" << std::endl;
//    }
//    stbi_image_free(data);
//
//    // 使用着色器程序
//    glUseProgram(shaderProgram);
//    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
//
//    // 获取变换矩阵的位置
//    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
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
//
//    // 渲染循环
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
//        }
//
//        // 处理输入
//        processInput(window, x, y, static_cast<float>(deltaTime));
//
//        // 清空颜色缓冲
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        // 创建平移矩阵
//        glm::mat4 transform = glm::mat4(1.0f); // 初始化为单位矩阵
//        transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
//
//        // 传递矩阵到着色器
//        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
//
//        // 绑定纹理
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture);
//
//        // 绘制矩形
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//        // 交换缓冲区和查询IO事件
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // 释放资源
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
//    glDeleteProgram(shaderProgram);
//
//    glfwTerminate();
//    return 0;
//}