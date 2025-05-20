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
    config->setifno(WINDOW_WIDTH, screenWidth/2);
    config->setifno(WINDOW_HEIGHT, screenHeight/2);
    config->setifno(VERTICAL_SYNC, 1);
    config->setifno(WINDOW_TITLE, "随机点名器");
    config->setifno(BACKGROUND_IMG, "files/imgs/background.png");
    config->setifno(POOL_COUNT, 4);
    config->setifno(MODE, 1);
    config->setifno(SPECIAL, 0);
    config->setifno(NAMES1, "files/names/1.txt");
    config->setifno(NAMES2, "files/names/2.txt");
    config->setifno(NAMES3, "files/names/3.txt");
    config->setifno(NAMES4, "files/names/4.txt");
    config->setifno(NAME_COLOR, 0xFFFFFFFF);
    config->setifno(OVERLAY1, "files/imgs/overlay1.png");
    config->setifno(OVERLAY2, "files/imgs/overlay2.png");
    config->setifno(OVERLAY3, "files/imgs/overlay3.png");
    config->setifno(OVERLAY4, "files/imgs/overlay4.png");
    config->setifno(FLOAT_WINDOW_WIDTH, screenHeight/10);
    config->setifno(FLOAT_WINDOW_HEIGHT, screenHeight/10);
    config->setifno(FLOAT_WINDOW_X, screenHeight*0.9);
    config->setifno(FLOAT_WINDOW_Y, screenWidth*0.9);
    config->setifno(FLOAT_WINDOW_IMG, "files/imgs/floatWindow.png");
    config->setifno(DEBUG, 0);
    config->setifno(TEXTR, 0);
    config->setifno(TEXTG, 0);
    config->setifno(TEXTB, 0);
    config->setifno(TEXTA, 255);
    config->setifno(USE_VIDEO_BACKGROUND, true);
    config->setifno(VIDEO_BACKGROUND_PATH, "files/video/bg.webm");
    config->setifno(CLICK_MUSIC, "files/music/click.mp3");
    core::Debugging=config->getBool(DEBUG);

    config->saveToFile();
}