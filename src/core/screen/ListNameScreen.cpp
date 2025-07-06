#include "core/screen/ListNameScreen.h"

#include <algorithm>

using namespace core;
using namespace screen;

void ListNameScreen::init() {
    background=Explorer::getInstance()->getBitmapPtr(BitmapID::NameBg);
    m_buttons.resize(10);
    for(int i = 0; i < m_buttons.size(); ++i) {
        m_buttons[i] = std::make_shared<ListNameButton>();
    }
    reloadButtonsRegion();
}

bool ListNameScreen::Click(int x, int y) {
    // 点击处理代码
    for(auto& button : buttons)if(button) if(button && button->OnClick(Point(x, y,false))) return true;
    for(auto& b: m_buttons)if(b) if( b->OnClick(Point(x, y,false))) return true;
    SwitchToScreenWithFade(ScreenID::MainMenu,0,0.5f, []() {
        core::Explorer::getInstance()->playAudio(AudioID::bgm);
    });
    return false;
}

void ListNameScreen::Draw() {
    updateTransition(0);
    // 绘制代码
    if(background && *background) {
        (*background)->Draw({0,0,1,1});
    }
    for(auto& button : buttons) {
        if(button) {
            button->Draw();
        }
    }
    for(auto& b: m_buttons) {
        if(b) {
            b->Draw();
        }
    }
    
    // 在最后绘制编辑覆盖层
    if (editModeEnabled) {
        DrawEditOverlays();
    }
}

void ListNameScreen::enter(int) {
    // 进入代码
    std::sort(nameItems.begin(), nameItems.end(), [](const NameEntry& a, const NameEntry& b) {
        return a.star > b.star; // 按星级降序排序
    });
    int i = 0;
    for(auto& button : m_buttons) {
        if(button) {
            button->SetRegion({0, 0, 1, 1});
            if(nameItems.size() > i) {
                button->SetName(nameItems[i++]);
            } else {
                button->SetName(NameEntry("未知", 0, NameType::Unknow, 0));
            }
            button->SetFontID(FontID::Name);
            button->SetColor(Color(10, 10, 10, 255));
            button->SetAudioID(AudioID::enter);
            button->SetFontScale(0.7f);
            // button->SetClickFunc([this, i]{ 
                // SwitchToScreenWithFade(ScreenID::NameInfo,i,0.5f);
            // });
            //TODO:名字详情页面
            button->SetRegion({baseRegion.getOriginX() + i * baseRegion.getOriginW()+1, baseRegion.getOriginY(), baseRegion.getOriginXEnd() + i * baseRegion.getOriginW() + 1, baseRegion.getOriginYEnd(),baseRegion.getRatio()});
        }
    }
    std::thread([this]{
        int index = 0;
        for(auto& button : m_buttons) {
            if(button) {
                Region region = {(index+1) * baseRegion.getOriginW(), baseRegion.getOriginY(), (index+2) * baseRegion.getOriginW(), baseRegion.getOriginYEnd(),baseRegion.getRatio()};
                button->MoveTo(region, true, 30.0f);
                Log<<"Moving button "<<index<<" region: "<<region<<op::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            index++;
            if(currentScreenID != ScreenID::ListName)return;
        }
    }).detach();
}

void ListNameScreen::reloadButtonsRegion(){
    for(auto& b:buttons)b->resetRegion();
    baseRegion=Config::getInstance()->getRegion(UI_REGION_LISTNAME_BUTTON);
}

ListNameButton::ListNameButton() {
    // 构造函数
    fontPtr = Explorer::getInstance()->getFontPtr(FontID::Name);
}

ListNameButton::~ListNameButton() {
    // 析构函数
}

void ListNameButton::SetName(const NameEntry& nameEntry) {
    SetText(nameEntry.name);
    BitmapID t=BitmapID::Unknown;
    switch (nameEntry.star)
    {
        case 3: t=BitmapID::star3; break;
        case 4: t=BitmapID::star4; break;
        case 5: t=BitmapID::star5; break;
        case 6: t=BitmapID::star6; break;
        default: t=BitmapID::star3; break;
    }
    SetBitmap(t);
}

void ListNameButton::Draw() {
    // 绘制按钮
    if (bitmapPtr && *bitmapPtr) {
        (*bitmapPtr)->Draw(region);
    }
    if (enableText && fontPtr && *fontPtr) {
        Region textRegion={region.getOriginX(), region.getOriginY() + 0.1f, region.getOriginXEnd(), region.getOriginYEnd() - 0.2f, region.getRatio()};
        (*fontPtr)->RenderTextVerticalBetween(text, textRegion, fontScale,color);
    }
}