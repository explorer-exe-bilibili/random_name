#include "mainloop.h"

#include "core/log.h"
#include "core/explorer.h"
#include "core/render/GLBase.h"
#include "core/baseItem/Base.h"
#include "core/baseItem/Button.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace core;

Button* test_button = nullptr;

// 窗口大小改变时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // 更新screenInfo的相关变量
    screenInfo.width = width;
    screenInfo.height = height;
    screenInfo.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    // 设置OpenGL视口大小
    GLCall(glViewport(0, 0, width, height));
    
    Log << Level::Info << "Window resized to " << width << "x" << height << op::endl;
}

int mainloop() {
    GLCall(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
    
    // 清除颜色和深度缓冲区
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // 设置视口和投影矩阵（如果需要）
    int width, height;
    glfwGetFramebufferSize(screenInfo.window, &width, &height);
    GLCall(glViewport(0, 0, width, height));

    test_button->Draw();
    // 交换前后缓冲区
    glfwSwapBuffers(screenInfo.window);

    // 处理事件
    glfwPollEvents();

    return 0;
}

void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            Log << Level::Info << "Key pressed: " << key << op::endl;
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Key released: " << key << op::endl;
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;
                default:
                    break;
            }
            break;
        case GLFW_REPEAT:
            Log << Level::Info << "Key repeated: " << key << op::endl;
            break;
        default:
            break;
    }
}

void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    switch (action) {
        case GLFW_PRESS:
            Log << Level::Info << "Mouse button pressed at (" << x << ", " << y << "): " << button << op::endl;
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Mouse button released at (" << x << ", " << y << "): " << button << op::endl;
            break;
        default:
            break;
    }
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
    screenInfo.window = glfwCreateWindow(800, 600, "OpenGL Demo", nullptr, nullptr);
    if (!screenInfo.window) {
        glfwTerminate();
        return -1;
    }
    
    // 设置当前上下文
    glfwMakeContextCurrent(screenInfo.window);
    glfwSwapInterval(0); // 垂直同步

    Log<<Level::Info<<"Loading GLAD"<<op::endl;
    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }
    glfwSetMouseButtonCallback(screenInfo.window, MouseButtonEvent);
    glfwSetKeyCallback(screenInfo.window, KeyEvent);
    glfwSetFramebufferSizeCallback(screenInfo.window, framebuffer_size_callback);
    // 打印OpenGL版本
    {
        const GLubyte* version = glGetString(GL_VERSION);
        std::cout << "OpenGL Version: " << version << std::endl;
    }
    core::screenInfo.width = 800;
    core::screenInfo.height = 600;

    Log<<Level::Info<<"Init explorer"<<op::endl;
    core::Explorer::getInstance();
    test_button = new Button("Test Button", 0, Region(0,0,800,600));
    Log<<Level::Info<<"starting render loop"<<op::endl<<op::flush;
    return 0;
}

int cleanup() {
    Log<<Level::Info<<"Cleaning up"<<op::endl<<op::flush;
    
    // 先释放Explorer实例以确保所有OpenGL资源被正确释放
    Log<<Level::Info<<"Releasing Explorer instance"<<op::endl<<op::flush;
    core::Explorer::getInstance().reset();
    delete test_button;
    test_button = nullptr;
    
    // 标记OpenGL上下文即将失效，防止后续OpenGL调用引起问题
    SetOpenGLContextInvalid();
    
    // 确保在终止GLFW前解绑当前上下文
    Log<<Level::Info<<"Unbinding OpenGL context"<<op::endl<<op::flush;
    glfwMakeContextCurrent(nullptr);
    
    // 然后再终止GLFW
    Log<<Level::Info<<"Terminating GLFW"<<op::endl<<op::flush;
    glfwTerminate();

    // 安全停止日志系统
    Log<<Level::Info<<"Shutting down logging system"<<op::endl<<op::flush;
    Log.Stop();
    
    return 0;
}