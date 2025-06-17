#include "core/screen/nameScreen.h"

#include "core/log.h"
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Font.h"
#include "core/Config.h"
#include "core/Drawer.h"
#include <exception>

using namespace screen;
using namespace core;

#define STAR L"E"

core::Color NameButton::color = core::Color(255, 255, 255, 255);
core::Color NameButton::star6Color = core::Color(255, 255, 0, 255);
core::Color NameScreen::SmallNameColor = core::Color(250, 250, 250, 255);

static int nameCount=0;
static unsigned int enterTime=0;

enum ButtonID{
    SkipButton = 0,
    AddNameButton,
    TypeButton
};

void NameScreen::init() {
    background=Explorer::getInstance()->getBitmap(BitmapID::NameBg);
    if(!background) {
        Log << Level::Warn << "NameScreen: Failed to load background bitmap." << op::endl;
    }
    rng = std::mt19937(std::random_device{}());
    dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    StarFont = Explorer::getInstance()->getFont(FontID::Icon);
    NameFont = Explorer::getInstance()->getFont(FontID::Name);
    if (!StarFont || !NameFont) {
        Log << Level::Error << "NameScreen: Failed to load fonts." << op::endl;
    }
    buttons.resize(3);
    for(int i = 0; i < buttons.size(); ++i) {
        buttons[i] = std::make_shared<core::Button>();
    }
    starPositions.clear();
    buttons[SkipButton]->SetRegion({0.82,0.045,0.88,0.08});
    buttons[SkipButton]->SetText("跳过>>");
    buttons[SkipButton]->SetFontScale(0.3f);
    buttons[SkipButton]->SetFontID(FontID::Normal);
    buttons[SkipButton]->SetAudioID(AudioID::enter);
    buttons[SkipButton]->SetClickFunc([this] {
        Log << Level::Info << "跳过名字输入，进入列表" << op::endl;
        SwitchToScreenWithFade(ScreenID::ListName);
        core::Explorer::getInstance()->getAudio()->stopAll();
    });
    buttons[SkipButton]->SetColor({255, 255, 255, 255});
    buttons[SkipButton]->SetEnableText(true);
    buttons[SkipButton]->SetEnableBitmap(false);
    buttons[SkipButton]->SetEnableFill(false);
    buttons[SkipButton]->SetEnable(false);
    buttons[AddNameButton]->SetRegion({0.35,0.8,0.65,0.836});
    buttons[AddNameButton]->SetText("增加此名字几率");
    buttons[AddNameButton]->SetFontScale(0.3f);
    buttons[AddNameButton]->SetFontID(FontID::Normal);
    buttons[AddNameButton]->SetClickFunc([this] {
        NameRandomer::getInstance(currentName.mode)->addName(currentName);
    });
    buttons[AddNameButton]->SetColor({255, 230, 34, 255});
    buttons[AddNameButton]->SetEnableText(true);
    buttons[AddNameButton]->SetEnableBitmap(false);
    buttons[AddNameButton]->SetEnableFill(false);
    buttons[AddNameButton]->SetEnable(false);
    buttons[TypeButton]->SetRegion({0.2,0.25,0.6,-1});
    if(bools[boolconfig::use_font_compatibility]) nameButton.SetFontID(FontID::Default);
    else nameButton.SetFontID(FontID::Name);
}

void NameScreen::Draw() {
    updateTransition();
    float alpha = getCurrentAlpha();
    if(background)
    background->Draw({0, 0, 1, 1}, alpha);
    for(auto& b: buttons)b->Draw(alpha*255);
    {std::lock_guard<std::mutex> lock(starPositionsMutex);
        for(auto s: starPositions)
            StarFont->RenderChar('E', s.getx(), s.gety(), 0.2f, Color(220,184,14,255));}
    Point pos(0.177, (8.0/13.0));
    Explorer::getInstance()->getFont(FontID::Normal)->RenderText(currentName.name, pos.getx(), pos.gety(), 0.3f, SmallNameColor);
    nameButton.Draw();
}

