#include "core/screen/SettingScreen.h"
#include "core/log.h"

#include "core/Config.h"
#include "core/Drawer.h"
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <fstream>
#include <iterator>
#include <locale>
#include <codecvt>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#elif defined(__APPLE__)
#undef DEBUG
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/stat.h>
#endif

using namespace core;
using namespace screen;

// 文件类型过滤器映射表实现
static std::vector<FileTypeFilter> getFiltersForType(FileType fileType) {
    // 定义文件类型到过滤器的映射
    static const std::map<FileType, std::vector<FileTypeFilter>> fileTypeFiltersMap = {
        { FileType::All, {
            {"所有文件(*.*)", {"*.*"}}
        }},
        { FileType::Picture, {
            {"图片文件(*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.tiff;*.webp)", {"*.png","*.jpg","*.jpeg","*.bmp","*.gif","*.tiff","*.webp"}}
        }},
        { FileType::Video, {
            {"视频文件(*.mp4;*.avi;*.mkv;*.mov;*.wmv;*.flv;*.webm)", {"*.mp4","*.avi","*.mkv","*.mov","*.wmv","*.flv","*.webm"} }
        }},
        { FileType::Audio, {
            {"音频文件(*.mp3;*.wav;*.flac;*.ogg;*.aac;*.m4a)", {"*.mp3","*.wav","*.flac","*.ogg","*.aac","*.m4a"}}
        }},
        { FileType::NameFile, {
            // {"文档文件(*.pdf;*.doc;*.docx;*.xlsx)", {"*.pdf","*.doc","*.docx","*.xlsx"}},
            {"文本文件(*.txt)", {"*.txt"}}
        }},
        // FileType::Custom 需要单独处理
    };

    // 查找映射表中是否有对应的文件类型
    auto it = fileTypeFiltersMap.find(fileType);
    if (it != fileTypeFiltersMap.end()) {
        return it->second;
    }
      // 默认返回所有文件类型
    return fileTypeFiltersMap.at(FileType::All);
}

// UTF-8字符处理辅助函数
namespace {
    // 计算UTF-8字符串中的字符数量（而非字节数量）
    size_t utf8_length(const std::string& str) {
        size_t length = 0;
        for (size_t i = 0; i < str.length(); ) {
            unsigned char c = str[i];
            if (c < 0x80) {
                // ASCII字符 (0xxxxxxx)
                i += 1;
            } else if ((c >> 5) == 0x06) {
                // 2字节字符 (110xxxxx 10xxxxxx)
                i += 2;
            } else if ((c >> 4) == 0x0E) {
                // 3字节字符 (1110xxxx 10xxxxxx 10xxxxxx) - 大部分中文字符
                i += 3;
            } else if ((c >> 3) == 0x1E) {
                // 4字节字符 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
                i += 4;
            } else {
                // 无效的UTF-8序列，跳过
                i += 1;
            }
            length++;
        }
        return length;
    }
    
    // 获取UTF-8字符串中指定位置的字符的字节长度
    size_t utf8_char_length(const std::string& str, size_t pos) {
        if (pos >= str.length()) return 0;
        
        unsigned char c = str[pos];
        if (c < 0x80) {
            return 1; // ASCII字符
        } else if ((c >> 5) == 0x06) {
            return 2; // 2字节字符
        } else if ((c >> 4) == 0x0E) {
            return 3; // 3字节字符 (大部分中文)
        } else if ((c >> 3) == 0x1E) {
            return 4; // 4字节字符
        } else {
            return 1; // 无效序列，按1字节处理
        }
    }
    
    // 安全地删除UTF-8字符串中的最后一个字符
    std::string utf8_remove_last_char(const std::string& str) {
        if (str.empty()) return str;
        
        size_t pos = str.length();
        
        // 从后往前找到一个完整字符的开始位置
        while (pos > 0) {
            pos--;
            unsigned char c = str[pos];
            
            // 如果是ASCII字符或UTF-8序列的开始字节
            if (c < 0x80 || (c >> 6) != 0x02) {
                break;
            }
        }
        
        return str.substr(0, pos);
    }
    
    // 将字符位置转换为字节位置
    size_t utf8_char_pos_to_byte_pos(const std::string& str, size_t char_pos) {
        size_t byte_pos = 0;
        size_t current_char = 0;
        
        while (byte_pos < str.length() && current_char < char_pos) {
            unsigned char c = str[byte_pos];
            if (c < 0x80) {
                byte_pos += 1;
            } else if ((c >> 5) == 0x06) {
                byte_pos += 2;
            } else if ((c >> 4) == 0x0E) {
                byte_pos += 3;
            } else if ((c >> 3) == 0x1E) {
                byte_pos += 4;
            } else {
                byte_pos += 1; // 无效序列
            }
            current_char++;
        }
        
        return byte_pos;
    }
    
    // 将字节位置转换为字符位置
    size_t utf8_byte_pos_to_char_pos(const std::string& str, size_t byte_pos) {
        size_t char_pos = 0;
        size_t current_byte = 0;
        
        while (current_byte < byte_pos && current_byte < str.length()) {
            unsigned char c = str[current_byte];
            if (c < 0x80) {
                current_byte += 1;
            } else if ((c >> 5) == 0x06) {
                current_byte += 2;
            } else if ((c >> 4) == 0x0E) {
                current_byte += 3;
            } else if ((c >> 3) == 0x1E) {
                current_byte += 4;
            } else {
                current_byte += 1; // 无效序列
            }
            char_pos++;
        }
        
        return char_pos;
    }
}

