#include "mainloop.h"
#include "version.h"
#include <SDL.h>
#include <stdio.h>
#include <filesystem>
#include "core/screen/mainScreen.h"
#include "core/screen/SettingScreen.h"
#include "core/screen/VideoScreen.h"
#include "core/screen/nameScreen.h"
#include "core/screen/ListNameScreen.h"
#include <tinyfiledialogs.h>
#include "core/render/OpenGLFontRenderer.h"
#include "core/baseItem/Font.h"

#ifdef _WIN32
#undef APIENTRY
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <shlobj.h>  // 用于获取用户目录
#endif

using namespace core;
using namespace std;

// 获取程序所在目录的安全函数
std::filesystem::path getExecutableDirectory() {
#ifdef _WIN32
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (length > 0) {
        std::filesystem::path execPath(path);
        return execPath.parent_path();
    }
#endif
    return std::filesystem::current_path();
}

// 获取用户数据目录
std::filesystem::path getUserDataDirectory() {
#ifdef _WIN32
    wchar_t* documentsPath = nullptr;
    if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &documentsPath) == S_OK) {
        std::filesystem::path userDir(documentsPath);
        userDir = userDir / "RandomNameApp";
        CoTaskMemFree(documentsPath);
        return userDir;
    }
#endif
    return std::filesystem::current_path();
}

// 检查目录是否可写
bool isDirectoryWritable(const std::filesystem::path& dir) {
    try {
        std::filesystem::path testFile = dir / "test_write_permission.tmp";
        std::ofstream test(testFile);
        if (test.is_open()) {
            test << "test";
            test.close();
            std::filesystem::remove(testFile);
            return true;
        }
    } catch (...) {
        // 忽略异常
    }
    return false;
}

