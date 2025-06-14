#include "core/Config.h"

#include "core/log.h"
#include <GLFW/glfw3.h>

void SetConfigItems(){
    core::Config* config = core::Config::getInstance();
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    int screenWidth=1920,screenHeight=1080;
    if(primary){
        const GLFWvidmode* mode =glfwGetVideoMode(primary);
        if(mode){
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
    config->setifno(OVERLAY1, "files/imgs/overlay1.webp");
    config->setifno(OVERLAY2, "files/imgs/overlay2.webp");
    config->setifno(OVERLAY3, "files/imgs/overlay3.webp");
    config->setifno(OVERLAY4, "files/imgs/overlay4.webp");
    
    config->setifno(NAME_COLOR, 0xFFFFFFFF);
    config->setifno(NAME_COLOR_6_STAR,0xFFFFFFFF);
    config->setifno(TEXT_COLOR, 0xFFFFFFFF);

    config->setifno(OFF_MUSIC,0);
    config->setifno(INWINDOW,1);
    config->setifno(OFF_VIDEO,0);
    config->setifno(USE_VIDEO_BACKGROUND, true);
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
    config->setifno(WINDOW_TITLE, "随机点名器");
    config->setifno(USE_FONT_COMPATIBILITY, 0);
    config->setifno(NO_VIDEO_PRELOAD, 0);

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

    config->setifno(DEBUG, 0);
    config->setifno(SHOW_FPS,0);
    config->setifno(USE_JSON_SETTINGS, 0);
    config->setifno(VOLUME, 50);
    
    core::Debugging=config->getBool(DEBUG);

    config->saveToFile();
}