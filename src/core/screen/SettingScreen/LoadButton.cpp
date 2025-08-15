#include "core/screen/SettingScreen.h"
#include "core/baseItem/lang.h"
#include "core/log.h"
#include "core/Config.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <iterator>

using namespace screen;
using namespace core;
using namespace settingScreen;
using namespace LanguageUtils;

#define PAGES "pages"
#define ITEM "item"
#define TITLE "title"
#define NAME "name"
#define CONFIG_NAME "configName"
#define BITMAP_NAME "bitmapName"
#define TYPE "type"
#define ACTIONS "actions"
#define FILE_TYPE "fileType"
#define AUDIO_ID "audioID"
#define VIDEO_ID "videoID"
#define BITMAP_ID "bitmapID"
#define FONT_ID "fontID"
#define MIN_COUNT "minCount"
#define MAX_COUNT "maxCount"
#define OUT_OF_LIMIT_OUTPUT "outOfLimitOutPut"
#define NUMBER "number"
#define SCREENID "screenID"
#define CHOICES "choices"

static nlohmann::json RollBack(std::string jsonpath) {
    Log << Level::Info << "开始回滚设置页面" << op::endl;
    
    // 创建JSON数据
    nlohmann::json j;
    nlohmann::json p;
    nlohmann::json i;
    
    // 图片页
    p[TITLE] = text("settings.page.images");

    i[NAME] = text("settings.item.pool1.image");
    i[CONFIG_NAME] = OVERLAY1;
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 1;
    i[BITMAP_ID] = BitmapID::Overlay0;
    p[ITEM].push_back(i); 
    i.clear();
    i[NAME] = text("settings.item.pool2.image");
    i[CONFIG_NAME] = OVERLAY2;
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 2;
    i[BITMAP_ID] = BitmapID::Overlay1;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.pool3.image");
    i[CONFIG_NAME] = OVERLAY3;
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 3;
    i[BITMAP_ID] = BitmapID::Overlay2;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.pool4.image");
    i[CONFIG_NAME] = OVERLAY4;
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 4;
    i[BITMAP_ID] = BitmapID::Overlay3;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.image.speed");
    i[CONFIG_NAME] = EXCHANGE_SPEED;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = 10000;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.image.speed");
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.default.pool.index");
    i[CONFIG_NAME] = MODE;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 3;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.default.pool");
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.use.video.background");
    i[CONFIG_NAME] = USE_VIDEO_BACKGROUND;
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 13;
    p[ITEM].push_back(i);
    i.clear();

    j[PAGES].push_back(p);
    p.clear();
    
    // 名单与卡池页
    p[TITLE] = text("settings.page.names");

    i[NAME] = text("settings.item.pool1.names");
    i[CONFIG_NAME] = NAMES1;
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.pool2.names");
    i[CONFIG_NAME] = NAMES2;
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.pool3.names");
    i[CONFIG_NAME] = NAMES3;
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.pool4.names");
    i[CONFIG_NAME] = NAMES4;
    i[FILE_TYPE] = FileType::NameFile;
    i[TYPE] = SettingButtonType::FileSelect;
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.name.color");
    i[CONFIG_NAME] = NAME_COLOR;
    i[TYPE] = SettingButtonType::ColorSelect;
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.name6.color");
    i[CONFIG_NAME] = NAME_COLOR_6_STAR;
    i[TYPE] = SettingButtonType::ColorSelect;
    i[NUMBER] = 6;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.small.name.color");
    i[CONFIG_NAME] = TEXT_COLOR;
    i[TYPE] = SettingButtonType::ColorSelect;
    i[NUMBER] = 7;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.special.pool");
    i[CONFIG_NAME] = SPECIAL;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 4;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.special");
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();
    
    j[PAGES].push_back(p);
    p.clear();
    
    // 视频页
    p[TITLE] = text("settings.page.video");
    i[NAME] = text("settings.item.signal3.video");
    i[CONFIG_NAME] = SIGNAL_3_STAR_VIDEO_PATH;
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Signal3star;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.signal4.video");
    i[CONFIG_NAME] = SIGNAL_4_STAR_VIDEO_PATH;
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Signal4star;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.signal5.video");
    i[CONFIG_NAME] = SIGNAL_5_STAR_VIDEO_PATH;
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Signal5star;
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.group4.video");
    i[CONFIG_NAME] = GROUP_4_STAR_VIDEO_PATH;
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Multi4star;
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.group5.video");
    i[CONFIG_NAME] = GROUP_5_STAR_VIDEO_PATH;
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = SettingButtonType::FileSelect;
    i[VIDEO_ID]= VideoID::Multi5star;
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.video.background");
    i[CONFIG_NAME] = BACKGROUND_VIDEO_PATH;
    i[FILE_TYPE] = FileType::Video;
    i[TYPE] = SettingButtonType::FileSelect;
    i[ACTIONS] = SettingButtonAction::Restart;
    i[NUMBER] = 6;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.video.off");
    i[CONFIG_NAME] = OFF_VIDEO;
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.music.off");
    i[CONFIG_NAME] = OFF_MUSIC;
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();
    
    j[PAGES].push_back(p);
    p.clear();
    /*
    // 悬浮窗页
    p[TITLE] = text("settings.page.floatwindow");
    
    i[NAME] = text("settings.item.floatwindow");
    i[CONFIG_NAME] = FLOATWINDOW;
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.floatwindow.init.x");
    i[CONFIG_NAME] = FLOAT_WINDOW_X;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.width;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.size");
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.floatwindow.init.y");
    i[CONFIG_NAME] = FLOAT_WINDOW_Y;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.height;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.size");
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.floatwindow.width");
    i[CONFIG_NAME] = FLOAT_WINDOW_WIDTH;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.width;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.size");
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.floatwindow.height");
    i[CONFIG_NAME] = FLOAT_WINDOW_HEIGHT;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 1;
    i[MAX_COUNT] = screenInfo.height;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.size");
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.floatwindow.mu");
    i[CONFIG_NAME] = FLOAT_WINDOW_MU;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 1;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.mu");
    i[NUMBER] = 6;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.floatwindow.image");
    i[CONFIG_NAME] = FLOAT_WINDOW_IMG;
    i[FILE_CHOOSE_WINDOW_NAME] = text("settings.item.floatwindow.choose");
    i[FILE_TYPE] = FileType::Picture;
    i[TYPE] = SettingButtonType::FileSelect;
    i[ACTIONS]= SettingButtonAction::Restart;
    i[NUMBER] = 11;
    i[BITMAP_ID] = BitmapID::floatWindow;
    p[ITEM].push_back(i);
    i.clear();
    i[NAME] = text("settings.item.floatwindow.alpha");
    i[CONFIG_NAME] = FLOAT_WINDOW_ALPHA;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 255;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.alpha");
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();

    j[PAGES].push_back(p);
    p.clear();
    */
    // 外观页
    p[TITLE] = text("settings.page.appearance");
    
    i[NAME] = text("settings.item.window.mode");
    i[CONFIG_NAME] = INWINDOW;
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.window.title");
    i[CONFIG_NAME] = WINDOW_TITLE;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::ResetWindowTitle;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.vsync");
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 3;
    i[CONFIG_NAME] = VERTICAL_SYNC;
    i[ACTIONS] = SettingButtonAction::Restart;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.window.width");
    i[CONFIG_NAME] = WINDOW_WIDTH;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 100;
    i[MAX_COUNT] = 16384;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.window.size");
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.window.height");
    i[CONFIG_NAME] = WINDOW_HEIGHT;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 100;
    i[MAX_COUNT] = 16384;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.window.size");
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.window.x");
    i[CONFIG_NAME] = WINDOW_X;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = screenInfo.width;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.screen.width");
    i[NUMBER] = 13;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.window.y");
    i[CONFIG_NAME] = WINDOW_Y;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = screenInfo.height;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.screen.height");
    i[NUMBER] = 14;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.font.text");
    i[CONFIG_NAME] = TEXT_FONT_PATH;
    i[FILE_TYPE] = FileType::Font;
    i[TYPE] = SettingButtonType::FileSelect;
    i[FONT_ID] = FontID::Normal;
    i[NUMBER] = 6;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.font.icon");
    i[CONFIG_NAME] = ICON_FONT_PATH;
    i[FILE_TYPE] = FileType::Font;
    i[TYPE] = SettingButtonType::FileSelect;
    i[FONT_ID] = FontID::Icon;
    i[NUMBER] = 7;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.font.name");
    i[CONFIG_NAME] = NAME_FONT_PATH;
    i[FILE_TYPE] = FileType::Font;
    i[TYPE] = SettingButtonType::FileSelect;
    i[FONT_ID] = FontID::Name;
    i[NUMBER] = 16;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.font.default");
    i[CONFIG_NAME] = DEFAULT_FONT_PATH;
    i[FILE_TYPE] = FileType::Font;
    i[TYPE] = SettingButtonType::FileSelect;
    i[FONT_ID] = FontID::Default;
    i[NUMBER] = 17;
    p[ITEM].push_back(i);
    i.clear();

    j[PAGES].push_back(p);
    p.clear();

    // 音频页
    p[TITLE] = text("settings.page.audio");
    i[NAME] = text("settings.item.audio.click");
    i[CONFIG_NAME] = CLICK_MUSIC_PATH;
    i[FILE_TYPE] = FileType::Sound;
    i[TYPE] = SettingButtonType::FileSelect;
    i[AUDIO_ID] = core::AudioID::click;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.audio.enter");
    i[CONFIG_NAME] = ENTER_MUSIC_PATH;
    i[FILE_TYPE] = FileType::Sound;
    i[TYPE] = SettingButtonType::FileSelect;
    i[AUDIO_ID] = core::AudioID::enter;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.audio.bgm");
    i[CONFIG_NAME] = BGM_PATH;
    i[FILE_TYPE] = FileType::Audio;
    i[TYPE] = SettingButtonType::FileSelect;
    i[AUDIO_ID] = core::AudioID::bgm;
    i[NUMBER] = 3;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.audio.star3");
    i[CONFIG_NAME] = STAR_3_MUSIC_PATH;
    i[FILE_TYPE] = FileType::Audio;
    i[TYPE] = SettingButtonType::FileSelect;
    i[AUDIO_ID] = core::AudioID::star3;
    i[NUMBER] = 4;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.audio.star4");
    i[CONFIG_NAME] = STAR_4_MUSIC_PATH;
    i[FILE_TYPE] = FileType::Audio;
    i[TYPE] = SettingButtonType::FileSelect;
    i[AUDIO_ID] = core::AudioID::star4;
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.audio.star5");
    i[CONFIG_NAME] = STAR_5_MUSIC_PATH;
    i[FILE_TYPE] = FileType::Audio;
    i[TYPE] = SettingButtonType::FileSelect;
    i[AUDIO_ID] = core::AudioID::star5;
    i[NUMBER] = 6;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.audio.starfull");
    i[CONFIG_NAME] = STAR_FULL_MUSIC_PATH;
    i[FILE_TYPE] = FileType::Audio;
    i[TYPE] = SettingButtonType::FileSelect;
    i[AUDIO_ID] = core::AudioID::starfull;
    i[NUMBER] = 7;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.audio.volume");
    i[CONFIG_NAME] = VOLUME;
    i[TYPE] = SettingButtonType::Textbox;
    i[ACTIONS] = SettingButtonAction::CountBetween;
    i[MIN_COUNT] = 0;
    i[MAX_COUNT] = 100;
    i[OUT_OF_LIMIT_OUTPUT] = text("settings.outoflimit.volume");
    i[NUMBER] = 11;
    
    p[ITEM].push_back(i);
    i.clear();

    j[PAGES].push_back(p);
    p.clear();

    p[TITLE] = text("settings.page.layout");

    i[NAME] = text("settings.item.layout.main");
    i[TYPE] = SettingButtonType::RegionEditor;
    i[SCREENID] = ScreenID::MainMenu;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.layout.settings");
    i[TYPE] = SettingButtonType::RegionEditor;
    i[SCREENID] = ScreenID::Settings;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.layout.name");
    i[TYPE] = SettingButtonType::RegionEditor;
    i[SCREENID] = ScreenID::Name;
    i[NUMBER] = 11;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.layout.list");
    i[TYPE] = SettingButtonType::RegionEditor;
    i[SCREENID] = ScreenID::ListName;
    i[NUMBER] = 12;
    p[ITEM].push_back(i);
    i.clear();

    j[PAGES].push_back(p);
    p.clear();

    // 杂项页
    p[TITLE] = text("settings.page.misc");

    i[NAME] = text("settings.item.use_json_settings");
    i[CONFIG_NAME] = USE_JSON_SETTINGS;
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 1;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.font_compatibility");
    i[CONFIG_NAME] = USE_FONT_COMPATIBILITY;
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 2;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.debug");
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 3;
    i[CONFIG_NAME] = DEBUG;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.show_fps");
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 4;
    i[CONFIG_NAME] = SHOW_FPS;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.imgs_path");
    i[CONFIG_NAME] = IMGS_PATH;
    i[TYPE] = SettingButtonType::PathSelect;
    i[NUMBER] = 5;
    p[ITEM].push_back(i);
    i.clear();
    
    i[NAME] = text("settings.item.nosmoothui");
    i[TYPE] = SettingButtonType::Switch;
    i[NUMBER] = 6;
    i[CONFIG_NAME] = NOSMOOTHUI;
    p[ITEM].push_back(i);
    i.clear();

    i[NAME] = text("settings.item.language");
    i[TYPE] = SettingButtonType::Choose;
    i[NUMBER] = 11;
    i[CONFIG_NAME] = LANG;
    i[CHOICES] = { "en-US", "zh-CN", "ja-JP", "zh-TW" };
    i[ACTIONS] = SettingButtonAction::Restart;
    p[ITEM].push_back(i);
    i.clear();

    j[PAGES].push_back(p);
    p.clear();
    // 写入JSON文件
    if (bools[boolconfig::use_json_settings]) {
        std::ofstream file("files/settings.json", std::ios::out | std::ios::trunc);
        if (file.is_open()) {
            try {
                Log << Level::Info << "开始写入JSON设置项" << op::endl;
                file << j.dump(4); // 将JSON数据写入文件，4表示缩进4个空格
                file.close();
                Log << Level::Info << "JSON数据已写入到: " << "files/settings.json" << op::endl;
            }
            catch (const std::exception& e) {
                Log << Level::Error << "写入JSON设置项时出错: " << e.what() << op::endl;
                file.close();
                return j;
            }
        }
        else {
            Log << Level::Error << "无法打开文件进行写入: files/settings.json" << op::endl;
        }
    }
    
    return j;
}

