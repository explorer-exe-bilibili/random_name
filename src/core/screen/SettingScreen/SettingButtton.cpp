#include "core/screen/SettingScreen.h"
#include "core/log.h"

#include "core/Config.h"
#include "core/Drawer.h"
#include <tinyfiledialogs/tinyfiledialogs.h>

using namespace core;
using namespace screen;

// 文件类型过滤器映射表实现
static std::vector<FileTypeFilter> getFiltersForType(FileType fileType) {
    // 定义文件类型到过滤器的映射
    static const std::map<FileType, std::vector<FileTypeFilter>> fileTypeFiltersMap = {
        { FileType::All, {
            {"所有文件", {"*.*"}}
        }},
        { FileType::Picture, {
            {"图片文件", {"*.png","*.jpg","*.jpeg","*.bmp","*.gif","*.tiff","*.webp"}}
        }},
        { FileType::Video, {
            {"视频文件", {"*.mp4","*.avi","*.mkv","*.mov","*.wmv","*.flv","*.webm"} }
        }},
        { FileType::Audio, {
            {"音频文件", {"*.mp3","*.wav","*.flac","*.ogg","*.aac","*.m4a"}}
        }},
        { FileType::NameFile, {
            // {"文档文件", "*.pdf;*.doc;*.docx;*.ppt;*.pptx;*.xls;*.xlsx"},
            {"文本文件", {"*.txt"}}
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
                
                button->SetText(Config::getInstance()->getBool(safeConfigName) ? "开" : "关");
                button->SetClickFunc([this, safeConfigName]{
                    try {
                        Config::getInstance()->toggleBool(safeConfigName);
                        button->SetText(Config::getInstance()->getBool(safeConfigName) ? "开" : "关");
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
            button->SetText("文本框");
            button->SetClickFunc([this]{
                // 处理文本框的点击事件
                checkActions();
            });
        }
        else if(item.type==SettingButtonType::ColorSelect){
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
                    button->SetFillColor(Config::getInstance()->getInt(configName));
                    button->SetEnableBitmap(false);
                    button->SetEnableFill(true);
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
                // 只显示文件名，不显示完整路径
                size_t pos = showText.find_last_of("/\\");
                if (pos != std::string::npos) {
                    showText = showText.substr(pos + 1);
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
                        Config::getInstance()->set(configName,path);
                        showText=path;
                        int i=0;
                        if(item.fileType==FileType::NameFile)
                            for(const auto& c:item.configName){
                                try{
                                    i= std::stoi(std::string(1, c));
                                }
                                catch (...) {
                                }
                                if(i>0&&i<10){
                                    break;
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
                // 只显示文件名，不显示完整路径
                size_t pos = showText.find_last_of("/\\");
                if (pos != std::string::npos) {
                    showText = showText.substr(pos + 1);
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
                        Config::getInstance()->set(configName,path);
                        button->SetText(Config::getInstance()->get(configName));
                        button->SetText(path);
                        button->SetEnableFill(false);
                        button->SetEnableBitmap(true);
                        button->SetBitmap(BitmapID::SettingButton);
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
            if(Debugging){
                Drawer::getInstance()->DrawSquare(TextRegion2,Color(255,0,0,255),false);
                Drawer::getInstance()->DrawSquare(TextRegion,Color(0,255,0,255),false);
            }
            font->RenderTextBetween(showText, TextRegion2, 0.35f, Color(255, 255, 255, alpha));
            font->RenderText(item.name,TextRegion.getx(),TextRegion.gety(),0.5f,Color(255,255,255,alpha));
        }
        else
            font->RenderTextBetween(item.name, TextRegion, 0.7f, Color(255, 255, 255, alpha));
    }
    if(button){
        button->Draw(alpha);
    }
    if(button2){
        button2->Draw(alpha);
    }
}

bool SettingButton::Click(Point point,int page) {
    if(page!=this->page)return false;
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
    
    // 调用 tinyfiledialogs 的颜色选择器
    const char* result = tinyfd_colorChooser(
        "选择颜色",  // 对话框标题
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
        // 保持 alpha 值不变
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
    
    // 调用 tinyfiledialogs 的文件夹选择对话框
    const char* selectedFolder = tinyfd_selectFolderDialog(
        "选择文件夹",       // 对话框标题
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
    
    // 在 tinyfiledialogs 中没有直接的打开文件功能
    // 使用平台特定的命令来打开文件
#ifdef _WIN32
    // Windows 平台
    std::string command = "start \"\" \"" + filePath + "\"";
    system(command.c_str());
#elif defined(__APPLE__)
    // macOS 平台
    std::string command = "open \"" + filePath + "\"";
    system(command.c_str());
#else
    // Linux 平台
    std::string command = "xdg-open \"" + filePath + "\"";
    system(command.c_str());
#endif
}

void SettingButton::checkActions(){
    if(item.type==SettingButtonType::Switch){

    }
    if(item.action&SettingButtonAction::Restart){
    }
}

void SettingButton::updateConfig() {
    
}