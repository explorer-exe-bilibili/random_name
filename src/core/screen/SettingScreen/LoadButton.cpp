#include "core/screen/SettingScreen.h"
#include "core/log.h"
#include "core/Config.h"

#include "nlohmann/json.hpp"

using namespace screen;
using namespace core;


static nlohmann::json RollBack(std::string jsonpath) {
    Log << Level::Info << "开始回滚设置页面" << op::endl;
    
    // 创建JSON数据
    nlohmann::json j;
    nlohmann::json p;
    nlohmann::json i;
    
    // 图片页
    p["title"] = "图片";

    i["name"] = "卡池1图片";
    i["configName"] = OVERLAY1;
    i["fileChooseWindowName"] = "选择卡池1图片";
    i["fileType"] = FileType::Picture;
    i["number"] = 1;
    i["bitmapID"] = 0;
    p["item"].push_back(i); 
    i.clear();
    i["name"] = "卡池2图片";
    i["configName"] = OVERLAY2;
    i["fileChooseWindowName"] = "选择卡池2图片";
    i["fileType"] = FileType::Picture;
    i["number"] = 2;
    i["bitmapID"] = 1;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "卡池3图片";
    i["configName"] = OVERLAY3;
    i["fileChooseWindowName"] = "选择卡池3图片";
    i["fileType"] = FileType::Picture;
    i["number"] = 3;
    i["bitmapID"] = 2;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "卡池4图片";
    i["configName"] = OVERLAY4;
    i["fileChooseWindowName"] = "选择卡池4图片";
    i["fileType"] = FileType::Picture;
    i["number"] = 4;
    i["bitmapID"] = 3;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "图片移动的速度";
    i["configName"] = EXCHANGE_SPEED;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 1;
    i["maxCount"] = 10000;
    i["outOfLimitOutPut"] = "输入一个1-10000之间的数字";
    i["number"] = 11;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "默认卡池序号";
    i["configName"] = MODE;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 1;
    i["maxCount"] = 4;
    i["outOfLimitOutPut"] = "输入一个1-4之间的数字";
    i["number"] = 12;
    p["item"].push_back(i);
    i.clear();
    
    j["pages"].push_back(p);
    p.clear();
    
    // 名单与卡池页
    p["title"] = "名单与卡池";

    i["name"] = "卡池1名单";
    i["configName"] = NAMES1;
    i["fileChooseWindowName"] = "选择卡池1名单";
    i["fileType"] = FileType::NameFile;
    i["number"] = 1;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "卡池2名单";
    i["configName"] = NAMES2;
    i["fileChooseWindowName"] = "选择卡池2名单";
    i["fileType"] = FileType::NameFile;
    i["number"] = 2;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "卡池3名单";
    i["configName"] = NAMES3;
    i["fileChooseWindowName"] = "选择卡池3名单";
    i["fileType"] = FileType::NameFile;
    i["number"] = 3;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "卡池4名单";
    i["configName"] = NAMES4;
    i["fileChooseWindowName"] = "选择卡池4名单";
    i["fileType"] = FileType::NameFile;
    i["number"] = 4;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "抽卡时名字的颜色";
    i["configName"] = NAME_COLOR;
    i["type"] = screen::SettingButtonType::ColorSelect;
    i["number"] = 5;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "抽卡时6星名字的颜色";
    i["configName"] = NAME_COLOR_6_STAR;
    i["type"] = screen::SettingButtonType::ColorSelect;
    i["number"] = 6;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "抽卡时小名字的颜色";
    i["configName"] = TEXT_COLOR;
    i["type"] = screen::SettingButtonType::ColorSelect;
    i["number"] = 7;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "抽背卡池";
    i["configName"] = SPECIAL;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 0;
    i["maxCount"] = 4;
    i["outOfLimitOutPut"] = "输入一个0-4之间的数字（0表示禁用）";
    i["number"] = 11;
    p["item"].push_back(i);
    i.clear();
    
    j["pages"].push_back(p);
    p.clear();
    
    // 视频页
    p["title"] = "视频";
    i["name"] = "单发3星视频";
    i["configName"] = SIGNAL_3_STAR_VIDEO_PATH;
    i["fileChooseWindowName"] = "选择单发3星视频";
    i["fileType"] = FileType::Video;
    i["number"] = 1;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "单发4星视频";
    i["configName"] = SIGNAL_4_STAR_VIDEO_PATH;
    i["fileChooseWindowName"] = "选择单发4星视频";
    i["fileType"] = FileType::Video;
    i["number"] = 2;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "单发5星视频";
    i["configName"] = SIGNAL_5_STAR_VIDEO_PATH;
    i["fileChooseWindowName"] = "选择单发5星视频";
    i["fileType"] = FileType::Video;
    i["number"] = 3;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "十发4星视频";
    i["configName"] = GROUP_4_STAR_VIDEO_PATH;
    i["fileChooseWindowName"] = "选择十发4星视频";
    i["fileType"] = FileType::Video;
    i["number"] = 4;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "十发5星视频";
    i["configName"] = GROUP_5_STAR_VIDEO_PATH;
    i["fileChooseWindowName"] = "选择十发5星视频";
    i["fileType"] = FileType::Video;
    i["number"] = 5;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "关闭视频";
    i["configName"] = OFF_VIDEO;
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 11;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "关闭音乐";
    i["configName"] = OFF_MUSIC;
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 12;
    p["item"].push_back(i);
    i.clear();
    
    j["pages"].push_back(p);
    p.clear();
    
    // 悬浮窗页
    p["title"] = "悬浮窗";
    
    i["name"] = "悬浮窗";
    i["configName"] = FLOATWINDOW;
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 1;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "初始x坐标";
    i["configName"] = FLOAT_WINDOW_X;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 1;
    i["maxCount"] = screenInfo.width;
    i["outOfLimitOutPut"] = "大小不能大于屏幕";
    i["number"] = 2;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "初始y坐标";
    i["configName"] = FLOAT_WINDOW_Y;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 1;
    i["maxCount"] = screenInfo.height;
    i["outOfLimitOutPut"] = "大小不能大于屏幕";
    i["number"] = 3;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "宽度";
    i["configName"] = FLOAT_WINDOW_WIDTH;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 1;
    i["maxCount"] = screenInfo.width;
    i["outOfLimitOutPut"] = "大小不能大于屏幕";
    i["number"] = 4;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "高度";
    i["configName"] = FLOAT_WINDOW_HEIGHT;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 1;
    i["maxCount"] = screenInfo.height;
    i["outOfLimitOutPut"] = "大小不能大于屏幕";
    i["number"] = 5;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "滑动系数";
    i["configName"] = FLOAT_WINDOW_MU;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 0;
    i["maxCount"] = 1;
    i["outOfLimitOutPut"] = "大小不能超过1或小于0";
    i["number"] = 6;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "悬浮窗图片";
    i["configName"] = FLOAT_WINDOW_IMG;
    i["fileChooseWindowName"] = "选择悬浮窗图片";
    i["fileType"] = FileType::Picture;
    i["actions"]= screen::SettingButtonAction::Restart;
    i["number"] = 11;
    i["bitmapID"] = BitmapID::floatWindow;
    p["item"].push_back(i);
    i.clear();
    i["name"] = "悬浮窗图片透明度";
    i["configName"] = FLOAT_WINDOW_ALPHA;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::CountBetween;
    i["minCount"] = 0;
    i["maxCount"] = 255;
    i["outOfLimitOutPut"] = "大小不能超过255或小于0";
    i["number"] = 12;
    p["item"].push_back(i);
    i.clear();

    j["pages"].push_back(p);
    p.clear();
    
    // 杂项页
    p["title"] = "杂项";
    
    i["name"] = "窗口模式";
    i["configName"] = INWINDOW;
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 1;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "标题";
    i["configName"] = WINDOW_TITLE;
    i["type"] = screen::SettingButtonType::Textbox;
    i["actions"] = screen::SettingButtonAction::ResetWindowTitle;
    i["number"] = 2;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "设置页面使用json文件";
    i["configName"] = USE_JSON_SETTINGS;
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 3;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "字体兼容模式(出现字体错误再开)";
    i["configName"] = USE_FONT_COMPATIBILITY;
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 4;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "调试模式";
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 5;
    i["configName"] = DEBUG;
    i["actions"] = screen::SettingButtonAction::Restart;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "省内存模式(不预加载视频)";
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 6;
    i["configName"] = NO_VIDEO_PRELOAD;
    i["actions"] = screen::SettingButtonAction::Restart;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "图片资源目录";
    i["type"] = screen::SettingButtonType::Textbox;
    i["number"] = 7;
    i["configName"] = IMGS_PATH;
    i["actions"] = screen::SettingButtonAction::Restart;
    i["type"] = screen::SettingButtonType::PathSelect;
    p["item"].push_back(i);
    i.clear();
    
    i["name"] = "关闭动画";
    i["type"] = screen::SettingButtonType::Switch;
    i["number"] = 8;
    i["configName"] = NOSMOOTHUI;
    p["item"].push_back(i);
    i.clear();
    
    j["pages"].push_back(p);
    p.clear();
    
    // 写入JSON文件
    if (!Config::getInstance()->getBool(USE_JSON_SETTINGS, false)) {
        std::ofstream file(jsonpath);
        if (file.is_open()) {
            try {
                Log << Level::Info << "开始写入JSON设置项" << op::endl;
                file << j.dump(4); // 将JSON数据写入文件，4表示缩进4个空格
                file.close();
                Log << Level::Info << "JSON数据已写入到: " << jsonpath << op::endl;
            }
            catch (const std::exception& e) {
                Log << Level::Error << "写入JSON设置项时出错: " << e.what() << op::endl;
                return j;
            }
        }
        else {
            Log << Level::Error << "无法打开文件进行写入" << op::endl;
        }
    }
    
    return j;
}