#ifdef _WIN32
// Windows软键盘相关函数
namespace {
    // 检查TabTip进程是否正在运行
    bool IsTabTipRunning() {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return false;
        }
        
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        bool found = false;
        if (Process32FirstW(hSnapshot, &pe32)) {
            do {
                if (_wcsicmp(pe32.szExeFile, L"TabTip.exe") == 0) {
                    found = true;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
        return found;
    }
    
    // 显示Windows软键盘
    void ShowVirtualKeyboard() {
        #ifdef _WIN32
        if(!bools[boolconfig::inwindow]){
            defullscreen(core::WindowInfo.window);
        }
        #endif
        try {
            // 方法1：启动osk.exe（Windows触屏键盘）
            HINSTANCE result = ShellExecuteW(
                NULL,
                L"open",
                L"osk.exe",
                NULL,
                NULL,
                SW_SHOW
            );
            
            if ((INT_PTR)result > 32) {
                Log << Level::Info << "Virtual keyboard launched successfully" << op::endl;
            } else {
                Log << Level::Warn << "Failed to launch virtual keyboard, error code: " << (long long)result << op::endl;
                
                // 方法2：尝试通过命令行启动
                system("start osk.exe");
            }
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception showing virtual keyboard: " << e.what() << op::endl;
        }
    }
    
    // 隐藏Windows软键盘
    void HideVirtualKeyboard() {
        #ifdef _WIN32
        if(!bools[boolconfig::inwindow]){
            fullscreen(core::WindowInfo.window);
        }
        #endif
        try {
            // 查找osk窗口并关闭
            HWND hWnd = FindWindowW(L"OSKMainClass", NULL);
            if (hWnd) {
                SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                Log << Level::Info << "Virtual keyboard hidden" << op::endl;
            } else {
                // 尝试通过进程终止的方式
                system("taskkill /f /im osk.exe >nul 2>&1");
            }
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception hiding virtual keyboard: " << e.what() << op::endl;
        }
    }
}

#elif defined(__APPLE__)
// macOS软键盘相关函数
namespace {
    static pid_t keyboardPid = -1;
    
    void ShowVirtualKeyboard() {
        try {
            // macOS方法1：通过AppleScript显示虚拟键盘
            const char* appleScript = R"(
                tell application "System Events"
                    tell process "SystemUIServer"
                        click (menu bar item 1 of menu bar 1 whose description contains "Accessibility")
                        click menu item "Show Accessibility Keyboard" of menu 1 of menu bar item 1 of menu bar 1
                    end tell
                end tell
            )";
            
            std::string command = "osascript -e '";
            command += appleScript;
            command += "'";
            
            int result = system(command.c_str());
            if (result == 0) {
                Log << Level::Info << "macOS virtual keyboard shown via AppleScript" << op::endl;
            } else {
                Log << Level::Warn << "Failed to show virtual keyboard via AppleScript, trying alternative method" << op::endl;
                
                // 方法2：使用系统偏好设置命令
                system("open -b com.apple.preference.universalaccess");
            }
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception showing virtual keyboard on macOS: " << e.what() << op::endl;
        }
    }
    
    void HideVirtualKeyboard() {
        try {
            // 通过AppleScript隐藏虚拟键盘
            const char* appleScript = R"(
                tell application "System Events"
                    tell process "SystemUIServer"
                        click (menu bar item 1 of menu bar 1 whose description contains "Accessibility")
                        click menu item "Hide Accessibility Keyboard" of menu 1 of menu bar item 1 of menu bar 1
                    end tell
                end tell
            )";
            
            std::string command = "osascript -e '";
            command += appleScript;
            command += "'";
            
            system(command.c_str());
            Log << Level::Info << "macOS virtual keyboard hidden" << op::endl;
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception hiding virtual keyboard on macOS: " << e.what() << op::endl;
        }
    }
}

#elif defined(__linux__)
// Linux软键盘相关函数
namespace {
    static pid_t keyboardPid = -1;
    
    // 检测可用的虚拟键盘
    std::string DetectAvailableKeyboard() {
        // 按优先级检测可用的虚拟键盘
        const char* keyboards[] = {
            "onboard",          // Ubuntu/GNOME默认
            "florence",         // 轻量级键盘
            "matchbox-keyboard", // 嵌入式系统常用
            "kvkbd",            // KDE虚拟键盘
            "cellwriter",       // 手写识别键盘
            nullptr
        };
        
        for (int i = 0; keyboards[i] != nullptr; i++) {
            std::string checkCmd = "which ";
            checkCmd += keyboards[i];
            checkCmd += " >/dev/null 2>&1";
            
            if (system(checkCmd.c_str()) == 0) {
                Log << Level::Info << "Found virtual keyboard: " << keyboards[i] << op::endl;
                return std::string(keyboards[i]);
            }
        }
        
        Log << Level::Warn << "No virtual keyboard found on Linux system" << op::endl;
        return "";
    }
    
    void ShowVirtualKeyboard() {
        try {
            if (keyboardPid > 0) {
                Log << Level::Debug << "Virtual keyboard already running" << op::endl;
                return;
            }
            
            std::string keyboard = DetectAvailableKeyboard();
            if (keyboard.empty()) {
                Log << Level::Error << "No virtual keyboard available on this Linux system" << op::endl;
                return;
            }
            
            // 启动虚拟键盘进程
            keyboardPid = fork();
            if (keyboardPid == 0) {
                // 子进程：启动虚拟键盘
                if (keyboard == "onboard") {
                    execl("/usr/bin/onboard", "onboard", "--size", "1000x300", nullptr);
                } else if (keyboard == "florence") {
                    execl("/usr/bin/florence", "florence", nullptr);
                } else if (keyboard == "matchbox-keyboard") {
                    execl("/usr/bin/matchbox-keyboard", "matchbox-keyboard", nullptr);
                } else {
                    execlp(keyboard.c_str(), keyboard.c_str(), nullptr);
                }
                
                // 如果execl失败
                exit(1);
            } else if (keyboardPid > 0) {
                Log << Level::Info << "Linux virtual keyboard (" << keyboard << ") launched with PID: " << keyboardPid << op::endl;
            } else {
                Log << Level::Error << "Failed to fork process for virtual keyboard" << op::endl;
                keyboardPid = -1;
            }
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception showing virtual keyboard on Linux: " << e.what() << op::endl;
        }
    }
    
