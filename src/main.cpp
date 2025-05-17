#include "mainloop.h"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace core;
using namespace std;

int main()
{
#ifdef DEBUG_MODE
    Log<<Level::Info<<"运行在Debug模式"<<op::endl;
#else
    Log<<Level::Info<<"运行在非Debug模式"<<op::endl;
#endif
    if(init()!=0){
        Log<<Level::Error<<"Failed to initialize"<<op::endl;
        return -1;
    }
    
    // 渲染循环
    while (!glfwWindowShouldClose(core::WindowInfo.window)) {
        mainloop();
    }
    
    // 执行清理操作并确保不再进行后续的OpenGL调用
    int cleanupResult = cleanup();
    if (cleanupResult != 0) {
        Log<<Level::Error<<"Cleanup failed with code: "<<cleanupResult<<op::endl;
    }
    
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
    }    Config::getInstance()->init();
    SetConfigItems();
    // 设置OpenGL版本和兼容性模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    // 启用向前兼容性
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    Log<<Level::Info<<"Creating window"<<op::endl;
    // 创建窗口
    WindowInfo.window = glfwCreateWindow(800, 600, "OpenGL Demo", nullptr, nullptr);
    if (!WindowInfo.window) {
        glfwTerminate();
        return -1;
    }
    
    // 设置当前上下文
    glfwMakeContextCurrent(WindowInfo.window);
    glfwSwapInterval(0); // 垂直同步

    Log<<Level::Info<<"Loading GLAD"<<op::endl;
    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }
    glfwSetMouseButtonCallback(WindowInfo.window, MouseButtonEvent);
    glfwSetKeyCallback(WindowInfo.window, KeyEvent);
    glfwSetFramebufferSizeCallback(WindowInfo.window, framebuffer_size_callback);
    // 打印OpenGL版本
    {
        const GLubyte* version = glGetString(GL_VERSION);
        std::cout << "OpenGL Version: " << version << std::endl;
    }
    core::WindowInfo.width = 800;
    core::WindowInfo.height = 600;

    Log<<Level::Info<<"Init explorer"<<op::endl;
    core::Explorer::getInstance();

    currentScreen = new screen::MainScreen();

    Log<<Level::Info<<"starting render loop"<<op::endl<<op::flush;
    return 0;
}

int cleanup() {
    Log<<Level::Info<<"Cleaning up"<<op::endl<<op::flush;
    
    // 先释放Explorer实例以确保所有OpenGL资源被正确释放
    Log<<Level::Info<<"Releasing Explorer instance"<<op::endl<<op::flush;
    core::Explorer::getInstance().reset();
    
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