void NameScreen::enter(int times) {
    currentIndex=0;
    enterTime++;
    nameCount=times;
    if(times>1)buttons[SkipButton]->SetEnable(true);
    else buttons[SkipButton]->SetEnable(false);
    if(nameItems[0].mode==Config::getInstance()->getInt(SPECIAL,0)-1)buttons[AddNameButton]->SetEnable(true);
    else buttons[AddNameButton]->SetEnable(false);
    changeName();
}

bool NameScreen::Click(int x,int y){    
    for(auto& b: buttons){
        if(b->OnClick(Point(x, y,false)))
        return true;
    }
    changeName();
    return true;
}

void NameScreen::PaintStars() const {
    int index=currentIndex;
    unsigned int nowEnterTime=enterTime;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    if(currentName.star<=5){
        for(int i = 0; i < currentName.star; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if(index!= currentIndex)return;
            if(nowEnterTime != enterTime)return;
            if(Screen::getCurrentScreen()->getID() != ScreenID::Name)return;
            Point pos(0.177+0.01*i, 31.0f/52.0f);
            {
                std::lock_guard<std::mutex> lock(starPositionsMutex);
                starPositions.emplace_back(pos);
            }
        }
    } else {
        for(int i=0;i<750;i++){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if(index!= currentIndex)return;
            if(nowEnterTime != enterTime)return;
            if(Screen::getCurrentScreen()->getID() != ScreenID::Name)return;
            Point pos(dist(rng),dist(rng));
            {
                std::lock_guard<std::mutex> lock(starPositionsMutex);
                starPositions.emplace_back(pos);
            }
        }
    }
}

void NameScreen::changeName() {
    core::Explorer::getInstance()->getAudio()->stopAll();
    if(currentIndex>=nameItems.size()){
        if(nameCount==1){
            Log << Level::Info << "所有名字已显示完毕" << op::endl;
            core::Explorer::getInstance()->playAudio(AudioID::bgm, 0);
            SwitchToScreenWithFade(ScreenID::MainMenu);
            return;
        }
        SwitchToScreenWithFade(ScreenID::ListName);
        return;
    }
    currentIndex++;
    currentName=nameItems[currentIndex-1];
    {
        std::lock_guard<std::mutex> lock(starPositionsMutex);
        starPositions.clear();
    }
    if(currentName.mode==Config::getInstance()->getInt(SPECIAL,0)-1)buttons[AddNameButton]->SetEnable(true);
    else buttons[AddNameButton]->SetEnable(false);
    if(nameCount>1)buttons[SkipButton]->SetEnable(true);
    else buttons[SkipButton]->SetEnable(false);
    switch (currentName.type)
    {
    case NameType::bow:buttons[TypeButton]->SetBitmap(BitmapID::Weapon_bowBg);break;
    case NameType::sword:buttons[TypeButton]->SetBitmap(BitmapID::Weapon_swordBg);break;
    case NameType::polearm:buttons[TypeButton]->SetBitmap(BitmapID::Weapon_polearmBg);break;
    case NameType::catalyst:buttons[TypeButton]->SetBitmap(BitmapID::Weapon_catalystBg);break;
    case NameType::claymore:buttons[TypeButton]->SetBitmap(BitmapID::Weapon_claymoreBg);break;
    default:break;
    }
    buttons[TypeButton]->SetEnable(false);
    buttons[TypeButton]->SetRegion({0.2,0.25,0.6,-1});    nameButton.SetRegion({0,-0.15,1,1.15});
    nameButton.SetName(currentName);
    int nowIndex=currentIndex;
    if(bools[boolconfig::nosmoothui]){
        nameButton.SetRegion({0.3,0.3,0.7,0.7});
        if(currentName.type!=NameType::Unknow){
            if(buttons[TypeButton]) {
                buttons[TypeButton]->SetEnable(true);
                buttons[TypeButton]->MoveTo({0.3,0.25,0.7,-1},true,8);
            }
        }
    }
    else{
        // 使用try-catch保护MoveTo调用
        try {
            nameButton.MoveTo({0.2,0.3,0.6,0.7},true,10,[nowIndex,this]{
                // 检查对象是否仍然有效
                if(nowIndex!=currentIndex)return;
                if(Screen::getCurrentScreen()->getID() != ScreenID::Name)return;
                
                try {
                    nameButton.MoveTo({0.3,0.3,0.7,0.7},true,8);
                    if(currentName.type!=NameType::Unknow){
                        if(buttons[TypeButton]) {
                            buttons[TypeButton]->SetEnable(true);
                            buttons[TypeButton]->MoveTo({0.3,0.25,0.7,-1},true,8);
                        }
                    }
                } catch (const std::exception& e) {
                    Log << Level::Error << "NameScreen::changeName - 内部动画执行异常: " << e.what() << op::endl;
                }
            });
        } catch (const std::exception& e) {
            Log << Level::Error << "NameScreen::changeName - 外部动画执行异常: " << e.what() << op::endl;
        }
    }
    // 使用异步方式启动星星绘制，避免阻塞
    std::thread([this]{
        try {
            PaintStars();
        } catch (const std::exception& e) {
            Log << Level::Error << "NameScreen::PaintStars - 异常: " << e.what() << op::endl;
        }
    }).detach();
    if(currentName.star==3)core::Explorer::getInstance()->playAudio(AudioID::star3);
    else if(currentName.star==4)core::Explorer::getInstance()->playAudio(AudioID::star4);
    else if(currentName.star==5)core::Explorer::getInstance()->playAudio(AudioID::star5);
    else core::Explorer::getInstance()->playAudio(AudioID::starfull);
    Log << Level::Info << "当前名字: " << currentName.name << ", 星级: " << currentName.star << ", 类型: " << static_cast<int>(currentName.type) << op::endl;
}