    void HideVirtualKeyboard() {
        try {
            if (keyboardPid > 0) {
                // 终止虚拟键盘进程
                kill(keyboardPid, SIGTERM);
                
                // 等待进程结束
                int status;
                waitpid(keyboardPid, &status, 0);
                
                Log << Level::Info << "Linux virtual keyboard process terminated" << op::endl;
                keyboardPid = -1;
            } else {
                // 如果没有记录PID，尝试通过进程名终止
                system("pkill -f onboard");
                system("pkill -f florence");
                system("pkill -f matchbox-keyboard");
                system("pkill -f kvkbd");
                
                Log << Level::Info << "Attempted to kill virtual keyboard processes" << op::endl;
            }
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Exception hiding virtual keyboard on Linux: " << e.what() << op::endl;
        }
    }
}

#else
// 其他平台的占位符实现
namespace {
    void ShowVirtualKeyboard() {
        Log << Level::Info << "Virtual keyboard not implemented for this platform" << op::endl;
    }
    
    void HideVirtualKeyboard() {
        Log << Level::Info << "Virtual keyboard not implemented for this platform" << op::endl;
    }
}
#endif

// 跨平台软键盘支持检测
namespace {
    bool IsVirtualKeyboardSupported() {
#ifdef _WIN32
        return true; // Windows始终支持OSK
#elif defined(__APPLE__)
        return true; // macOS支持辅助功能键盘
#elif defined(__linux__)
        // 检测Linux是否有可用的虚拟键盘
        const char* keyboards[] = {
            "onboard", "florence", "matchbox-keyboard", "kvkbd", "cellwriter", nullptr
        };
        
        for (int i = 0; keyboards[i] != nullptr; i++) {
            std::string checkCmd = "which ";
            checkCmd += keyboards[i];
            checkCmd += " >/dev/null 2>&1";
            
            if (system(checkCmd.c_str()) == 0) {
                return true;
            }
        }
        return false;
#else
        return false; // 其他平台暂不支持
#endif
    }
    
    std::string GetPlatformKeyboardInfo() {
#ifdef _WIN32
        return "Windows屏幕键盘 (OSK)";
#elif defined(__APPLE__)
        return "macOS辅助功能键盘";
#elif defined(__linux__)
        // 返回第一个找到的键盘
        const char* keyboards[] = {
            "onboard", "florence", "matchbox-keyboard", "kvkbd", "cellwriter", nullptr
        };
        
        for (int i = 0; keyboards[i] != nullptr; i++) {
            std::string checkCmd = "which ";
            checkCmd += keyboards[i];
            checkCmd += " >/dev/null 2>&1";
            
            if (system(checkCmd.c_str()) == 0) {
                return std::string("Linux虚拟键盘 (") + keyboards[i] + ")";
            }
        }
        return "Linux虚拟键盘 (未安装)";
#else
        return "不支持的平台";
#endif
    }
}