void SettingScreen::loadButtons() {
    buttons.clear();
    titles.clear();
    nlohmann::json jsonData;
    if(Config::getInstance()->getBool(USE_JSON_SETTINGS,0)){
        std::ifstream file("settings.json");
        if (!file.is_open()) {
            Log << Level::Error << "无法打开设置文件" << op::endl;
            return;
        }
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error& e) {
            Log << Level::Error << "JSON解析错误: " << e.what() << op::endl;
            return;
        }
        file.close();
        if (jsonData.is_null()) {
            Log << Level::Error << "JSON数据为空" << op::endl;
            return;
        }
        if (!jsonData.is_array()) {
            Log << Level::Error << "JSON数据格式错误，应该是数组" << op::endl;
            return;
        }
    }
    else jsonData = RollBack("settings.json");
    if (jsonData.is_null()) {
        Log << Level::Error << "JSON数据为空" << op::endl;
        return;
    }
    int page = 0;
    for (const auto& item : jsonData) {
        for (const auto& button : item["item"]) {
            sItem item;
            item.name = button.value("name", "未知按钮");
            item.configName = button.value("configName", "Unknown");
            item.bitmapName = button.value("bitmapName", "");
            item.type = button.value("type", SettingButtonType::Switch);
            item.action = button.value("actions", SettingButtonAction::None);
            if(item.type==SettingButtonType::FileSelect){
                item.fileType = button.value("fileType", FileType::All);
                if(item.fileType==FileType::Audio){
                    item.audioID = button.value("audioID", "Unknown");
                }
                else if(item.fileType==FileType::Video){
                    item.videoID = button.value("videoID", core::VideoID::Unknown);
                }
                else if(item.fileType==FileType::Picture){
                    item.bitmapID = button.value("bitmapID", core::BitmapID::Unknown);
                }
                else if(item.fileType==FileType::Font){
                    item.fontID = button.value("fontID", core::FontID::Unknown);
                }
            }
            else if(item.action&SettingButtonAction::CountBetween){
                item.minCount = button.value("minCount", 0);
                item.maxCount = button.value("maxCount", 0);
                item.outOfLimitOutPut = button.value("outOfLimitOutPut", "");
            }
            int number=button.value("number", 0);
            buttons.emplace_back(item, number, page);
        }
        titles.push_back(item.value("title", "未知标题"));
        page++;
    }
}
