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
    o1,
    o2,
    o3,
    o4
};

void MainScreen::init() {
    // 初始化按钮和背景
    mode=Config::getInstance()->getInt("mode");
    buttons.resize(9);
    for(int i = 0; i < buttons.size(); ++i) {
        buttons[i] = std::make_shared<Button>();
    }
    overlays.resize(Config::getInstance()->getInt(POOL_COUNT));
    for(int i = 0; i < overlays.size(); ++i) {
        overlays[i] = std::make_shared<Button>();
    }
    for(int i=0; i< buttons.size()-o1; i++)
    {
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
        overlays[i]->SetEnableText(false);
        overlays[i]->SetEnable(true);
    }
    buttons[set]->SetEnableBitmap(false);
    buttons[set]->SetText(SETICON);
    buttons[set]->SetFontID(FontID::Icon);
    buttons[set]->SetRegionStr(UI_REGION_MAINMENU_SETTING);
    buttons[set]->SetColor({211,188,142,255});
    buttons[set]->SetClickFunc([this] {
        Log << Level::Info << "进入设置界面" << op::endl;
        if(screens[ScreenID::Settings] == nullptr) {
            RegisterScreen(ScreenID::Settings, std::make_shared<screen::SettingScreen>());
        }
        Screen::SwitchToScreenWithFade(ScreenID::Settings, 0, 0.5f);
    });
    buttons[set]->SetEnable(true);
    buttons[set]->SetFontScale(0.35f);
    buttons[offVideo]->SetBitmap(BitmapID::MainScreenButton);
    buttons[offVideo]->SetClickFunc([this]{
        bools[boolconfig::off_video] = !bools[boolconfig::off_video];
        buttons[offVideo]->SetText(bools[boolconfig::off_video] ? "视频:关" : "视频:开");
        SyncConfig(); // 同步到配置文件
    });
    buttons[offVideo]->SetEnable(true);
    buttons[offVideo]->SetFontScale(0.25f);
    buttons[offVideo]->SetText(bools[boolconfig::off_video] ? "视频:关" : "视频:开");
    buttons[offVideo]->SetColor({0,0,0,255});
    buttons[offVideo]->SetFontID(FontID::Normal);
    buttons[offVideo]->SetRegionStr(UI_REGION_MAINMENU_OFFVIDEO);
    buttons[x1]->SetEnableText(false);
    buttons[x10]->SetEnableText(false);
    buttons[x1]->SetRegionStr(UI_REGION_MAINMENU_1STAR);
    buttons[x10]->SetRegionStr(UI_REGION_MAINMENU_10STAR);
    if(mode<=3){
        buttons[x1]->SetBitmap(BitmapID::Pink1Button);
        buttons[x10]->SetBitmap(BitmapID::Pink10Button);
    }else{
        buttons[x1]->SetBitmap(BitmapID::Blue1Button);
        buttons[x10]->SetBitmap(BitmapID::Blue10Button);
    }
    buttons[x1]->SetClickFunc([this]{
        for(int i=0; i< buttons.size(); i++)
        {
            buttons[i]->FadeOut(1,255,0);
        }
        overlays[mode]->FadeOut(1,255,0,[this]{
            SwitchToScreen(ScreenID::Video,10+mode);
        });
    });
    buttons[x10]->SetClickFunc([this]{
        for(int i=0; i< buttons.size(); i++)
        {
            buttons[i]->FadeOut(1,255,0);
        }
        overlays[mode]->FadeOut(1,255,0,[this]{
            SwitchToScreen(ScreenID::Video,20+mode);
        });
    });
    background = core::Explorer::getInstance()->getBitmapPtr(BitmapID::Background);

    int tmp=0;
    Region o_region(Config::getInstance()->getRegion(UI_REGION_MAINMENU_OVERLAY));
    for(auto& b:overlays){
        int t=tmp-mode;
        b->MoveTo({o_region.getOriginX()+t,o_region.getOriginY(),o_region.getOriginXEnd()+t,o_region.getOriginYEnd(),o_region.getRatio()});
        tmp++;
    }
    reloadButtonsRegion();
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
    background = nullptr;
}

bool MainScreen::Click(int x, int y) {
    // 调用基类的Click方法
    return Screen::Click(x, y);
}

void MainScreen::changeMode() {
    int tmp=0;
    Region o_region(Config::getInstance()->getRegion(UI_REGION_MAINMENU_OVERLAY));
    for(auto& b:overlays){
        int t=tmp-mode;
        b->MoveTo({o_region.getOriginX()+t,o_region.getOriginY(),o_region.getOriginXEnd()+t,o_region.getOriginYEnd(),o_region.getRatio()}
            ,true,Config::getInstance()->getInt(EXCHANGE_SPEED)
        ,[&b,t]{
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
        o->Draw(getCurrentAlpha()*255.0f);
    }
}

void MainScreen::enter(int){
    exitButton->SetClickFunc([]{core::quit();});
    if(bools[boolconfig::use_video_background] && videoBackground) {
        videoBackground->resume();
    }
    Log << Level::Info << "进入主屏幕" << op::endl;
}

void MainScreen::reloadButtonsRegion() {
    for(auto& b:buttons)b->resetRegion();
    Region o_region(Config::getInstance()->getRegion(UI_REGION_MAINMENU_OVERLAY));
    int tmp=0;
    for(auto& b:overlays){
        int t=tmp-mode;
        b->MoveTo({o_region.getOriginX()+t,o_region.getOriginY(),o_region.getOriginXEnd()+t,o_region.getOriginYEnd(),o_region.getRatio()});
        tmp++;
    }
    Region region(Config::getInstance()->getRegion(UI_REGION_MAINMENU_CHECK));
    float x=region.getOriginX();
    float tmp2=(region.getOriginXEnd()-x)/(buttons.size()-o1);
    for(int i=0;i<buttons.size()-o1;i++){
        buttons[i+o1]->SetRegion({x,region.getOriginY(),x+tmp2*0.8,region.getOriginYEnd(),region.getRatio()});
        x+=tmp2;
    }
}