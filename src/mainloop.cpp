#include "mainloop.h"

#include "core/log.h"
#include "core/explorer.h"
#include "core/render/GLBase.h"

#ifdef _WIN32
#include <windows.h>
#endif
GLFWwindow* window;


int mainloop(GLFWwindow* window) {
    GLCall(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
    
    // 清除颜色和深度缓冲区
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // 设置视口和投影矩阵（如果需要）
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    GLCall(glViewport(0, 0, width, height));
    
    // 尝试绘制位图
    auto& bitmap = core::Explorer::getInstance()->getBitmap("1");
    
    // 输出调试信息，确认位图状态
    Log << Level::Info << "Drawing bitmap: size=" << bitmap.getWidth() << "x" << bitmap.getHeight() << op::endl;
    
    // 绘制位图
    bitmap.Draw(core::Region(0, 0, width, height));
    
    // 交换前后缓冲区
    GLCall(glfwSwapBuffers(window));
    
    // 处理事件
    GLCall(glfwPollEvents());
    
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
    {
        const GLubyte* version = glGetString(GL_VERSION);
        std::cout << "OpenGL Version: " << version << std::endl;
    }
    core::screenInfo.width = 800;
    core::screenInfo.height = 600;

    Log<<Level::Info<<"Init explorer"<<op::endl;
    core::Explorer::getInstance();
    Log<<Level::Info<<"starting render loop"<<op::endl<<op::flush;
    return 0;
}

int cleanup() {
    Log<<Level::Info<<"Cleaning up"<<op::endl<<op::flush;
    GLFWwindow* w = window; // preserve
    GLCall(glfwTerminate());

    // 安全停止日志系统
    Log<<Level::Info<<"Shutting down logging system"<<op::endl<<op::flush;
    Log.Stop();
    return 0;
}