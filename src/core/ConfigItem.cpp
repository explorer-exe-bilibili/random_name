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
    config->setifno("WindowWidth", screenWidth/2);
    config->setifno("WindowHeight", screenHeight/2);
    config->setifno("VerticalSync", 1);
    config->setifno("WindowTitle", "随机点名器");
    config->setifno("BackgroundIMG", "files/imgs/background.png");
    config->setifno("overLay1", "files/imgs/overlay1.png");
    config->setifno("overLay2", "files/imgs/overlay2.png");
    config->setifno("overLay3", "files/imgs/overlay3.png");
    config->setifno("overLay4", "files/imgs/overlay4.png");
    config->setifno("floatWindowWidth", screenHeight/10);
    config->setifno("floatWindowHeight", screenHeight/10);
    config->setifno("floatWindowX", screenHeight*0.9);
    config->setifno("floatWindowY", screenWidth*0.9);
    config->setifno("floatWindowIMG", "files/imgs/floatWindow.png");
    config->setifno("debugMode",0);
    core::Debugging=config->getBool("debugMode");
}