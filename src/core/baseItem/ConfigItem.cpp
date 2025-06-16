#include "core/Config.h"
#include "core/configItem.h"
#include "core/log.h"
#include "core/baseItem/Base.h"
#include "core/explorer.h"
#include "core/screen/base.h"
#include "core/screen/nameScreen.h"
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

// 实现bools映射
std::map<boolconfig, bool> bools;

// 从配置名字符串映射到boolconfig枚举
boolconfig GetBoolConfigFromString(const std::string& configName) {
    if (configName == OFF_MUSIC) return boolconfig::offmusic;
    if (configName == INWINDOW) return boolconfig::inwindow;
    if (configName == OFF_VIDEO) return boolconfig::off_video;
    if (configName == USE_VIDEO_BACKGROUND) return boolconfig::use_video_background;
    if (configName == NOSMOOTHUI) return boolconfig::nosmoothui;
    if (configName == FLOATWINDOW) return boolconfig::floatwindow;
    if (configName == USE_FONT_COMPATIBILITY) return boolconfig::use_font_compatibility;
    if (configName == NO_VIDEO_PRELOAD) return boolconfig::no_video_preload;
    if (configName == DEBUG) return boolconfig::debug;
    if (configName == SHOW_FPS) return boolconfig::show_fps;
    if (configName == USE_JSON_SETTINGS) return boolconfig::use_json_settings;
    // 默认返回debug
    return boolconfig::debug;
}

// 从Config加载bool值到bools映射
void LoadBoolsFromConfig() {
    core::Config* config = core::Config::getInstance();
    bools[boolconfig::offmusic] = config->getBool(OFF_MUSIC, false);
    bools[boolconfig::inwindow] = config->getBool(INWINDOW, true);
    bools[boolconfig::off_video] = config->getBool(OFF_VIDEO, false);
    bools[boolconfig::use_video_background] = config->getBool(USE_VIDEO_BACKGROUND, true);
    bools[boolconfig::nosmoothui] = config->getBool(NOSMOOTHUI, false);
    bools[boolconfig::floatwindow] = config->getBool(FLOATWINDOW, true);
    bools[boolconfig::use_font_compatibility] = config->getBool(USE_FONT_COMPATIBILITY, false);
    bools[boolconfig::no_video_preload] = config->getBool(NO_VIDEO_PRELOAD, false);
    bools[boolconfig::debug] = config->getBool(DEBUG, false);
    bools[boolconfig::show_fps] = config->getBool(SHOW_FPS, false);
    bools[boolconfig::use_json_settings] = config->getBool(USE_JSON_SETTINGS, false);
}

void fullscreen(GLFWwindow* window){
    if(!window) return;
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if(!primary) {
        Log << Level::Error << "Failed to get primary monitor for fullscreen" << op::endl;
        return;
    }
    // 更新窗口信息
    core::WindowInfo.width = core::screenInfo.width;
    core::WindowInfo.height = core::screenInfo.height;
    core::WindowInfo.aspectRatio = static_cast<float>(core::screenInfo.width) / core::screenInfo.height;
#ifdef _WIN32
    //用winapi取消全屏独占
    HWND hwnd = glfwGetWin32Window(window);
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_POPUP);
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
    glfwSetWindowMonitor(window, primary, 0, 0, core::screenInfo.width, core::screenInfo.height, GLFW_DONT_CARE);
    // 刷新窗口
    glfwSwapBuffers(window);
    Log << Level::Info << "Switched to fullscreen mode: " << core::screenInfo.width << "x" << core::screenInfo.height << "@" << GLFW_DONT_CARE << "Hz" << op::endl;
}