SettingButton::SettingButton(sItem item_, int number, int page)
    : item(item_), number(number), page(page) {
        
        // 验证传入的item参数 - 使用更安全的字符串访问方式
        try {
            // 安全地验证 name 字符串
            if (item.name.empty()) {
                Log << Level::Warn << "SettingButton constructor - name is empty" << op::endl;
                item.name = "unknown_button";
            } else {
                // 验证字符串的有效性，但不输出中文内容到日志
                size_t nameLen = item.name.length();
                if (nameLen > 1000) {
                    Log << Level::Warn << "SettingButton constructor - name too long, truncating" << op::endl;
                    item.name = "long_name_truncated";
                }
            }
            
            // 安全地验证 configName 字符串
            if (item.configName.empty()) {
                Log << Level::Warn << "SettingButton constructor - configName is empty" << op::endl;
                item.configName = "unknown_config";
            } else {
                size_t len = item.configName.length();
                if (len > 1000) {
                    Log << Level::Error << "SettingButton constructor - configName too long" << op::endl;
                    item.configName = "unknown_config";
                }
            }
            
            // 安全地验证 outOfLimitOutPut 字符串
            if (!item.outOfLimitOutPut.empty() && item.outOfLimitOutPut.length() > 1000) {
                Log << Level::Warn << "SettingButton constructor - outOfLimitOutPut too long, truncating" << op::endl;
                item.outOfLimitOutPut = "error_message_truncated";
            }
        }
        catch (const std::exception& e) {
            Log << Level::Error << "SettingButton constructor - exception during string validation: " << e.what() << op::endl;
            item.name = "error_button";
            item.configName = "unknown_config";
            item.outOfLimitOutPut = "error";
        }
        catch (...) {
            Log << Level::Error << "SettingButton constructor - unknown exception during string validation" << op::endl;
            item.name = "error_button";
            item.configName = "unknown_config";  
            item.outOfLimitOutPut = "error";
        }
        
        font=core::Explorer::getInstance()->getFontPtr(FontID::Normal);
        Region Button2Region;
        if(number<=10){
            TextRegion={0,number*0.08+0.1,0.4,number*0.08+0.15};
            if(item.type!=SettingButtonType::PathSelect&&item.type!=SettingButtonType::FileSelect)
                ButtonRegion={0.41,number*0.08+0.1,0.5,number*0.08+0.15};
            else{
                ButtonRegion={0.41,number*0.08+0.1,0.455,number*0.08+0.15};
                Button2Region={0.455,number*0.08+0.1,0.5,number*0.08+0.15};
            }
        }else{
            number-=10;
            TextRegion={0.51,number*0.08+0.1,0.82,number*0.08+0.15};
            if(item.type!=SettingButtonType::PathSelect&&item.type!=SettingButtonType::FileSelect)
                ButtonRegion={0.83,number*0.08+0.1,0.92,number*0.08+0.15};
            else{
                ButtonRegion={0.83,number*0.08+0.1,0.875,number*0.08+0.15};
                Button2Region={0.875,number*0.08+0.1,0.92,number*0.08+0.15};
            }
        }
        button=std::make_shared<core::Button>(Button());
        button->SetColor({0,0,0,255});
        button->SetFontID(FontID::Normal);
        button->SetRegion(ButtonRegion);
        button->SetEnableText(true);
        button->SetFontScale(0.3);
        button->SetBitmap(BitmapID::SettingButton);
        if(item.type==SettingButtonType::Switch){
            // 安全地访问configName
            std::string safeConfigName;
            try {
                if (!item.configName.empty()) {
                    safeConfigName = item.configName;
                } else {
                    Log << Level::Warn << "Switch button has empty configName" << op::endl;
                    safeConfigName = "unknown_config";
                }
                
                boolconfig configEnum = GetBoolConfigFromString(safeConfigName);
                button->SetText(bools[configEnum] ? "开" : "关");
                button->SetClickFunc([this, safeConfigName, configEnum]{
                    try {
                        if (configEnum == (boolconfig)-1) {
                            Log << Level::Error << "Invalid configName: " << safeConfigName << op::endl;
                            return;
                        }
                        bools[configEnum] = !bools[configEnum];
                        button->SetText(bools[configEnum] ? "开" : "关");
                        SyncConfig(); // 同步到配置文件
                        checkActions();
                    }
                    catch (const std::exception& e) {
                        Log << Level::Error << "Error in switch button click: " << e.what() << op::endl;
                    }
                });
            }
            catch (const std::exception& e) {
                Log << Level::Error << "Error setting up switch button: " << e.what() << op::endl;
                button->SetText("错误");
            }
        }
        else if(item.type==SettingButtonType::Textbox){
            // 获取当前配置值作为默认文本
            std::string currentValue = Config::getInstance()->get(item.configName, "");
            button->SetText(currentValue.empty() ? "点击输入" : currentValue);
            // 设置编辑区域（使用按钮区域的完全相同尺寸）
            editingTextRegion = ButtonRegion;
            
            button->SetClickFunc([this, configName = item.configName]{
                try {
                    if (configName.empty()) {
                        Log << Level::Error << "Textbox button has empty configName" << op::endl;
                        return;
                    }
                    
                    Log << Level::Debug << "Textbox button clicked, configName: " << configName << op::endl;
                    // 启动内嵌编辑模式
                    if (!isTextboxEditing) {
                        isTextboxEditing = true;
                        editingText = Config::getInstance()->get(configName, "");
                        cursorPosition = core::utf8_length(editingText); // 使用UTF-8字符长度
                        lastCursorBlink = std::chrono::steady_clock::now();
                        showCursor = true;
                          // 跨平台显示软键盘（仅在支持的平台上）
                        if (IsVirtualKeyboardSupported()) {
                            ShowVirtualKeyboard();
                        } else {
                            Log << Level::Info << "Virtual keyboard not supported on this platform" << op::endl;
                        }
                        
                        Log << Level::Debug << "Started inline text editing for: " << configName 
                            << ", current text: '" << editingText << "'" << op::endl;
                    }
                    else {
                        Log << Level::Debug << "Already editing, ignoring click" << op::endl;
                    }
                }
                catch (const std::exception& e) {
                    Log << Level::Error << "Error in textbox button: " << e.what() << op::endl;
                }
            });
        }
        else if(item.type==SettingButtonType::ColorSelect){
            button->SetEnableBitmap(false);
            button->SetEnableFill(true);
            button->SetFillColor(Config::getInstance()->getUInt(item.configName, Color(0, 0, 0, 255)));
            button->SetText("选择颜色");
            button->SetClickFunc([this, configName = item.configName]{
                // 安全地访问configName
                try {
                    if (configName.empty()) {
                        Log << Level::Error << "ColorSelect button has empty configName" << op::endl;
                        return;
                    }
                    
                    // 处理颜色选择的点击事件
                    Color color = selectColor();
                    Config::getInstance()->set(configName,color);
                    Config::getInstance()->saveToFile();
                    button->SetFillColor(Config::getInstance()->getUInt(configName));
                    checkActions();
                }
                catch (const std::exception& e) {
                    Log << Level::Error << "Error in color select button: " << e.what() << op::endl;
                }
            });
        }
        else if(item.type==SettingButtonType::FileSelect){
            showText=Config::getInstance()->getPath(item.configName,"");
            if(!showText.empty()){
                if(!core::isFileExists(showText)){
                    Log<<Level::Warn<<"FileSelect button - file does not exist: "<<showText<<op::endl;
                    showText="等待选择";
                }
                else{
                    // 只显示文件名，不显示完整路径
                    size_t pos = showText.find_last_of("/\\");
                    if (pos != std::string::npos) {
                        showText = showText.substr(pos + 1);
                    }
                }
            }
            button2=std::make_shared<core::Button>(Button());
            button2->SetRegion(Button2Region);
            button2->SetBitmap(BitmapID::SettingButton);
            button2->SetText("打开");
            button2->SetEnableBitmap(true);
            button2->SetEnableText(true);
            button2->SetEnableFill(false);
            button2->SetFontID(FontID::Normal);
            button2->SetFontScale(0.3);
            button2->SetColor({0,0,0,255});
            button2->SetClickFunc([this]{
                openFile();
            });
            button->SetText("选择");
            button->SetClickFunc([this, configName = item.configName]{
                try {
                    if (configName.empty()) {
                        Log << Level::Error << "FileSelect button has empty configName" << op::endl;
                        return;
                    }
                    
                    std::string path=selectFile();
                    if (!path.empty()) {
                        if(!core::isFileExists(path))return;
                        Config::getInstance()->set(configName,path);
                        Config::getInstance()->saveToFile();
                        // 只显示文件名，不显示完整路径
                        size_t pos = path.find_last_of("/\\");
                        if (pos != std::string::npos) {
                            showText = path.substr(pos + 1);
                        }
                        int i=0;
                        if(item.fileType==FileType::NameFile)
                            for(const auto& c_char:item.configName){
                                // 检查字符是否为数字
                                if (std::isdigit(static_cast<unsigned char>(c_char))) {
                                    try{
                                        i = std::stoi(std::string(1, c_char));
                                    }
                                    catch (...) {
                                        // 如果转换失败，则忽略此字符
                                    }
                                    if(i>0&&i<10){ // 假设 i 应该是 1-9 之间的单个数字
                                        break;
                                    }
                                }
                            }
                        switch (item.fileType)
                        {
                        case FileType::Picture:core::Explorer::getInstance()->loadBitmap(item.bitmapID, path);break;
                        case FileType::Video:core::Explorer::getInstance()->loadVideo(item.videoID, path);break;
                        case FileType::Audio:core::Explorer::getInstance()->loadAudio(item.audioID, path);break;
                        case FileType::Sound:core::Explorer::getInstance()->loadSound(item.audioID, path);break;
                        case FileType::Font:
                            if(item.fontID!=FontID::Name){core::Explorer::getInstance()->loadFont(item.fontID, path,false,48);}
                            else core::Explorer::getInstance()->loadFont(item.fontID, path,false,150);break;
                        case FileType::NameFile: core::NameRandomer::getInstance(i)->setFile(path); break;
                        default:break;
                        }
                        checkActions();
                    }
                }
                catch (const std::exception& e) {
                    Log << Level::Error << "Error in file select button: " << e.what() << op::endl;
                }
            });
        }
        else if(item.type==SettingButtonType::PathSelect){
            showText=Config::getInstance()->get(item.configName,"");
            if(!showText.empty()){
                if(!core::isFileExists(showText)){
                    showText="等待选择";
                }
                else{
                    // 只显示文件名，不显示完整路径
                    size_t pos = showText.find_last_of("/\\");
                    if (pos != std::string::npos) {
                        showText = showText.substr(pos + 1);
                    }
                }
            }
            button2=std::make_shared<core::Button>(Button());
            button2->SetRegion(Button2Region);
            button2->SetBitmap(BitmapID::SettingButton);
            button2->SetText("打开");
            button2->SetEnableBitmap(true);
            button2->SetEnableText(true);
            button2->SetEnableFill(false);
            button2->SetFontID(FontID::Normal);
            button2->SetFontScale(0.3);
            button2->SetColor({0,0,0,255});
            button2->SetClickFunc([this]{
                openFile();
            });
            button->SetText("选择");
            button->SetClickFunc([this, configName = item.configName]{
                try {
                    if (configName.empty()) {
                        Log << Level::Error << "PathSelect button has empty configName" << op::endl;
                        return;
                    }
                    
                    std::string path=selectPath();
                    if (!path.empty()) {
                        if(!core::isFileExists(path))return;
                        if(!showText.empty()){
                            // 只显示文件名，不显示完整路径
                            size_t pos = path.find_last_of("/\\");
                            if (pos != std::string::npos) {
                                showText = path.substr(pos + 1);
                            }
                        }
                        Config::getInstance()->set(configName,path);
                        Config::getInstance()->saveToFile();
                        button->SetText(Config::getInstance()->get(configName));
                        button->SetText(path);
                        button->SetEnableFill(false);
                        button->SetEnableBitmap(true);
                        button->SetBitmap(BitmapID::SettingButton);
                        if(item.fileType==FileType::Picture)core::Explorer::getInstance()->loadImagesFromDirectory(path);
                        checkActions();
                    }
                }
                catch (const std::exception& e) {
                    Log << Level::Error << "Error in path select button: " << e.what() << op::endl;
                }
            });
        }
}