void SettingScreen::loadButtons() {
    static bool loaded=false;
    if(loaded)return;
    loaded=true;
    s_buttons.clear();
    titles.clear();
    nlohmann::json jsonData;
    if(bools[boolconfig::use_json_settings]){
        std::ifstream file("files/settings.json", std::ios::in);
        if (!file.is_open()) {
            Log << Level::Info << "设置文件不存在，创建默认设置" << op::endl;
            jsonData = RollBack("files/settings.json");
        }
        else {
            try {
                // 读取文件内容为字符串，避免编码问题
                std::string content((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
                file.close();
                
                if (content.empty()) {
                    Log << Level::Info << "设置文件为空，创建默认设置" << op::endl;
                    jsonData = RollBack("files/settings.json");
                }
                else {
                    jsonData = nlohmann::json::parse(content);
                }
            } catch (const nlohmann::json::parse_error& e) {
                Log << Level::Error << "JSON解析错误: " << e.what() << op::endl;
                jsonData = RollBack("files/settings.json");
            } catch (const std::exception& e) {
                Log << Level::Error << "读取设置文件时出错: " << e.what() << op::endl;
                jsonData = RollBack("files/settings.json");
            }
        }
        
        if (jsonData.is_null()) {
            Log << Level::Info << "JSON数据为空，创建默认设置" << op::endl;
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
            titles.push_back(item.value(TITLE, text("settings.unknown.title")));
            for (const auto& button : item[ITEM]) {
                sItem item;
                item.name = button.value(NAME, text("settings.unknown.button"));
                item.configName = button.value(CONFIG_NAME, "Unknown");
                item.bitmapName = button.value(BITMAP_NAME, "");
                item.type = button.value(TYPE, SettingButtonType::Switch);
                item.action = button.value(ACTIONS, SettingButtonAction::None);
                if(item.type==SettingButtonType::FileSelect){
                    item.fileType = button.value(FILE_TYPE, FileType::All);
                    if(item.fileType==FileType::Audio|| item.fileType==FileType::Sound){
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
                    else if(item.fileType==FileType::NameFile){}
                }
                else if(item.action&SettingButtonAction::CountBetween){
                    item.minCount = button.value(MIN_COUNT, 0);
                    item.maxCount = button.value(MAX_COUNT, 0);
                    item.outOfLimitOutPut = button.value(OUT_OF_LIMIT_OUTPUT, "");
                }
                else if(item.type==SettingButtonType::RegionEditor){
                    item.screenID = button.value(SCREENID, screen::ScreenID::Unknown);
                }
                else if(item.type==SettingButtonType::Choose){
                    if(button.contains(CHOICES) && button[CHOICES].is_array()){
                        for(const auto& choice : button[CHOICES]){
                            if(choice.is_string()){
                                item.chooses.push_back(choice.get<std::string>());
                            }
                        }
                    }
                }
                int number=button.value(NUMBER, 0);
                s_buttons.emplace_back(std::make_shared<SettingButton>(item, number, page));
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