void defullscreen(GLFWwindow* window){
    if(!window) return;
    core::WindowInfo.width = core::Config::getInstance()->getInt(WINDOW_WIDTH, core::screenInfo.width / 2);
    core::WindowInfo.height = core::Config::getInstance()->getInt(WINDOW_HEIGHT, core::screenInfo.height / 2);
    int x = core::Config::getInstance()->getInt(WINDOW_X, 0);
    int y = core::Config::getInstance()->getInt(WINDOW_Y, 0);
    // 恢复窗口到原来的大小和位置
    glfwSetWindowMonitor(window, nullptr, x, y, core::WindowInfo.width, core::WindowInfo.height, GLFW_DONT_CARE);
}

// 将bools映射的值同步到Config
void SyncConfig() {
    core::Config* config = core::Config::getInstance();
    config->set(OFF_MUSIC, bools[boolconfig::offmusic]);
    config->set(INWINDOW, bools[boolconfig::inwindow]);
    config->set(OFF_VIDEO, bools[boolconfig::off_video]);
    config->set(USE_VIDEO_BACKGROUND, bools[boolconfig::use_video_background]);
    config->set(NOSMOOTHUI, bools[boolconfig::nosmoothui]);
    config->set(FLOATWINDOW, bools[boolconfig::floatwindow]);
    config->set(USE_FONT_COMPATIBILITY, bools[boolconfig::use_font_compatibility]);
    config->set(NO_VIDEO_PRELOAD, bools[boolconfig::no_video_preload]);
    config->set(DEBUG, bools[boolconfig::debug]);
    config->set(SHOW_FPS, bools[boolconfig::show_fps]);
    config->set(USE_JSON_SETTINGS, bools[boolconfig::use_json_settings]);
    config->saveToFile();
    // 设置全屏
    if(!bools[boolconfig::inwindow])fullscreen(core::WindowInfo.window);
    else defullscreen(core::WindowInfo.window);
    if(bools[boolconfig::offmusic])core::Explorer::getInstance()->getAudio()->stopAll();
    else {
        if(!core::Explorer::getInstance()->getAudio()->isMusicPlaying())
            core::Explorer::getInstance()->playAudio(core::AudioID::bgm, -1);
    }
    screen::Screen::setUseVideoBackground(bools[boolconfig::use_video_background]);
    screen::NameButton::setColor(core::Config::getInstance()->getInt(NAME_COLOR, core::Color(200,200,200,255)));
    screen::NameButton::set6Color(core::Config::getInstance()->getInt(NAME_COLOR_6_STAR, core::Color(200,200,200,255)));
    screen::NameScreen::setSmallNameColor(core::Config::getInstance()->getInt(TEXT_COLOR, core::Color(250,250,250,255)));
    core::Explorer::getInstance()->getAudio()->setMusicVolume(core::Config::getInstance()->getInt(VOLUME, 100));
    core::Explorer::getInstance()->getAudio()->setSoundVolume(core::AudioIDToString(core::AudioID::click),core::Config::getInstance()->getInt(VOLUME, 100));
    core::Explorer::getInstance()->getAudio()->setSoundVolume(core::AudioIDToString(core::AudioID::enter),core::Config::getInstance()->getInt(VOLUME, 100));
}