void SettingButton::Draw(int currentPage, unsigned char alpha)const {
    if(currentPage!=page)return;
    if(!item.name.empty()){
        if(!showText.empty()){
            Region TextRegion2={TextRegion.getOriginXEnd()-0.15f, TextRegion.getOriginY(), TextRegion.getOriginXEnd(), TextRegion.getOriginYEnd(),true};
            if(bools[boolconfig::debug]){
                Drawer::getInstance()->DrawSquare(TextRegion2,Color(255,0,0,255),false);
                Drawer::getInstance()->DrawSquare(TextRegion,Color(0,255,0,255),false);
            }
            if(font&&*font){
                (*font)->RenderTextBetween(showText, TextRegion2, 0.35f, Color(255, 255, 255, alpha));
                (*font)->RenderText(item.name,TextRegion.getx(),TextRegion.gety(),0.5f,Color(255,255,255,alpha));
            }
        }
        else
            if(font&&*font)
                (*font)->RenderTextBetween(item.name, TextRegion, 0.6f, Color(255, 255, 255, alpha));
    }      // 如果正在编辑文本框，绘制内嵌编辑框
    if(isTextboxEditing && item.type == SettingButtonType::Textbox) {
        
        // 绘制编辑框背景（亮白色，完全不透明）
        Drawer::getInstance()->DrawSquare(editingTextRegion, Color(255, 255, 255, 255), true);
        // 绘制边框（蓝色）
        Drawer::getInstance()->DrawSquare(editingTextRegion, Color(0,120,255,255), false);
        
        // 准备显示文本（包含光标）
        std::string displayText = editingText;
        
        // 光标闪烁逻辑
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCursorBlink).count();
        if (elapsed > 500) { // 每500ms闪烁一次
            const_cast<SettingButton*>(this)->showCursor = !showCursor;
            const_cast<SettingButton*>(this)->lastCursorBlink = now;
        }
          // 添加光标
        if (showCursor && cursorPosition <= core::utf8_length(displayText)) {
            // 将字符位置转换为字节位置来插入光标
            size_t byte_pos = utf8_char_pos_to_byte_pos(displayText, cursorPosition);
            displayText.insert(byte_pos, "|");
        }
        
        // 如果文本为空，显示提示
        if (displayText.empty() || (displayText.size() == 1 && displayText[0] == '|')) {
            displayText = showCursor ? "|点击输入..." : "点击输入...";
        }
        
        // 计算文本渲染区域（与背景区域完全一致，只是稍微缩进避免贴边）
        Region textDrawRegion = editingTextRegion;
        float margin = 0.002f; // 减少缩进，使文本更居中
        textDrawRegion.setx(textDrawRegion.getx() + margin);
        textDrawRegion.setxend(textDrawRegion.getxend() - margin);
        textDrawRegion.sety(textDrawRegion.gety() + margin);
        textDrawRegion.setyend(textDrawRegion.getyend() - margin);
        textDrawRegion.setRatio(false);
        
        // 绘制编辑中的文本（黑色文字，较大字体）
        if (font && *font)
            (*font)->RenderTextBetween(displayText, textDrawRegion, 0.3f, Color(0, 0, 0, 255));
    }
    else if(button){
        button->Draw(alpha);
    }
    
    if(button2){
        button2->Draw(alpha);
    }
}

