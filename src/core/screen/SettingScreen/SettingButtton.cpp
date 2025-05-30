#include "core/screen/SettingScreen.h"

#include "core/Config.h"
#include <tinyfiledialogs/tinyfiledialogs.h>

using namespace core;
using namespace screen;

// 文件类型过滤器映射表实现
static std::vector<FileTypeFilter> getFiltersForType(FileType fileType) {
    // 定义文件类型到过滤器的映射
    static const std::map<FileType, std::vector<FileTypeFilter>> fileTypeFiltersMap = {
        { FileType::All, {
            {"所有文件", "*.*"}
        }},
        { FileType::Picture, {
            {"图片文件", "*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.tiff;*.webp;"}
        }},
        { FileType::Video, {
            {"视频文件", "*.mp4;*.avi;*.mkv;*.mov;*.wmv;*.flv;*.webm"}
        }},
        { FileType::Audio, {
            {"音频文件", "*.mp3;*.wav;*.flac;*.ogg;*.aac;*.m4a"}
        }},
        { FileType::NameFile, {
            // {"文档文件", "*.pdf;*.doc;*.docx;*.ppt;*.pptx;*.xls;*.xlsx"},
            {"文本文件", "*.txt"}
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
            button->SetText(Config::getInstance()->getBool(item.configName) ? "开" : "关");
            button->SetClickFunc([this]{
                Config::getInstance()->toggleBool(item.configName);
                button->SetText(Config::getInstance()->getBool(item.configName) ? "开" : "关");
                checkActions();
            });
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
            button->SetClickFunc([this]{
                // 处理颜色选择的点击事件
                Color color = selectColor();
                Config::getInstance()->set(item.configName,color);
                button->SetFillColor(Config::getInstance()->getInt(item.configName));
                button->SetEnableBitmap(false);
                button->SetEnableFill(true);
                checkActions();
            });
        }
        else if(item.type==SettingButtonType::FileSelect){
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
            button->SetClickFunc([this]{
                std::string path=selectFile();
                Config::getInstance()->set(item.configName,path);
                button->SetText(Config::getInstance()->get(item.configName));
                button->SetText(path);
                button->SetEnableFill(false);
                button->SetEnableBitmap(true);
                button->SetBitmap(BitmapID::SettingButton);
                checkActions();
            });
        }
        else if(item.type==SettingButtonType::PathSelect){
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
            button->SetClickFunc([this]{
                std::string path=selectPath();
                Config::getInstance()->set(item.configName,path);
                button->SetText(Config::getInstance()->get(item.configName));
                button->SetText(path);
                button->SetEnableFill(false);
                button->SetEnableBitmap(true);
                button->SetBitmap(BitmapID::SettingButton);
                checkActions();
            });
        }
}

void SettingButton::Draw(int currentPage, unsigned char alpha)const {
    if(currentPage!=page)return;
    if(!item.name.empty()){
        font->RenderTextBetween(item.name, TextRegion, 0.7f, Color(255, 255, 255, alpha));
    }
    if(button){
        button->Draw(alpha);
    }
    if(button2){
        button2->Draw(alpha);
    }
}

bool SettingButton::Click(Point point) {
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
    
    // 调用 tinyfiledialogs 的颜色选择器
    const char* result = tinyfd_colorChooser(
        "选择颜色",  // 对话框标题
        hexColor,    // 默认颜色（十六进制格式）
        NULL,        // 自定义颜色数组（可选）
        NULL         // 自定义颜色数组大小（可选）
    );
    
    // 如果用户选择了颜色（未取消）
    if (result != NULL) {
        unsigned int r, g, b;
        // 解析返回的十六进制颜色字符串
        if (sscanf(result, "#%02x%02x%02x", &r, &g, &b) == 3) {
            currentColor.r = static_cast<unsigned char>(r);
            currentColor.g = static_cast<unsigned char>(g);
            currentColor.b = static_cast<unsigned char>(b);
            // 保持 alpha 值不变
        }
    }
    
    return currentColor;
}

std::string SettingButton::selectFile() {
    std::string defaultPath = Config::getInstance()->get(item.configName);
    
    // 准备过滤器数组
    std::vector<const char*> filterPatterns;
    std::vector<const char*> filterDescriptions;
    
    for (const auto& filter : getFiltersForType(item.fileType)) {
        filterPatterns.push_back(filter.pattern.c_str());
        filterDescriptions.push_back(filter.description.c_str());
    }
    
    // 如果没有过滤器，添加默认的"所有文件"过滤器
    if (filterPatterns.empty()) {
        filterPatterns.push_back("*.*");
        filterDescriptions.push_back("所有文件");
    }
    
    // 调用文件选择对话框
    const char* selectedFile = tinyfd_openFileDialog(
        "选择文件",          
        defaultPath.c_str(), 
        static_cast<int>(filterPatterns.size()),
        filterPatterns.data(),
        "文件类型", // 这个参数在某些平台上会被忽略
        0
    );
    
    if (selectedFile != NULL) {
        return std::string(selectedFile);
    }
    
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
    if(item.action&SettingButtonAction::Restart){
    }
}

void SettingButton::updateConfig() {
    
}