void NameButton::Draw(unsigned char alpha) {
    if(!font)return;
    int i=0;
    for(auto&r: regions) 
        r.setFatherRegion(region);
        if(text.length()<=regions.size()) for(const auto& c: text){
        if(bools[boolconfig::debug]){
            Drawer::getInstance()->DrawSquare({regions[i].getx(), regions[i].gety(), regions[i].getxend(), regions[i].getyend(),false},Color(255,0,0,255),false);
            Drawer::getInstance()->DrawSquare(region,Color(255,0,255,255),false);
        }
        if(starCount<6)
            font->RenderCharFitRegion(c, regions[i].getx(), regions[i].gety(), regions[i].getxend(), regions[i].getyend(), fontScale, color);
        else
            font->RenderCharFitRegion(c, regions[i].getx(), regions[i].gety()
            ,regions[i].getxend(), regions[i].getyend(), star6Color);
        i++;
    }
    else {
        font->RenderTextCentered(text, region, fontScale, color);
    }
}

void NameButton::SetName(const core::NameEntry& name) {
    text = core::string2wstring(name.name);
    starCount=name.star;
    regions.clear();
    switch (text.length())
    {
    case 1:
        regions.emplace_back(0,0,1,1);
        break;
    case 2:
        regions.emplace_back(0,0.25,0.5,0.75);
        regions.emplace_back(0.5,0.25,1,0.75);
        break;
    case 3:
        regions.emplace_back(0.25,0,0.75,0.5);
        regions.emplace_back(0,0.5,0.5,1);
        regions.emplace_back(0.5,0.5,1,1);
        break;
    case 4:
        regions.emplace_back(0,0,0.5,0.5);
        regions.emplace_back(0.5,0,1,0.5);
        regions.emplace_back(0,0.5,0.5,1);
        regions.emplace_back(0.5,0.5,1,1);
        break;
    case 5:
        regions.emplace_back(0,0,0.333,0.333);
        regions.emplace_back(0.333,0,0.666,0.333);
        regions.emplace_back(0.666,0,1,0.333);
        regions.emplace_back(0.16,0.333,0.49,0.666);
        regions.emplace_back(0.50,0.333,0.84,0.666);
        break;
    default:break;
    }
    for(auto& r: regions){
        r.setFatherRegion(region);
    }
}

float NameRegion::getx() const {
    return fatherRegion.getx() + x * fatherRegion.getWidth();
}

float NameRegion::gety() const {
    return fatherRegion.gety() + y * (fatherRegion.getyend_()-fatherRegion.gety_());
}

float NameRegion::getxend() const {
    return fatherRegion.getxend() - (1-xend)* fatherRegion.getWidth();
}

float NameRegion::getyend() const {
    return fatherRegion.gety() + yend * (fatherRegion.getyend_()-fatherRegion.gety_());
}