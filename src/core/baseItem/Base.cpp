#include "core/baseItem/Base.h"
#include <locale>
#include <cstring>
#include "core/log.h"
#include <utf8cpp/utf8.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h>
    #include <psapi.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <limits.h>
    #ifdef __APPLE__
        #include <mach-o/dyld.h>
    #else
        #include <linux/limits.h>
    #endif
#endif

using namespace core;

ScreenInfo core::WindowInfo,core::screenInfo;
// 初始化静态成员变量
namespace core {
    namespace color {
        Color white(255, 255, 255, 255);
        Color black(0, 0, 0, 255);
        Color red(255, 0, 0, 255);
        Color green(0, 255, 0, 255);
        Color blue(0, 0, 255, 255);
    }
}

float core::Region::getyend() const { 
    if(yend<=0)
        return (gety()-getx()+getxend());
    else 
        return screenRatio ? yend * WindowInfo.height : yend; 
}

std::string core::wstring2string(const std::wstring& wstr) {
    std::string str;
    utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(str));
    return str;
}

std::wstring core::string2wstring(const std::string& str) {
    std::wstring wstr;
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(wstr));
    return wstr;
}

size_t core::utf8_length(const std::string& str) {
    size_t length = 0;
    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = str[i];
        if (c < 0x80) {
            i += 1; // ASCII字符
        } else if ((c >> 5) == 0x6) {
            i += 2; // 2字节UTF-8字符
        } else if ((c >> 4) == 0xe) {
            i += 3; // 3字节UTF-8字符（包括大部分中文）
        } else if ((c >> 3) == 0x1e) {
            i += 4; // 4字节UTF-8字符
        } else {
            i += 1; // 无效字符，跳过
        }
        length++;
    }
    return length;
}

void core::openFile(const std::string& path) {
    if (path.empty()) return;
#ifdef _WIN32
    // Windows 平台
    std::filesystem::path fspath(core::string2wstring(path));

    std::wstring command = L"start \"\" \"" + fspath.wstring() + L"\"";
    _wsystem(command.c_str());
    return;

#else
    std::filesystem::path fspath(path);
    if (fspath.is_relative()) {
        // 获取可执行文件所在目录
        std::filesystem::path exePath = std::filesystem::current_path();
        fspath = exePath / fspath;
    }
#endif

#ifdef __APPLE__
    // macOS 平台
    std::string command = "open \"" + fspath.string() + "\"";
    system(command.c_str());
#endif
#ifdef __linux__
    // Linux 平台
    std::string command = "xdg-open \"" + fspath.string() + "\"";
    system(command.c_str());
#endif
}


bool core::isFileExists(const std::string& path) {
        if (path.empty()) {
            return false;
        }
        
#ifdef _WIN32
        try {
            std::wstring wPath = core::string2wstring(path);
            std::filesystem::path fsPath(wPath);
            return std::filesystem::exists(fsPath);
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception in FileExistsUTF8 (Windows): " << e.what() << " for path: " << path << op::endl;
            // 回退到标准库方法
            try {
                return std::filesystem::exists(std::filesystem::u8path(path));
            }
            catch (...) {
                return std::filesystem::exists(path);
            }
        }
#else
        // Unix系统（Linux/macOS）：直接使用UTF-8路径
        try {
            bool exists = std::filesystem::exists(path);
            Log << Level::Debug << "FileExistsUTF8 (Unix): " << path << " -> " << (exists ? "exists" : "not found") << op::endl;
            return exists;
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception in FileExistsUTF8 (Unix): " << e.what() << " for path: " << path << op::endl;
            // 回退到C风格检查
            struct stat buffer;
            return (stat(path.c_str(), &buffer) == 0);
        }
#endif
}

bool core::stringContains(const std::string& str, const std::string& substr) {
    if (substr.empty()) {
        return true;  // 空字符串被认为包含在任何字符串中
    }
    if (str.empty()) {
        return false; // 空字符串不能包含非空字符串
    }
    return str.find(substr) != std::string::npos;
}

