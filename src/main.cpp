#include "mainloop.h"
#include <SDL.h>
#include <stdio.h>
#include "core/screen/mainScreen.h"
#include "core/screen/SettingScreen.h"
#include "core/screen/VideoScreen.h"
#include "core/screen/nameScreen.h"
#include "core/decrash/ErrorRecovery.h"
#include "core/decrash/OpenGLErrorRecovery.h"
#include "core/decrash/MemoryMonitor.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using namespace core;
using namespace std;

// 当使用SDL2时，main函数需要被重新定义为SDL_main
int main(int argc, char* argv[])
{
    // 注册错误处理器
    ErrorRecovery::registerErrorHandler(
        ErrorRecovery::ErrorType::MEMORY_ALLOCATION,
        []() {
            Log << Level::Warn << "内存分配失败，执行垃圾回收" << op::endl;
            MemoryMonitor::getInstance().forceGarbageCollection();
        }
    );
    
    ErrorRecovery::registerErrorHandler(
        ErrorRecovery::ErrorType::OPENGL_CONTEXT,
        []() {
            Log << Level::Warn << "OpenGL 错误，尝试重新初始化上下文" << op::endl;
            OpenGLErrorRecovery::markContextInvalid();
        }
    );
    
    ErrorRecovery::registerErrorHandler(
        ErrorRecovery::ErrorType::FFMPEG_DECODE,
        []() {
            Log << Level::Warn << "FFmpeg 解码错误，清理视频资源" << op::endl;
            MemoryMonitor::getInstance().forceGarbageCollection();
        }
    );

#ifdef _WIN32
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
#ifdef DEBUG_MODE
    Log<<Level::Info<<"运行在Debug模式"<<op::endl;
    // _wchdir(L"..");
#else
    Log<<Level::Info<<"运行在非Debug模式"<<op::endl;
#endif

    try {
        // 使用错误恢复执行初始化
        int result = ErrorRecovery::executeWithRetry<int>(
            []() { return init(); },
            {.maxRetries = 3, .baseDelay = std::chrono::milliseconds(1000)}
        );
        
        if(result != 0){
            Log<<Level::Error<<"Failed to initialize"<<op::endl;
            return -1;
        }
        
        // 初始化FPS计算
        lastFPSUpdateTime = glfwGetTime();
        frameCount = 0;
        currentFPS = 0.0;
        
        // 渲染循环 - 现在包含错误恢复
        while (!glfwWindowShouldClose(core::WindowInfo.window)) {
            if (ErrorRecovery::shouldAbort()) {
                Log << Level::Error << "连续错误过多，程序退出" << op::endl;
                break;
            }
            
            try {
                mainloop();
            } catch (const std::exception& e) {
                Log << Level::Error << "主循环异常: " << e.what() << op::endl;
                
                // 尝试恢复OpenGL上下文
                if (!OpenGLErrorRecovery::recoverLostContext(core::WindowInfo.window)) {
                    Log << Level::Error << "无法恢复OpenGL上下文，程序退出" << op::endl;
                    break;
                }
            }
        }
        
    } catch (const std::exception& e) {
        Log << Level::Error << "程序发生致命错误: " << e.what() << op::endl;
        return -1;
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
    // WSL 特定的 GLFW 提示
    #ifdef __linux__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // WSL 兼容性设置
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    
    // 减少显存使用
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
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
    glfwSwapInterval(0); // 垂直同步    Log<<Level::Info<<"Loading GLAD"<<op::endl;
    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }
    
    // 验证 OpenGL 上下文
    Log << Level::Info << "OpenGL Version: " << (const char*)glGetString(GL_VERSION) << op::endl;
    Log << Level::Info << "OpenGL Renderer: " << (const char*)glGetString(GL_RENDERER) << op::endl;

    // 初始化 OpenGL 错误恢复系统
    OpenGLErrorRecovery::reset();
    
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

    core::Explorer::getInstance()->playAudio(AudioID::bgm, -1);
    core::Explorer::getInstance()->getAudio()->setMusicVolume(50);
    screen::Screen::RegisterScreen(screen::ScreenID::MainMenu, std::make_shared<screen::MainScreen>());
    screen::Screen::SwitchToScreen(screen::ScreenID::MainMenu);
    screen::Screen::RegisterScreen(screen::ScreenID::Settings, std::make_shared<screen::SettingScreen>());
    screen::Screen::RegisterScreen(screen::ScreenID::Video,std::make_shared<screen::VideoScreen>());
    screen::Screen::RegisterScreen(screen::ScreenID::Name,std::make_shared<screen::NameScreen>());
    Log<<Level::Info<<"starting render loop"<<op::endl<<op::flush;
    return 0;
}

int cleanup() {
    Log<<Level::Info<<"Cleaning up"<<op::endl<<op::flush;
    
    // 清理其他资源
    core::Explorer::getInstance2().reset();

    // 标记OpenGL上下文即将失效，防止后续OpenGL调用引起问题
    SetOpenGLContextInvalid();
    core::OpenGLErrorRecovery::markContextInvalid();
    
    // 确保在终止GLFW前解绑当前上下文
    Log<<Level::Info<<"Unbinding OpenGL context"<<op::endl<<op::flush;
    glfwMakeContextCurrent(nullptr);
    
    // 然后再终止GLFW
    Log<<Level::Info<<"Terminating GLFW"<<op::endl<<op::flush;
    // 终止GLFW
    glfwTerminate();
    
    // SDL会在程序退出时自动清理，但显式调用可确保顺序
    SDL_Quit();

    // 安全停止日志系统
    Log<<Level::Info<<"Shutting down logging system"<<op::endl<<op::flush;
    Log.Stop();
    
    return 0;
}