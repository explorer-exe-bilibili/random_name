#include "mainloop.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "core/log.h"

#ifdef _WIN32
#include <windows.h>
#endif
#pragma execution_character_set("utf-8")

using namespace std;
GLFWwindow* window;

int init();
int cleanup();

int main()
{
    if(init()!=0){
        Log<<Level::Error<<"Failed to initialize"<<op::endl;
        return -1;
    }
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        mainloop(window);
    }
    cleanup();
    return 0;
}



int init(){
    // 设置控制台代码页为UTF-8
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // 启用VT处理
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    #endif
    Log.Init();
    Log<<Level::Info<<"Initializing GLFW"<<op::endl;
    // 初始化GLFW
    if (!glfwInit()) {
        return -1;
    }
    
    // 设置OpenGL版本和核心模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    Log<<Level::Info<<"Creating window"<<op::endl;
    // 创建窗口
    window = glfwCreateWindow(800, 600, "OpenGL Demo", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // 设置当前上下文
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // 垂直同步

    Log<<Level::Info<<"Loading GLAD"<<op::endl;
    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }
    glfwSetMouseButtonCallback(window, MouseButtonEvent);
    glfwSetKeyCallback(window, KeyEvent);
    // 打印OpenGL版本
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    Log<<Level::Info<<"starting render loop"<<op::endl<<op::flush;
    return 0;
}

int cleanup() {
    Log<<Level::Info<<"Cleaning up"<<op::endl<<op::flush;
    glfwTerminate();

    // 安全停止日志系统
    Log<<Level::Info<<"啊Shutting down logging system"<<op::endl<<op::flush;
    Log.Stop();
    return 0;
}