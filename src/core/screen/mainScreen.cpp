#include "core/screen/mainScreen.h"

#include "core/screen/SettingScreen.h"

#include "core/log.h"
#include "core/explorer.h"
#include "core/Config.h"

using namespace screen;
using namespace core;

#define SETICON "'"

enum{
    set=0,
    offVideo,
    his,
    x1,
    x10,
    exitButton,
    o1,
    o2,
    o3,
    o4
};

void MainScreen::init() {
    // 初始化按钮和背景
    mode=Config::getInstance()->getInt("mode", 0);
    buttons.clear();
    buttons.resize(10);
    for(int i = 0; i < buttons.size(); ++i) {
        buttons[i] = std::make_shared<Button>();
    }
    overlays.resize(Config::getInstance()->getInt(POOL_COUNT));
    for(int i = 0; i < overlays.size(); ++i) {
        overlays[i] = std::make_shared<Button>();
    }
    float x=0.2,tmp=(1-x*2)/Config::getInstance()->getInt(POOL_COUNT);
    for(int i=0; i< Config::getInstance()->getInt(POOL_COUNT); i++)
    {
        buttons[i+o1]->SetRegion({x,0.1,x+tmp*0.8,0.2});
        buttons[i+o1]->SetBitmap(BitmapID(int(BitmapID::Overlay0)+i));
        buttons[i+o1]->SetText("over"+std::to_string(i+1));
        buttons[i+o1]->SetEnableText(false);
        buttons[i+o1]->SetClickFunc([this, i] {
            mode = i;
            changeMode();
        });
        buttons[i+o1]->SetEnable(true);
        overlays[i]->SetBitmap(BitmapID(int(BitmapID::Overlay0)+i));
        overlays[i]->SetText("over"+std::to_string(i+1));
        overlays[i]->SetRegion({1,0.25,1.68,0.75});
        overlays[i]->SetEnableText(false);
        overlays[i]->SetEnable(true);
        x += tmp;
    }
    buttons[set]->SetEnableBitmap(false);
    buttons[set]->SetText(SETICON);
    buttons[set]->SetFontID(FontID::Icon);
    buttons[set]->SetRegion({0.1,0.82,0.12,0.86});
    buttons[set]->SetColor({211,188,142,255});
    buttons[set]->SetClickFunc([this] {
        Log << Level::Info << "进入设置界面" << op::endl;
        if(screens[ScreenID::Settings] == nullptr) {
            RegisterScreen(ScreenID::Settings, std::make_shared<screen::SettingScreen>());
        }
        Screen::SwitchToScreen(ScreenID::Settings);
    });
    buttons[set]->SetEnable(true);
    buttons[set]->SetFontScale(0.35f);
    background = core::Explorer::getInstance()->getBitmap(BitmapID::Background);

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
            videoBackground->close();
            videoBackground = nullptr;
        } catch (const std::exception& e) {
            Log << Level::Error << "清理视频播放器时发生异常: " << e.what() << op::endl;
        } catch (...) {
            Log << Level::Error << "清理视频播放器时发生未知异常" << op::endl;
        }
    }
    background = nullptr;
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
        o->Draw();
    }
}

void MainScreen::enter(int){
    // 在主屏幕进入时注册所有屏幕
    static bool screensRegistered = false;
    
    if (!screensRegistered) {
        // 注册主屏幕
        
        screensRegistered = true;
    }
    
    Log << Level::Info << "进入主屏幕" << op::endl;
}