void core::startFileWithoutWindow(const std::string& path) {
    if (path.empty()) return;
    std::filesystem::path fspath(path);
    if (fspath.is_relative()) {
        // 获取可执行文件所在目录
        std::filesystem::path exePath = std::filesystem::current_path();
        fspath = exePath / fspath;
    }
    #ifdef _WIN32
    // Windows 平台
    std::wstring wpath = core::string2wstring(fspath.string());
    ShellExecuteW(NULL, L"open", wpath.c_str(), NULL, NULL, SW_HIDE);
    #endif
    #ifdef __APPLE__
    // macOS 平台
    std::string command = "open -a \"" + fspath.string() + "\"";
    system(command.c_str());
    #endif
    #ifdef __linux__
    // Linux 平台
    std::string command = fspath.string() + " &"; // 在后台运行
    system(command.c_str());
    #endif
}

void core::quit() {
    glfwSetWindowShouldClose(WindowInfo.window, true);
}

std::string getExecutablePath() {
#ifdef _WIN32
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (length == 0) {
        return "";
    }
    
    // 转换为UTF-8字符串
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, path, -1, NULL, 0, NULL, NULL);
    if (utf8Length == 0) {
        return "";
    }
    
    std::string utf8Path(utf8Length - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, path, -1, &utf8Path[0], utf8Length, NULL, NULL);
    return utf8Path;
    
#elif defined(__APPLE__)
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::string(path);
    }
    
    // 如果缓冲区太小，重新分配
    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
        return std::string(buffer.data());
    }
    return "";
    
#else // Linux
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX - 1);
    if (count == -1) {
        return "";
    }
    path[count] = '\0';
    return std::string(path);
#endif
}

void core::restart() {
    Log << Level::Info << "正在重启程序..." << op::endl;
    
    std::string executablePath = getExecutablePath();
    if (executablePath.empty()) {
        Log << Level::Error << "无法获取可执行文件路径，重启失败" << op::endl;
        return;
    }
    
#ifdef _WIN32
    // Windows 实现
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    
    // 获取命令行参数
    LPWSTR cmdLine = GetCommandLineW();
    
    // 创建新进程
    BOOL success = CreateProcessW(
        NULL,           // 应用程序名称
        cmdLine,        // 命令行
        NULL,           // 进程安全属性
        NULL,           // 线程安全属性
        FALSE,          // 继承句柄
        0,              // 创建标志
        NULL,           // 环境
        NULL,           // 当前目录
        &si,            // 启动信息
        &pi             // 进程信息
    );
    
    if (success) {
        Log << Level::Info << "新进程已启动，正在退出当前进程" << op::endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        // 关闭当前窗口
        glfwSetWindowShouldClose(WindowInfo.window, true);
        
        // 强制退出当前进程
        std::exit(0);
    } else {
        DWORD error = GetLastError();
        Log << Level::Error << "创建新进程失败，错误代码: " << error << op::endl;
    }
    
#else
    // Unix/Linux/macOS 实现
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程：启动新的程序实例
        std::vector<char*> args;
        args.push_back(const_cast<char*>(executablePath.c_str()));
        
        // 这里可以添加原始的命令行参数
        // 目前简化处理，只传递程序名
        args.push_back(nullptr);
        
        // 等待一小段时间让父进程有机会清理
        usleep(100000); // 100ms
        
        execv(executablePath.c_str(), args.data());
        
        // 如果execv失败，记录错误并退出
        Log << Level::Error << "execv 失败" << op::endl;
        std::exit(1);
        
    } else if (pid > 0) {
        // 父进程：准备退出
        Log << Level::Info << "子进程已启动 (PID: " << pid << ")，正在退出当前进程" << op::endl;
        
        // 关闭当前窗口
        glfwSetWindowShouldClose(WindowInfo.window, true);
        
        // 强制退出当前进程
        std::exit(0);
        
    } else {
        // fork 失败
        Log << Level::Error << "fork 失败，无法重启程序" << op::endl;
    }
#endif
}