void SetConfigItems(){
    core::Config* config = core::Config::getInstance();
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    int screenWidth=1920,screenHeight=1080;
    if(primary){
        const GLFWvidmode* mode =glfwGetVideoMode(primary);
        if(mode){
            core::screenInfo.width=mode->width;
            core::screenInfo.height=mode->height;
            screenHeight=mode->height;
            screenWidth=mode->width;
        }
    }
    else Log<<"Failed to get screen size"<<op::endl;
    config->setScreenSize(screenWidth,screenHeight);
    config->setifno(POOL_COUNT, 4);
    config->setifno(MODE, 1);
    config->setifno(SPECIAL, 0);
    config->setifno(EXCHANGE_SPEED, 100);
    config->setifno(NAMES1, "files/names/1.txt");
    config->setifno(NAMES2, "files/names/2.txt");
    config->setifno(NAMES3, "files/names/3.txt");
    config->setifno(NAMES4, "files/names/4.txt");
    config->setifno(OVERLAY1, "files/imgs/overlay0.webp");
    config->setifno(OVERLAY2, "files/imgs/overlay1.webp");
    config->setifno(OVERLAY3, "files/imgs/overlay2.webp");
    config->setifno(OVERLAY4, "files/imgs/overlay3.webp");
    
    config->setifno(NAME_COLOR, core::Color(200,200,200,255));
    config->setifno(NAME_COLOR_6_STAR,core::Color(200,200,200,255));
    config->setifno(TEXT_COLOR, core::Color(250,250,250,255));

    config->setifno(OFF_MUSIC,0);
    config->setifno(INWINDOW,1);
    config->setifno(OFF_VIDEO,0);
    config->setifno(USE_VIDEO_BACKGROUND, true); //TODO
    config->setifno(NOSMOOTHUI,0);

    config->setifno(FLOATWINDOW,true);
    config->setifno(FLOAT_WINDOW_WIDTH, screenHeight/10);
    config->setifno(FLOAT_WINDOW_HEIGHT, screenHeight/10);
    config->setifno(FLOAT_WINDOW_X, screenHeight*0.9);
    config->setifno(FLOAT_WINDOW_Y, screenWidth*0.9);
    config->setifno(FLOAT_WINDOW_IMG, "files/imgs/floatWindow.webp");
    config->setifno(FLOAT_WINDOW_ALPHA, 0.5);
    config->setifno(FLOAT_WINDOW_MU, 0.5);

    config->setifno(WINDOW_WIDTH, screenWidth/2);
    config->setifno(WINDOW_HEIGHT, screenHeight/2);
    config->setifno(WINDOW_X,0);
    config->setifno(WINDOW_Y,0);
    config->setifno(VERTICAL_SYNC, 1);
    config->setifno(WINDOW_TITLE, "祈愿");
    config->setifno(USE_FONT_COMPATIBILITY, 0);
    config->setifno(NO_VIDEO_PRELOAD, 0);//TODO

    config->setifno(BACKGROUND_IMG_PATH, "files/imgs/background.webp");
    config->setifno(IMGS_PATH,"files/imgs/");

    config->setifno(VIDEO_BACKGROUND_PATH,"files/video/bg.webm");
    config->setifno(SIGNAL_3_STAR_VIDEO_PATH,"files/video/signal-3star.webm");
    config->setifno(SIGNAL_4_STAR_VIDEO_PATH,"files/video/signal-4star.webm");
    config->setifno(SIGNAL_5_STAR_VIDEO_PATH,"files/video/signal-5star.webm");
    config->setifno(GROUP_4_STAR_VIDEO_PATH,"files/video/group-4star.webm");

    config->setifno(GROUP_5_STAR_VIDEO_PATH,"files/video/group-5star.webm");
    config->setifno(DEFAULT_FONT_PATH,"files/fonts/spare.ttf");
    config->setifno(ICON_FONT_PATH,"files/fonts/icon.ttf");
    config->setifno(NAME_FONT_PATH,"files/fonts/QS.ttf");
    config->setifno(TEXT_FONT_PATH,"files/fonts/text.ttf");
    config->setifno(CLICK_MUSIC_PATH, "files/music/click.mp3");
    config->setifno(ENTER_MUSIC_PATH,"files/music/enter.mp3");
    config->setifno(BACKGROUND_MUSIC_PATH,"files/music/bgm.mp3");
    config->setifno(STAR_3_MUSIC_PATH,"files/music/star3.mp3");
    config->setifno(STAR_4_MUSIC_PATH,"files/music/star4.mp3");
    config->setifno(STAR_5_MUSIC_PATH,"files/music/star5.mp3");
    config->setifno(STAR_FULL_MUSIC_PATH,"files/music/starfull.mp3");

    config->setifno(DEBUG, 0);
    config->setifno(SHOW_FPS,0);
    config->setifno(USE_JSON_SETTINGS, 0);
    config->setifno(VOLUME, 100);
    
    // 在设置完配置后，加载bools映射
    LoadBoolsFromConfig();
    
    config->saveToFile();
}