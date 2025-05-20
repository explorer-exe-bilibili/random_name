#include "core/screen/mainScreen.h"

#include "core/log.h"
#include "core/explorer.h"
#include "core/config.h"

using namespace screen;
using namespace core;

enum{
    o1=0,
    o2,
    o3,
    o4,
    set,
    offVideo,
    his,
    x1,
    x10,
    exitButton
};

void MainScreen::init() {
    // 初始化按钮和背景
    mode=Config::getInstance()->getInt("mode", 0);
    buttons.clear();
    buttons.resize(10);
    overlays.resize(Config::getInstance()->getInt(POOL_COUNT));
    for(int i = 0; i < overlays.size(); ++i) {
        overlays[i] = std::make_shared<Button>();
    }
    for(int i = 0; i < buttons.size(); ++i) {
        buttons[i] = std::make_shared<Button>();
    }
    float x=0.2,tmp=(1-x*2)/Config::getInstance()->getInt(POOL_COUNT);
    for(int i=0; i< Config::getInstance()->getInt(POOL_COUNT); i++)
    {
        buttons[i]->SetRegion({x,0.1,x+tmp*0.8,0.2});
        buttons[i]->SetBitmap("over"+std::to_string(i+1));
        buttons[i]->SetText("over"+std::to_string(i+1));
        buttons[i]->SetEnableText(false);
        buttons[i]->SetClickFunc([this, i] {
            mode = i;
            changeMode();
        });
        buttons[i]->SetEnable(true);
        overlays[i]->SetBitmap("over"+std::to_string(i+1));
        overlays[i]->SetText("over"+std::to_string(i+1));
        overlays[i]->SetRegion({1,0.25,1.68,0.75});
        overlays[i]->SetEnableText(false);
        overlays[i]->SetEnable(true);
        x += tmp;
    }

    background = core::Explorer::getInstance()->getBitmap("1");
    
    tmp=0;
    for(auto& b:overlays){
        int t=tmp-mode;
        b->MoveTo({0.16+t,0.25,0.84+t,0.75});
        tmp++;
    }
}

MainScreen::~MainScreen() {
    // 清理视频播放器资源
    if (videoBackground) {
        try {
            videoBackground->stop();
            videoBackground = nullptr;
        } catch (const std::exception& e) {
            Log << Level::Error << "清理视频播放器时发生异常: " << e.what() << op::endl;
        } catch (...) {
            Log << Level::Error << "清理视频播放器时发生未知异常" << op::endl;
        }
    }
}

bool MainScreen::Click(int x, int y) {
    // 调用基类的Click方法
    return Screen::Click(x, y);
}

void MainScreen::changeMode() {
    int tmp=0;
    for(auto& b:overlays){
        int t=tmp-mode;
        b->MoveTo({0.16+t,0.25,0.84+t,0.75},true,20,[&b,t]{
            Log<< "Overlay "<< t << op::endl;
            b->SetEnable(true);
        });
        tmp++;
    }
}

void MainScreen::Draw(){
    Screen::Draw();
    // 绘制覆盖层
    for(auto& o:overlays){
        o->Draw(100);
    }
}