bool SettingButton::Click(core::Point point,int page) {
    if(page!=this->page)return false;
    
    // 如果正在编辑文本框
    if (isTextboxEditing && item.type == SettingButtonType::Textbox) {
        // 检查点击是否在编辑区域内
        if (point.getx() >= editingTextRegion.getx() && point.getx() <= editingTextRegion.getxend() &&
            point.gety() >= editingTextRegion.gety() && point.gety() <= editingTextRegion.getyend()) {
            Log << Level::Debug << "Click inside editing region, continuing edit" << op::endl;
            return true; // 在编辑区域内的点击不结束编辑
        } else {
            Log << Level::Debug << "Click outside editing region, finishing edit" << op::endl;
            FinishEditing(); // 点击编辑区域外自动保存
            return false; // 允许其他控件处理这个点击
        }
    }
    
    if (button && button->OnClick(point)) {
        return true;
    }
    if (button2 && button2->OnClick(point)) {
        return true;
    }
    return false;
}

core::Color SettingButton::selectColor(){
    Color currentColor = Config::getInstance()->getUInt(item.configName);

    // 将当前颜色转换为十六进制字符串表示
    char hexColor[8];
    snprintf(hexColor, sizeof(hexColor), "#%02x%02x%02x", currentColor.r, currentColor.g, currentColor.b);

    // 创建用于接收结果的RGB数组
    unsigned char resultRGB[3] = {0, 0, 0};
    std::string title="选择"+item.name;
    // 调用 tinyfd 的颜色选择器
    const char* result = tinyfd_colorChooser(
        title.c_str(),  // 对话框标题
        hexColor,    // 默认颜色（十六进制格式）
        NULL,        // 默认RGB数组（可选，因为已提供十六进制）
        resultRGB    // 输出的RGB值数组
    );
    
    // 如果用户选择了颜色（未取消）
    if (result != NULL) {
        // 使用返回的RGB数组值，这样更直接且安全
        currentColor.r = resultRGB[0];
        currentColor.g = resultRGB[1];
        currentColor.b = resultRGB[2];
        currentColor.a = 255;
    }
    
    return currentColor;
}

