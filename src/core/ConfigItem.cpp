#include "core/Config.h"

#include "core/log.h"
#include <glfw/glfw3.h>

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
    config->setifno(NAMES1, "files/names/1.txt");
    config->setifno(NAMES2, "files/names/2.txt");
    config->setifno(NAMES3, "files/names/3.txt");
    config->setifno(NAMES4, "files/names/4.txt");
    config->setifno(OVERLAY1, "files/imgs/overlay1.png");
    config->setifno(OVERLAY2, "files/imgs/overlay2.png");
    config->setifno(OVERLAY3, "files/imgs/overlay3.png");
    config->setifno(OVERLAY4, "files/imgs/overlay4.png");
    
    config->setifno(NAME_COLOR, 0xFFFFFFFF);
    config->setifno(NAME_COLOR_6_STAR,0xFFFFFFFF);
    config->setifno(TEXTR, 255);
    config->setifno(TEXTG, 255);
    config->setifno(TEXTB, 255);
    config->setifno(TEXTA, 255);

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
    config->setifno(FLOAT_WINDOW_IMG, "files/imgs/floatWindow.png");

    config->setifno(WINDOW_WIDTH, screenWidth/2);
    config->setifno(WINDOW_HEIGHT, screenHeight/2);
    config->setifno(WINDOW_X,0);
    config->setifno(WINDOW_Y,0);
    config->setifno(VERTICAL_SYNC, 1);
    config->setifno(WINDOW_TITLE, "随机点名器");

    config->setifno(BACKGROUND_IMG_PATH, "files/imgs/background.png");
    config->setifno(IMGS_PATH,"files/imgs/");
    config->setifno(VIDEO_BACKGROUND_PATH,"files/video/bg.webm");
    config->setifno(DEFAULT_FONT_PATH,"files/fonts/spare.ttf");
    config->setifno(ICON_FONT_PATH,"files/fonts/icon.ttf");
    config->setifno(CLICK_MUSIC_PATH, "files/music/click.mp3");
    config->setifno(ENTER_MUSIC_PATH,"files/music/enter.mp3");
    config->setifno(BACKGROUND_MUSIC_PATH,"files/music/bgm.mp3");

    config->setifno(DEBUG, 0);
    config->setifno(SHOW_FPS,0);
    
    core::Debugging=config->getBool(DEBUG);

    config->saveToFile();
}