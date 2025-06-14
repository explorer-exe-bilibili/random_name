#include "core/screen/SettingScreen.h"
#include "core/log.h"
#include "core/Config.h"

#include "nlohmann/json.hpp"

using namespace screen;
using namespace core;

#define PAGES "pages"
#define ITEM "item"
#define TITLE "title"
#define NAME "name"
#define CONFIG_NAME "configName"
#define BITMAP_NAME "bitmapName"
#define TYPE "type"
#define ACTIONS "actions"
#define FILE_TYPE "fileType"
#define FILE_CHOOSE_WINDOW_NAME "fileChooseWindowName"
#define AUDIO_ID "audioID"
#define VIDEO_ID "videoID"
#define BITMAP_ID "bitmapID"
#define FONT_ID "fontID"
#define MIN_COUNT "minCount"
#define MAX_COUNT "maxCount"
#define OUT_OF_LIMIT_OUTPUT "outOfLimitOutPut"
#define NUMBER "number"

static nlohmann::json RollBack(std::string jsonpath) {
    Log << Level::Info << "开始回滚设置页面" << op::endl;
    
    // 创建JSON数据
    nlohmann::json j;
    nlohmann::json p;
    nlohmann::json i;
    
    // 图片页
    p[TITLE] = "图片";

    i[NAME] = "卡池1图片";
    i[CONFIG_NAME] = OVERLAY1;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池1图片";
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 1;
    i[BITMAP_ID] = BitmapID::Overlay0;
    p[ITEM].push_back(i); 
    i.clear();
    i[NAME] = "卡池2图片";
    i[CONFIG_NAME] = OVERLAY2;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池2图片";
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 2;
    i[BITMAP_ID] = BitmapID::Overlay1;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "卡池3图片";
    i[CONFIG_NAME] = OVERLAY3;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池3图片";
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 3;
    i[BITMAP_ID] = BitmapID::Overlay2;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "卡池4图片";
    i[CONFIG_NAME] = OVERLAY4;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池4图片";
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 4;
    i[BITMAP_ID] = BitmapID::Overlay3;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = "图片移动的速度";
    i[CONFIG_NAME] = EXCHANGE_SPEED;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = 10000;
    i[OUT_OF_LIMIT_OUTPUT] = "输入一个1-10000之间的数字";
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = "默认卡池序号";
    i[CONFIG_NAME] = MODE;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = 4;
    i[OUT_OF_LIMIT_OUTPUT] = "输入一个1-4之间的数字";
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();
    
    j[PAGES].push_back(p);
    p.clear();
    
    // 名单与卡池页
    p[TITLE] = "名单与卡池";

    i[NAME] = "卡池1名单";
    i[CONFIG_NAME] = NAMES1;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池1名单";
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "卡池2名单";
    i[CONFIG_NAME] = NAMES2;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池2名单";
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "卡池3名单";
    i[CONFIG_NAME] = NAMES3;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池3名单";
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "卡池4名单";
    i[CONFIG_NAME] = NAMES4;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择卡池4名单";
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "抽卡时名字的颜色";
    i[CONFIG_NAME] = NAME_COLOR;
    i[TYPE] = screen::SettingButtonType::ColorSelect;
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "抽卡时6星名字的颜色";
    i[CONFIG_NAME] = NAME_COLOR_6_STAR;
    i[TYPE] = screen::SettingButtonType::ColorSelect;
    i[NUMBER] = 6;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "抽卡时小名字的颜色";
    i[CONFIG_NAME] = TEXT_COLOR;
    i[TYPE] = screen::SettingButtonType::ColorSelect;
    i[NUMBER] = 7;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "抽背卡池";
    i[CONFIG_NAME] = SPECIAL;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 4;
    i[OUT_OF_LIMIT_OUTPUT] = "输入一个0-4之间的数字（0表示禁用）";
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();
    
    j[PAGES].push_back(p);
    p.clear();
    
    // 视频页
    p[TITLE] = "视频";
    i[NAME] = "单发3星视频";
    i[CONFIG_NAME] = SIGNAL_3_STAR_VIDEO_PATH;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择单发3星视频";
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Signal3star;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "单发4星视频";
    i[CONFIG_NAME] = SIGNAL_4_STAR_VIDEO_PATH;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择单发4星视频";
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Signal4star;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "单发5星视频";
    i[CONFIG_NAME] = SIGNAL_5_STAR_VIDEO_PATH;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择单发5星视频";
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Signal5star;
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "十发4星视频";
    i[CONFIG_NAME] = GROUP_4_STAR_VIDEO_PATH;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择十发4星视频";
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Multi4star;
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "十发5星视频";
    i[CONFIG_NAME] = GROUP_5_STAR_VIDEO_PATH;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择十发5星视频";
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Multi5star;
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "关闭视频";
    i[CONFIG_NAME] = OFF_VIDEO;
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "关闭音乐";
    i[CONFIG_NAME] = OFF_MUSIC;
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();
    
    j[PAGES].push_back(p);
    p.clear();
    
    // 悬浮窗页
    p[TITLE] = "悬浮窗";
    
    i[NAME] = "悬浮窗";
    i[CONFIG_NAME] = FLOATWINDOW;
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "初始x坐标";
    i[CONFIG_NAME] = FLOAT_WINDOW_X;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.width;
    i[OUT_OF_LIMIT_OUTPUT] = "大小不能大于屏幕";
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "初始y坐标";
    i[CONFIG_NAME] = FLOAT_WINDOW_Y;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.height;
    i[OUT_OF_LIMIT_OUTPUT] = "大小不能大于屏幕";
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "宽度";
    i[CONFIG_NAME] = FLOAT_WINDOW_WIDTH;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.width;
    i[OUT_OF_LIMIT_OUTPUT] = "大小不能大于屏幕";
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "高度";
    i[CONFIG_NAME] = FLOAT_WINDOW_HEIGHT;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.height;
    i[OUT_OF_LIMIT_OUTPUT] = "大小不能大于屏幕";
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "滑动系数";
    i[CONFIG_NAME] = FLOAT_WINDOW_MU;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 1;
    i[OUT_OF_LIMIT_OUTPUT] = "大小不能超过1或小于0";
    i[NUMBER] = 6;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "悬浮窗图片";
    i[CONFIG_NAME] = FLOAT_WINDOW_IMG;
    i[FILE_CHOOSE_WINDOW_NAME] = "选择悬浮窗图片";
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = screen::SettingButtonType::FileSelect;
    i[ACTIONS]= screen::SettingButtonAction::Restart;
    i[NUMBER] = 11;
    i[BITMAP_ID] = BitmapID::floatWindow;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "悬浮窗图片透明度";
    i[CONFIG_NAME] = FLOAT_WINDOW_ALPHA;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 255;
    i[OUT_OF_LIMIT_OUTPUT] = "大小不能超过255或小于0";
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();

    j[PAGES].push_back(p);
    p.clear();
    
    // 杂项页
    p[TITLE] = "杂项";
    
    i[NAME] = "窗口模式";
    i[CONFIG_NAME] = INWINDOW;
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "标题";
    i[CONFIG_NAME] = WINDOW_TITLE;
    i[TYPE] = screen::SettingButtonType::Textbox;
    i[ACTIONS] = screen::SettingButtonAction::ResetWindowTitle;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "设置页面使用json文件";
    i[CONFIG_NAME] = USE_JSON_SETTINGS;
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "字体兼容模式(出现字体错误再开)";
    i[CONFIG_NAME] = USE_FONT_COMPATIBILITY;
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "调试模式";
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 5;
    i[CONFIG_NAME] = DEBUG;
    i[ACTIONS] = screen::SettingButtonAction::Restart;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "省内存模式(不预加载视频)";
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 6;
    i[CONFIG_NAME] = NO_VIDEO_PRELOAD;
    i[ACTIONS] = screen::SettingButtonAction::Restart;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = "图片资源目录";
    i[CONFIG_NAME] = IMGS_PATH;
    i[TYPE] = screen::SettingButtonType::PathSelect;
    i[ACTIONS] = screen::SettingButtonAction::Restart;
    i[NUMBER] = 7;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = "关闭动画";
    i[TYPE] = screen::SettingButtonType::Switch;
    i[NUMBER] = 8;
    i[CONFIG_NAME] = NOSMOOTHUI;
    p[ITEM].push_back(i);
    i.clear();
    
    j[PAGES].push_back(p);
    p.clear();
    
    // 写入JSON文件
    if (Config::getInstance()->getBool(USE_JSON_SETTINGS, false)) {
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
    s_buttons.clear();
    titles.clear();
    nlohmann::json jsonData;
    if(Config::getInstance()->getBool(USE_JSON_SETTINGS,0)){
        std::ifstream file("files/settings.json");
        if (!file.is_open()) {
            Log << Level::Error << "无法打开设置文件" << op::endl;
            jsonData = RollBack("files/settings.json");
        }
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error& e) {
            Log << Level::Error << "JSON解析错误: " << e.what() << op::endl;
            jsonData = RollBack("files/settings.json");
        }
        file.close();
        if (jsonData.is_null()) {
            Log << Level::Error << "JSON数据为空" << op::endl;
            jsonData = RollBack("files/settings.json");
        }        if (!jsonData.contains("pages") || !jsonData["pages"].is_array()) {
            Log << Level::Error << "JSON数据格式错误，应该包含pages数组" << op::endl;
            jsonData = RollBack("files/settings.json");
        }
    }
    else jsonData = RollBack("files/settings.json");
    int page = 0;
    try{
        for (const auto& item : jsonData[PAGES]) {
            titles.push_back(item.value(TITLE, "未知标题"));
            for (const auto& button : item[ITEM]) {
                sItem item;
                item.name = button.value(NAME, "未知按钮");
                item.configName = button.value(CONFIG_NAME, "Unknown");
                item.bitmapName = button.value(BITMAP_NAME, "");
                item.type = button.value(TYPE, SettingButtonType::Switch);
                item.action = button.value(ACTIONS, SettingButtonAction::None);
                if(item.type==SettingButtonType::FileSelect){
                    item.fileType = button.value(FILE_TYPE, FileType::All);
                    item.fileChooseWindowName = button.value(FILE_CHOOSE_WINDOW_NAME, "选择文件");
                    if(item.fileType==FileType::Audio){
                        item.audioID = button.value(AUDIO_ID, core::AudioID::Unknown);
                    }
                    else if(item.fileType==FileType::Video){
                        item.videoID = button.value(VIDEO_ID, core::VideoID::Unknown);
                    }
                    else if(item.fileType==FileType::Picture){
                        item.bitmapID = button.value(BITMAP_ID, core::BitmapID::Unknown);
                    }
                    else if(item.fileType==FileType::Font){
                        item.fontID = button.value(FONT_ID, core::FontID::Unknown);
                    }
                }
                else if(item.action&SettingButtonAction::CountBetween){
                    item.minCount = button.value(MIN_COUNT, 0);
                    item.maxCount = button.value(MAX_COUNT, 0);
                    item.outOfLimitOutPut = button.value(OUT_OF_LIMIT_OUTPUT, "");
                }
                int number=button.value(NUMBER, 0);
                s_buttons.push_back(std::make_shared<SettingButton>(item, number, page));
            }
            page++;
        }
    }

    catch(std::exception& e){
        Log << Level::Error << "加载设置按钮时出错: " << e.what() << op::endl;
        return;
    }
    this->pages=page-1;
}