std::string SettingButton::selectFile() {
    // 检查 configName 是否有效
    if (item.configName.empty()) {
        Log << Level::Error << "SettingButton::selectFile() - configName is empty" << op::endl;
        return "";
    }
    
    // 验证字符串内存是否有效
    try {
        // 尝试访问字符串的基本属性以检测内存损坏
        size_t len = item.configName.length();
        if (len == 0 || len > 1000) { // 合理的长度检查
            Log << Level::Error << "SettingButton::selectFile() - configName has invalid length: " << len << op::endl;
            return "";
        }
        
        // 检查字符串是否包含有效字符
        const char* data = item.configName.c_str();
        if (data == nullptr) {
            Log << Level::Error << "SettingButton::selectFile() - configName data is null" << op::endl;
            return "";
        }
        
        Log << Level::Debug << "SettingButton::selectFile() - configName: " << item.configName << op::endl;
    }
    catch (const std::exception& e) {
        Log << Level::Error << "SettingButton::selectFile() - Exception while accessing configName: " << e.what() << op::endl;
        return "";
    }
    catch (...) {
        Log << Level::Error << "SettingButton::selectFile() - Unknown exception while accessing configName" << op::endl;
        return "";
    }
    
    std::string defaultPath;
    try {
        defaultPath = Config::getInstance()->get(item.configName);
    }
    catch (const std::exception& e) {
        Log << Level::Error << "SettingButton::selectFile() - Exception in Config::get(): " << e.what() << op::endl;
        // 即使无法获取配置的路径，也可能希望允许用户选择一个新文件，
        // 因此这里不立即返回空字符串，defaultPath 将为空。
        // 或者，根据需求，如果无法获取默认路径则返回 ""。
        // defaultPath = ""; // 确保 defaultPath 为空字符串
    }

    // 准备扁平化的过滤器模式列表和描述列表
    std::vector<const char*> flatPatternList;
    std::vector<std::string> descriptionList;

    std::vector<FileTypeFilter> filterGroups = getFiltersForType(item.fileType);

    // 从 getFiltersForType 返回的每个组中收集模式和描述
    // getFiltersForType 保证至少返回 FileType::All 的过滤器
    for (const auto& group : filterGroups) {
        if (!group.pattern.empty()) {
            flatPatternList.insert(flatPatternList.end(), group.pattern.begin(), group.pattern.end());
            // 仅当从此组添加了模式并且描述不为空时才添加描述
            if (!group.description.empty()) {
                 descriptionList.push_back(group.description);
            }
        }
    }

    // 如果在处理完所有组后 flatPatternList 仍然为空（不太可能，因为 FileType::All），
    // 或者没有收集到描述，则添加默认值。
    // getFiltersForType 的回退机制应确保此情况不会发生。
    if (flatPatternList.empty()) {
        Log << Level::Warn << "Pattern list is empty after processing filter groups. Defaulting to all files." << op::endl;
        flatPatternList.push_back("*.*");
    }
    if (descriptionList.empty()) {
        // 如果没有描述，则使用通用描述
        descriptionList.push_back("所有文件");
    }
    
    // 将描述列表组合成单个字符串
    std::string combinedDescription;
    if (!descriptionList.empty()) {
        combinedDescription = descriptionList[0];
        for (size_t i = 1; i < descriptionList.size(); ++i) {
            combinedDescription += "; " + descriptionList[i]; // tinyfd 可能只显示第一个，或以某种方式处理组合字符串
        }
    }
    // 如果 combinedDescription 仍然为空（理论上不应发生），tinyfd 会处理 NULL 或空描述。
    
    Log << Level::Debug << "Total filter patterns for tinyfd: " << flatPatternList.size() 
              << ", Combined description for tinyfd: " << combinedDescription << op::endl;
    
    // 调用文件选择对话框
    const char* selectedFile = tinyfd_openFileDialog(
        item.name.c_str(),                                         // 对话框标题
        defaultPath.c_str(),                            // 默认路径/文件
        static_cast<int>(flatPatternList.size()),       // 扁平化模式列表中的模式总数
        flatPatternList.data(),                         // 指向模式数组的指针 (const char* const*)
        combinedDescription.c_str(),                    // 单个描述字符串
        0                                               // 0 表示单文件选择，1 表示多文件选择
    );
    
    if (selectedFile != NULL) {
        return std::string(selectedFile);
    }
    
    // 如果用户取消或未选择文件，则返回原始的 defaultPath
    // 如果最初未能从 Config 获取 defaultPath，它将是一个空字符串。
    return defaultPath;
}

std::string SettingButton::selectPath(){
    // 获取当前配置的路径作为初始路径
    std::string defaultPath = Config::getInstance()->get(item.configName, "");
    std::string title = "选择" + item.name;
    // 调用 tinyfiledialogs 的文件夹选择对话框
    const char* selectedFolder = tinyfd_selectFolderDialog(
        title.c_str(),       // 对话框标题
        defaultPath.c_str() // 初始目录
    );
    
    // 如果用户选择了文件夹（未取消）
    if (selectedFolder != NULL) {
        return std::string(selectedFolder);
    }
    
    // 如果用户取消了选择，返回原始路径
    return defaultPath;
}

void SettingButton::openFile() {
    core::openFile(Config::getInstance()->get(item.configName));
}

void SettingButton::checkActions(){
    if(item.action&SettingButtonAction::Restart){
        int result = tinyfd_messageBox("重启提示", "修改设置后需要重启程序才能生效，是否现在重启？", "yesno", "question", 1);
        if (result == 1) {
            Log << Level::Info << "User chose to restart the application" << op::endl;
            core::restart();
        } else {
            Log << Level::Info << "User chose not to restart the application" << op::endl;
        }
    }
    if(item.action&SettingButtonAction::ResetWindowTitle){
        glfwSetWindowTitle(WindowInfo.window, Config::getInstance()->get(WINDOW_TITLE, "祈愿").c_str());
    }
    if(item.action&SettingButtonAction::ReloadVideo){
        if(Config::getInstance()->getBool(NO_VIDEO_PRELOAD,false)){
            core::Explorer::getInstance()->unloadAllVideo();
        }
    }
    SyncConfig();
}

