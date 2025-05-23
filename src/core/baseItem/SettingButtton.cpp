#include "core/screen/SettingScreen.h"

#include "core/config.h"
#include <tinyfiledialogs/tinyfiledialogs.h>

using namespace core;
using namespace screen;

SettingButton::SettingButton(const std::string& name, SettingButtonType type, int number, int page)
    : name(name), type(type), page(page) {
        font=core::Explorer::getInstance()->getFont(FontID::Normal);
        Region Button2Region;
        if(number<=10){
            TextRegion={0,number*0.08+0.1,0.4,number*0.08+0.15};
            if(type!=SettingButtonType::PathSelect)
                ButtonRegion={0.41,number*0.08+0.1,0.5,number*0.08+0.15};
            else{
                ButtonRegion={0.41,number*0.08+0.1,0.455,number*0.08+0.15};
                Button2Region={0.455,number*0.08+0.1,0.5,number*0.08+0.15};
            }
        }else{
            number-=10;
            TextRegion={0.51,number*0.08+0.1,0.82,number*0.08+0.15};
            if(type!=SettingButtonType::PathSelect)
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
        if(type==SettingButtonType::Switch){
            button->SetText(Config::getInstance()->getBool(ConfigName) ? "开" : "关");
            button->SetClickFunc([this]{
                Config::getInstance()->toggleBool(ConfigName);
                button->SetText(Config::getInstance()->getBool(ConfigName) ? "开" : "关");
                checkActions();
            });
        }
        else if(type==SettingButtonType::Textbox){
            button->SetText("文本框");
            button->SetClickFunc([this]{
                // 处理文本框的点击事件
                checkActions();
            });
        }
        else if(type==SettingButtonType::ColorSelect){
            button->SetText("选择颜色");
            button->SetClickFunc([this]{
                // 处理颜色选择的点击事件
                Color color = selectColor();
                Config::getInstance()->set(ConfigName,color);
                button->SetFillColor(Config::getInstance()->getInt(ConfigName));
                button->SetEnableBitmap(false);
                button->SetEnableFill(true);
                checkActions();
            });
        }
        else if(type==SettingButtonType::PathSelect){
            button2=std::make_shared<core::Button>(Button());
            button2->SetRegion(Button2Region);
            button2->SetBitmap(BitmapID::SettingButton);
            button2->SetText("打开");
            button2->SetEnableBitmap(true);
            button2->SetEnableText(true);
            button2->SetEnableFill(false);
            button2->SetClickFunc([this]{
                openFile();
            });
            button->SetText("选择");
            button->SetClickFunc([this]{
                std::string path=selectPath();
                Config::getInstance()->set(ConfigName,path);
                button->SetText(Config::getInstance()->get(ConfigName));
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
    if(!name.empty()){
        font->RenderTextBetween(name, TextRegion, 0.7f, Color(255, 255, 255, alpha));
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
    Color currentColor = Config::getInstance()->getInt(ConfigName);
    
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

std::string SettingButton::selectPath(const std::vector<FileTypeFilter>& filters) {
    // 获取当前配置的路径作为初始路径
    std::string defaultPath = Config::getInstance()->get(ConfigName);
    
    // 准备文件过滤器
    std::vector<const char*> filterPatterns;
    std::string filterDescription;
    
    if (filters.empty()) {
        // 如果没有指定过滤器，则使用默认的"所有文件"
        const char* defaultFilter[] = { "*.*" };
        const char* selectedFile = tinyfd_openFileDialog(
            "选择文件",          // 对话框标题
            defaultPath.c_str(), // 初始目录
            1,                   // 过滤器数量
            defaultFilter,       // 过滤器模式
            "所有文件",          // 过滤器描述
            0                    // 是否允许多选 (0 = 否)
        );
        
        if (selectedFile != NULL) {
            return std::string(selectedFile);
        }
    } else {
        // 如果指定了过滤器，就使用它们
        // 首先收集所有过滤器的模式字符串
        for (const auto& filter : filters) {
            filterPatterns.push_back(filter.pattern.c_str());
            
            if (!filterDescription.empty()) {
                filterDescription += ";";
            }
            filterDescription += filter.description;
        }
        
        // 调用文件选择对话框
        const char* selectedFile = tinyfd_openFileDialog(
            "选择文件",          // 对话框标题
            defaultPath.c_str(), // 初始目录
            static_cast<int>(filterPatterns.size()), // 过滤器数量
            filterPatterns.data(),                   // 过滤器模式数组
            filterDescription.c_str(),               // 过滤器描述
            0                                        // 是否允许多选 (0 = 否)
        );
        
        if (selectedFile != NULL) {
            return std::string(selectedFile);
        }
    }
    
    // 如果用户取消了选择，返回原始路径
    return defaultPath;
}

void SettingButton::openFile() {
    std::string filePath = Config::getInstance()->get(ConfigName);
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
    if(actions&SettingButtonAction::Restart){

    }
}