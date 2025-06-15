#include "core/screen/SettingScreen.h"
#include "core/log.h"

#include "core/Config.h"
#include "core/Drawer.h"
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <fstream>
#include <iterator>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
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
        try {
            // 方法1：启动TabTip.exe（Windows触屏键盘）
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
        try {
            // 查找TabTip窗口并关闭
            HWND hWnd = FindWindowW(L"IPTip_Main_Window", NULL);
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
#endif

SettingButton::SettingButton(sItem item_, int number, int page)
    : item(item_), number(number), page(page) {
        
        // 验证传入的item参数
        if (item.configName.empty()) {
            Log << Level::Warn << "SettingButton constructor - configName is empty for item: " << item.name << op::endl;
            item.configName = "unknown_config"; // 设置默认值
        }
        
        // 验证configName字符串的完整性
        try {
            size_t len = item.configName.length();
            const char* data = item.configName.c_str();
            if (data == nullptr || len == 0 || len > 1000) {
                Log << Level::Error << "SettingButton constructor - invalid configName for item: " << item.name << op::endl;
                item.configName = "unknown_config"; // 设置安全的默认值
            }
        }
        catch (...) {
            Log << Level::Error << "SettingButton constructor - exception accessing configName for item: " << item.name << op::endl;
            item.configName = "unknown_config"; // 设置安全的默认值
        }
        
        font=core::Explorer::getInstance()->getFont(FontID::Normal);
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
                        bools[configEnum] = !bools[configEnum];
                        button->SetText(bools[configEnum] ? "开" : "关");
                        SyncBoolsToConfig(); // 同步到配置文件
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
        }        else if(item.type==SettingButtonType::Textbox){
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
                        cursorPosition = editingText.length();
                        lastCursorBlink = std::chrono::steady_clock::now();
                        showCursor = true;
                        
#ifdef _WIN32
                        // 显示软键盘
                        ShowVirtualKeyboard();
#endif
                        
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
            button->SetFillColor((unsigned int)Config::getInstance()->getInt(item.configName, Color(0, 0, 0, 255)));
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
                    button->SetFillColor((unsigned int)Config::getInstance()->getInt(configName));
                    checkActions();
                }
                catch (const std::exception& e) {
                    Log << Level::Error << "Error in color select button: " << e.what() << op::endl;
                }
            });
        }
        else if(item.type==SettingButtonType::FileSelect){
            showText=Config::getInstance()->get(item.configName,"");
            if(!showText.empty()){
                if(!std::filesystem::exists(showText)){
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
                        if(!std::filesystem::exists(path))return;
                        Config::getInstance()->set(configName,path);
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
                if(!std::filesystem::exists(showText)){
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
                        if(!std::filesystem::exists(path))return;
                        if(!showText.empty()){
                            // 只显示文件名，不显示完整路径
                            size_t pos = path.find_last_of("/\\");
                            if (pos != std::string::npos) {
                                showText = path.substr(pos + 1);
                            }
                        }
                        Config::getInstance()->set(configName,path);
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
            font->RenderTextBetween(showText, TextRegion2, 0.35f, Color(255, 255, 255, alpha));
            font->RenderText(item.name,TextRegion.getx(),TextRegion.gety(),0.5f,Color(255,255,255,alpha));
        }
        else
            font->RenderTextBetween(item.name, TextRegion, 0.6f, Color(255, 255, 255, alpha));
    }      // 如果正在编辑文本框，绘制内嵌编辑框
    if(isTextboxEditing && item.type == SettingButtonType::Textbox) {
        
        // 绘制编辑框背景（亮白色，完全不透明）
        Drawer::getInstance()->DrawSquare(editingTextRegion, Color(255, 255, 255, 255), true);
        // 绘制边框（蓝色）
        Drawer::getInstance()->DrawSquare(editingTextRegion, Color(0, 120, 255, 255), false);
        
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
        if (showCursor && cursorPosition <= displayText.length()) {
            displayText.insert(cursorPosition, "|");
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
        font->RenderTextBetween(displayText, textDrawRegion, 0.5f, Color(0, 0, 0, 255));
        
        // 绘制编辑提示（在编辑框下方）
        std::string hint = "回车确认 | ESC取消";
        Region hintRegion = editingTextRegion;
        hintRegion.sety(hintRegion.getyend() + 0.002f);
        hintRegion.setyend(hintRegion.gety() + 0.025f);
        hintRegion.setRatio(false);
        font->RenderTextBetween(hint, hintRegion, 0.25f, Color(120, 120, 120, 200));
    }
    else if(button){
        button->Draw(alpha);
    }
    
    if(button2){
        button2->Draw(alpha);
    }
}

bool SettingButton::Click(Point point,int page) {
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
    Color currentColor = Config::getInstance()->getInt(item.configName);

    // 将当前颜色转换为十六进制字符串表示
    char hexColor[8];
    sprintf(hexColor, "#%02x%02x%02x", currentColor.r, currentColor.g, currentColor.b);
    
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
    std::string filePath = Config::getInstance()->get(item.configName);
    if (filePath.empty()) return;
    std::filesystem::path fspath(filePath);
    if (fspath.is_relative()) {
        // 获取可执行文件所在目录
        std::filesystem::path exePath = std::filesystem::current_path();
        fspath = exePath / fspath;
    }
    // 在 tinyfiledialogs 中没有直接的打开文件功能
    // 使用平台特定的命令来打开文件
#ifdef _WIN32
    // Windows 平台
    std::string command = "start \"\" \"" + fspath.string() + "\"";
    system(command.c_str());
#elif defined(__APPLE__)
    // macOS 平台
    std::string command = "open \"" + fspath.string() + "\"";
    system(command.c_str());
#else
    // Linux 平台
    std::string command = "xdg-open \"" + fspath.string() + "\"";
    system(command.c_str());
#endif
}

void SettingButton::checkActions(){
    if(item.type==SettingButtonType::Switch){

    }
    if(item.action&SettingButtonAction::Restart){
    }
}

bool SettingButton::HandleKeyInput(char key) {
    if (!isTextboxEditing || item.type != SettingButtonType::Textbox) {
        return false;
    }
    
    Log << Level::Debug << "HandleKeyInput called with key: " << (int)key << " ('" << key << "')" << op::endl;
    
    if (key == '\b' || key == 127) { // 退格键
        if (cursorPosition > 0) {
            editingText.erase(cursorPosition - 1, 1);
            cursorPosition--;
            Log << Level::Debug << "Backspace: new text = '" << editingText << "', cursor = " << cursorPosition << op::endl;
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
    else if (key >= 32 && key <= 126) { // 可打印字符
        editingText.insert(cursorPosition, 1, key);
        cursorPosition++;
        Log << Level::Debug << "Character added: new text = '" << editingText << "', cursor = " << cursorPosition << op::endl;
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
        
        // 更新按钮显示
        button->SetText(editingText.empty() ? "点击输入" : editingText);
          // 关闭编辑模式
        isTextboxEditing = false;
        
#ifdef _WIN32
        // 隐藏软键盘
        HideVirtualKeyboard();
#endif
        
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
    
#ifdef _WIN32
    // 隐藏软键盘
    HideVirtualKeyboard();
#endif
    
    Log << Level::Debug << "Cancelled text editing" << op::endl;
}

std::string SettingButton::selectText() {
    // 获取当前配置的文本作为默认值
    std::string defaultText = Config::getInstance()->get(item.configName, "");
    
    Log << Level::Debug << "selectText() called, defaultText: " << defaultText << op::endl;
    
#ifdef _WIN32
    // Windows 平台：使用美观的自定义对话框
    return showSimpleInputDialog(item.name, "请输入内容:", defaultText);
#else
    // 非Windows平台使用tinyfiledialogs
    const char* result = tinyfd_inputBox(
        item.name.c_str(),           // 对话框标题
        "请输入内容:",               // 提示文本
        defaultText.c_str()          // 默认值
    );
    
    if (result != NULL) {
        return std::string(result);
    }
    
    return defaultText;
#endif
}

#ifdef _WIN32
std::string SettingButton::showSimpleInputDialog(const std::string& title, const std::string& prompt, const std::string& defaultValue) {
    // 获取父窗口
    HWND parentHwnd = GetForegroundWindow();
    if (!parentHwnd) {
        parentHwnd = GetActiveWindow();
    }
    
    // 创建模态对话框窗口
    const char* className = "ModernInputDialog";
    
    // 注册窗口类
    WNDCLASSA wc = {};
    wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        static HWND hEdit = nullptr;
        static HWND hStatic = nullptr;
        static std::string* pResult = nullptr;
        
        switch (msg) {
        case WM_CREATE:
            {
                CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
                pResult = (std::string*)cs->lpCreateParams;
                
                // 创建提示标签
                hStatic = CreateWindowA("STATIC", "请输入内容:", 
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    20, 20, 300, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
                
                // 创建输入框
                hEdit = CreateWindowA("EDIT", pResult ? pResult->c_str() : "",
                    WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                    20, 50, 300, 25, hwnd, (HMENU)100, GetModuleHandle(NULL), NULL);
                
                // 创建确定按钮
                CreateWindowA("BUTTON", "确定",
                    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    170, 90, 70, 30, hwnd, (HMENU)IDOK, GetModuleHandle(NULL), NULL);
                
                // 创建取消按钮
                CreateWindowA("BUTTON", "取消",
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    250, 90, 70, 30, hwnd, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);
                
                // 设置现代化字体
                HFONT hFont = CreateFontA(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Microsoft YaHei");
                
                if (hFont) {
                    SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
                    EnumChildWindows(hwnd, [](HWND child, LPARAM font) -> BOOL {
                        char className[256];
                        GetClassNameA(child, className, sizeof(className));
                        if (strcmp(className, "Button") == 0) {
                            SendMessage(child, WM_SETFONT, font, TRUE);
                        }
                        return TRUE;
                    }, (LPARAM)hFont);
                }
                
                // 聚焦到输入框并选中所有文本
                SetFocus(hEdit);
                SendMessage(hEdit, EM_SETSEL, 0, -1);
                
                return 0;
            }
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case IDOK:
                {
                    char buffer[1024];
                    GetWindowTextA(hEdit, buffer, sizeof(buffer));
                    if (pResult) {
                        *pResult = buffer;
                    }
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    return 0;
                }
                
            case IDCANCEL:
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                return 0;
                
            case 100: // Edit control
                if (HIWORD(wParam) == EN_CHANGE) {
                    // 可以在这里添加实时验证
                }
                break;
            }
            break;
            
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_CTLCOLORSTATIC:
            // 现代化颜色方案 - 深色文字，浅色背景
            SetTextColor((HDC)wParam, RGB(33, 37, 41));
            SetBkColor((HDC)wParam, RGB(248, 249, 250));
            return (LRESULT)GetStockObject(WHITE_BRUSH);
            
        case WM_CTLCOLOREDIT:
            // 输入框颜色
            SetTextColor((HDC)wParam, RGB(33, 37, 41));
            SetBkColor((HDC)wParam, RGB(255, 255, 255));
            return (LRESULT)GetStockObject(WHITE_BRUSH);
        }
        
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    };
    
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClassA(&wc);
    
    // 创建对话框
    std::string result = defaultValue;
    HWND hDlg = CreateWindowA(className, title.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 360, 160,
        parentHwnd, NULL, GetModuleHandle(NULL), &result);
    
    if (hDlg) {
        // 居中显示
        RECT rect;
        GetWindowRect(hDlg, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
        SetWindowPos(hDlg, HWND_TOPMOST, x, y, width, height, SWP_SHOWWINDOW);
        
        // 确保窗口在前台
        SetForegroundWindow(hDlg);
        SetActiveWindow(hDlg);
        
        // 消息循环
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            
            if (!IsDialogMessage(hDlg, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            // 检查窗口是否还存在
            if (!IsWindow(hDlg)) {
                break;
            }
        }
    }
    
    UnregisterClassA(className, GetModuleHandle(NULL));
    
    Log << Level::Debug << "Input dialog result: " << result << op::endl;
    return result;
}
#endif

void SettingButton::updateConfig() {
    
}