bool SettingButton::HandleKeyInput(char key) {
    if (!isTextboxEditing || item.type != SettingButtonType::Textbox) {
        return false;
    }
    
    Log << Level::Debug << "HandleKeyInput called with key: " << (int)key << " ('" << key << "')" << op::endl;
    
    if (key == '\b' || key == 127) { // 退格键
        if (cursorPosition > 0) {
            try {
                // 安全地删除光标前的一个UTF-8字符
                size_t byte_pos = utf8_char_pos_to_byte_pos(editingText, cursorPosition);
                
                if (byte_pos > 0) {
                    // 找到前一个字符的开始位置
                    size_t prev_char_start = byte_pos;
                    do {
                        prev_char_start--;
                        if (prev_char_start == 0) break;
                        unsigned char c = editingText[prev_char_start];
                        // 找到UTF-8字符的开始字节 (不是continuation byte)
                        if (c < 0x80 || (c >> 6) != 0x02) {
                            break;
                        }
                    } while (prev_char_start > 0);
                    
                    // 删除从prev_char_start到byte_pos的字符
                    editingText.erase(prev_char_start, byte_pos - prev_char_start);
                    cursorPosition--;
                    
                    Log << Level::Debug << "Backspace (UTF-8): new text = '" << editingText 
                        << "', cursor = " << cursorPosition << " chars, " 
                        << core::utf8_length(editingText) << " total chars" << op::endl;
                }
            }
            catch (const std::exception& e) {
                Log << Level::Error << "Error in UTF-8 backspace handling: " << e.what() << op::endl;
                // fallback: 使用简单的字符串删除
                if (!editingText.empty()) {
                    editingText = utf8_remove_last_char(editingText);
                    cursorPosition = core::utf8_length(editingText);
                }
            }
        }
    }
    else if (key == '\r' || key == '\n') { // 回车键 - 完成输入
        Log << Level::Debug << "Enter pressed, finishing editing" << op::endl;
        FinishEditing();
    }
    else if (key == 27) { // ESC键 - 取消输入
        Log << Level::Debug << "ESC pressed, cancelling editing" << op::endl;
        CancelEditing();
    }
    // 注意：ASCII可打印字符现在通过HandleUnicodeInput处理，避免重复输入
    else {
        // 对于其他控制字符，记录但不处理
        Log << Level::Debug << "Control key ignored: " << (int)key << op::endl;
    }
    
    return true;
}

bool SettingButton::HandleUnicodeInput(const std::string& utf8_char) {
    if (!isTextboxEditing || item.type != SettingButtonType::Textbox) {
        return false;
    }
    
    // 跳过控制字符 (如退格、回车、ESC等，这些在HandleKeyInput中处理)
    if (utf8_char.length() == 1 && utf8_char[0] < 32) {
        return false;
    }
    
    Log << Level::Debug << "HandleUnicodeInput called with UTF-8 char: '" << utf8_char << "'" << op::endl;
    
    try {
        // 将UTF-8字符插入到当前光标位置
        size_t byte_pos = utf8_char_pos_to_byte_pos(editingText, cursorPosition);
        editingText.insert(byte_pos, utf8_char);
        cursorPosition++;
        
        Log << Level::Debug << "UTF-8 character inserted: new text = '" << editingText 
            << "', cursor = " << cursorPosition << " chars, " 
            << (long)core::utf8_length(editingText) << " total chars" << op::endl;
    }
    catch (const std::exception& e) {
        Log << Level::Error << "Error inserting UTF-8 character: " << e.what() << op::endl;
        return false;
    }
    
    return true;
}

void SettingButton::FinishEditing() {
    if (!isTextboxEditing) return;
    if(item.action&SettingButtonAction::CountBetween){
        // 检测传入字符串是否为纯数字
        bool isValidNumber = true;
        if (editingText.empty()) {
            isValidNumber = false;
        } else {
            for (size_t i = 0; i < editingText.length(); ++i) {
                char c = editingText[i];
                // 允许数字、小数点和负号（负号只能在开头）
                if (!std::isdigit(c) && c != '.' && !(c == '-' && i == 0)) {
                    isValidNumber = false;
                    break;
                }
            }
            
            // 检查小数点数量不超过1个
            if (isValidNumber) {
                int dotCount = 0;
                for (char c : editingText) {
                    if (c == '.') {
                        dotCount++;
                    }
                }
                if (dotCount > 1) {
                    isValidNumber = false;
                }
            }
        }
        
        if (!isValidNumber) {
            Log << Level::Error << "Invalid number format: " << editingText << op::endl;
            // 显示错误提示
            tinyfd_messageBox("输入错误", "请输入有效的数字", "ok", "error", 1);
            CancelEditing();
            return;
        }
        
        try{
            double value = std::stod(editingText);
            Log << Level::Debug << "Parsed value for count: " << value << op::endl;
            if(value<item.minCount||value>item.maxCount){
                Log << Level::Warn << "Value out of range: " << value << ", expected between " 
                    << item.minCount << " and " << item.maxCount << op::endl;

                // 显示警告提示
                std::string warningMessage = "输入值 " + editingText + " 超出范围 (" + 
                    std::to_string(item.minCount) + " - " + std::to_string(item.maxCount) + ")";
                tinyfd_messageBox("输入错误", warningMessage.c_str(), "ok", "error", 1);
                // 取消编辑
                CancelEditing();               
                return;
                
            }
        }
        catch (const std::exception& e) {
            Log << Level::Error << "Invalid input for count: " << editingText << ", error: " << e.what() << op::endl;
            // 显示错误提示
            tinyfd_messageBox("输入错误", "请输入有效的数字", "ok", "error", 1);
            CancelEditing();
            return;
        }
    }
    try {
        // 保存到配置
        Config::getInstance()->set(item.configName, editingText);
        Config::getInstance()->saveToFile();

        // 更新按钮显示
        button->SetText(editingText.empty() ? "点击输入" : editingText);
          // 关闭编辑模式
        isTextboxEditing = false;
        
        // 跨平台隐藏软键盘（仅在支持的平台上）
        if (IsVirtualKeyboardSupported()) {
            HideVirtualKeyboard();
        }
        
        // 执行相关动作
        checkActions();
        
        Log << Level::Debug << "Finished text editing, saved: " << editingText << op::endl;
    }
    catch (const std::exception& e) {
        Log << Level::Error << "Error finishing text edit: " << e.what() << op::endl;
        CancelEditing();
    }
}

void SettingButton::CancelEditing() {
    if (!isTextboxEditing) return;
      isTextboxEditing = false;
    
    // 跨平台隐藏软键盘（仅在支持的平台上）
    if (IsVirtualKeyboardSupported()) {
        HideVirtualKeyboard();
    }
    
    Log << Level::Debug << "Cancelled text editing" << op::endl;
}