// 当使用SDL2时，main函数需要被重新定义为SDL_main
int main(int argc, char* argv[])
{
    if(init() != 0){
        Log<<Level::Error<<"Failed to initialize"<<op::endl;
        return -1;
    }
        
    // 初始化FPS计算
    lastFPSUpdateTime = glfwGetTime();
    frameCount = 0;
    currentFPS = 0.0;
        
    // 渲染循环 - 现在包含错误恢复
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
#ifdef _WIN32
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 初始化目录状态
    core::initializeDirectories();
    
    // 设置工作目录为程序所在目录（确保资源文件能正常加载）
    try {
        std::filesystem::current_path(core::g_executableDir);
        Log << Level::Info << "设置工作目录为程序目录: " << core::g_executableDir.string() << op::endl;
    } catch (const std::exception& e) {
        Log << Level::Warn << "无法设置工作目录: " << e.what() << op::endl;
    }
    
    if (core::g_programDirWritable) {
        Log << Level::Info << "程序目录可写" << op::endl;
    } else {
        Log << Level::Info << "程序目录不可写，配置和日志将使用用户目录: " << core::g_userDataDir.string() << op::endl;
        
        // 确保用户数据目录存在
        try {
            std::filesystem::create_directories(core::g_userDataDir);
        } catch (const std::exception& e) {
            Log << Level::Error << "无法创建用户数据目录: " << e.what() << op::endl;
        }
    }
#else
    // 非Windows平台弹出未经测试的提示
    tinyfd_messageBox("警告", "此程序在非Windows平台上运行，可能会遇到未测试的情况。\nThis Program may not work as expected as no tests have been conducted.", "ok", "warning", 1);
#endif

#ifdef _WIN32
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
#ifdef DEBUG_MODE
    Log<<Level::Info<<"运行在Debug模式"<<op::endl;
#else
    Log<<Level::Info<<"运行在非Debug模式"<<op::endl;
#endif
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
    Log<<Level::Info<<"Starting initialization Version: "<< VERSION_FULL_STRING <<op::endl;
    Log<<Level::Info<<"Initializing GLFW"<<op::endl;
        // 初始化GLFW
    if (!glfwInit()) {
        Log << Level::Error << "GLFW初始化失败" << op::endl;
        core::showGLFWInitError();
        return -1;
    }
    
    try {
        Config::getInstance()->init();
    } catch (const std::exception& e) {
        Log << Level::Warn << "配置初始化失败: " << e.what() << op::endl;
        core::showConfigInitError();
        // 配置初始化失败不会导致程序退出，将使用默认配置
    } catch (...) {
        Log << Level::Warn << "配置初始化失败: 未知异常" << op::endl;
        core::showConfigInitError();
    }
    
    SetConfigItems();
    // 检查程序路径是否包含非ASCII字符
    core::checkProgramPathAndWarn();
    if(std::filesystem::exists("upgrade_temp.exe")){
        std::filesystem::remove("upgrade.exe");
        Log<<Level::Info<<"Found upgrade_temp.exe, starting upgrade process"<<op::endl;
        std::filesystem::rename("upgrade_temp.exe", "upgrade.exe");
    }
    core::startFileWithoutWindow("upgrade.exe");
    // 设置OpenGL版本和兼容性模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    // 启用向前兼容性
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    Config& config=*Config::getInstance();
    Log<<Level::Info<<"Creating window"<<op::endl;
    // 创建窗口
    WindowInfo.window = glfwCreateWindow(config.getInt(WINDOW_WIDTH), config.getInt(WINDOW_HEIGHT), config.get(WINDOW_TITLE).c_str(), nullptr, nullptr);
    if (!WindowInfo.window) {
        Log << Level::Error << "窗口创建失败" << op::endl;
        core::showWindowCreationError();
        glfwTerminate();
        return -1;
    }
    
    // 设置当前上下文
    glfwMakeContextCurrent(WindowInfo.window);
    glfwSwapInterval(Config::getInstance()->getBool(VERTICAL_SYNC)); // 垂直同步
    Log<<Level::Info<<"Loading GLAD"<<op::endl;
    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Log << Level::Error << "GLAD初始化失败" << op::endl;
        core::showGLADInitError();
        glfwTerminate();
        return -1;
    }
    
    // 验证 OpenGL 版本和兼容性
    Log << Level::Info << "OpenGL Version: " << (const char*)glGetString(GL_VERSION) << op::endl;
    Log << Level::Info << "OpenGL Renderer: " << (const char*)glGetString(GL_RENDERER) << op::endl;
    
    // 检查OpenGL版本是否满足要求
    if (!core::validateOpenGLSupport()) {
        Log << Level::Error << "OpenGL版本检查失败，程序将退出" << op::endl;
        glfwTerminate();
        return -1;
    }

    // 创建并初始化字体渲染后端（OpenGL）并注入到 Font
    {
        static core::OpenGLFontRenderer s_fontRenderer;
        if (!s_fontRenderer.Initialize(WindowInfo.window)) {
            Log << Level::Error << "Font renderer initialization failed" << op::endl;
            // 失败时继续，但 Font::SetFontRenderer 不会被设置，创建 Font 时会报错并安全返回
        } else {
            core::Font::SetFontRenderer(&s_fontRenderer);
            Log << Level::Info << "Injected OpenGLFontRenderer into Font" << op::endl;
        }
    }
    
    glfwSetMouseButtonCallback(WindowInfo.window, MouseButtonEvent);
    glfwSetCursorPosCallback(WindowInfo.window, MouseMoveEvent);
    glfwSetKeyCallback(WindowInfo.window, KeyEvent);
    glfwSetCharCallback(WindowInfo.window, CharEvent); // 设置Unicode字符输入回调
    glfwSetFramebufferSizeCallback(WindowInfo.window, framebuffer_size_callback);
    // 打印OpenGL版本
    {
        const GLubyte* version = glGetString(GL_VERSION);
        std::cout << "OpenGL Version: " << version << std::endl;
    }
    core::WindowInfo.width = config.getInt(WINDOW_WIDTH);
    core::WindowInfo.height = config.getInt(WINDOW_HEIGHT);
    
    Log<<Level::Info<<"Init explorer"<<op::endl;
    try {
        core::Explorer::getInstance();
    } catch (const std::exception& e) {
        Log << Level::Error << "资源管理器初始化失败: " << e.what() << op::endl;
        core::showExplorerInitError();
        glfwTerminate();
        return -1;
    } catch (...) {
        Log << Level::Error << "资源管理器初始化失败: 未知异常" << op::endl;
        core::showExplorerInitError();
        glfwTerminate();
        return -1;
    }

    try {
        core::Explorer::getInstance()->playAudio(AudioID::bgm);
    } catch (const std::exception& e) {
        Log << Level::Warn << "背景音乐播放失败: " << e.what() << op::endl;
        // 音频播放失败不会导致程序退出
    } catch (...) {
        Log << Level::Warn << "背景音乐播放失败: 未知异常" << op::endl;
    }
    
    screen::Screen::RegisterScreen(screen::ScreenID::MainMenu, std::make_shared<screen::MainScreen>());
    screen::Screen::SwitchToScreen(screen::ScreenID::MainMenu);
    screen::Screen::RegisterScreen(screen::ScreenID::Settings, std::make_shared<screen::SettingScreen>());
    screen::Screen::RegisterScreen(screen::ScreenID::Video,std::make_shared<screen::VideoScreen>());
    screen::Screen::RegisterScreen(screen::ScreenID::Name,std::make_shared<screen::NameScreen>());
    screen::Screen::RegisterScreen(screen::ScreenID::ListName,std::make_shared<screen::ListNameScreen>());
    SyncConfig();
    Log<<Level::Info<<"starting render loop"<<op::endl<<op::flush;
    return 0;
}

int cleanup() {
    Log<<Level::Info<<"Cleaning up"<<op::endl<<op::flush;
    
    // 清理其他资源
    core::Explorer::getInstance2().reset();

    // 标记OpenGL上下文即将失效，防止后续OpenGL调用引起问题
    SetOpenGLContextInvalid();
    
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