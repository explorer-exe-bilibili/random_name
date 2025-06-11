#include "core/screen/ListNameScreen.h"

using namespace core;
using namespace screen;

void ListNameScreen::init() {
    background=Explorer::getInstance()->getBitmap(BitmapID::NameBg);
    m_buttons.resize(10);
    for(int i = 0; i < m_buttons.size(); ++i) {
        m_buttons[i] = std::make_shared<ListNameButton>();
    }
    std::shared_ptr<Button> exitButton = std::make_shared<Button>();
    exitButton->SetBitmap(BitmapID::Exit);
    exitButton->SetRegion({0.8f, 0.05f, 0.95f, 0.15f});
    exitButton->SetAudioID(AudioID::enter);
    exitButton->SetClickFunc([]() {
        SwitchToScreenWithFade(ScreenID::MainMenu, 0, 0.5f, []() {
            core::Explorer::getInstance()->playAudio(AudioID::bgm, 0);
        });
    });
}

bool ListNameScreen::Click(int x, int y) {
    // 点击处理代码
    for(auto& button : buttons)if(button) if(button && button->OnClick(Point(x, y))) return true;
    for(auto& b: m_buttons)if(b) if( b->OnClick(Point(x, y))) return true;
    SwitchToScreenWithFade(ScreenID::MainMenu,0,0.5f, []() {
        core::Explorer::getInstance()->playAudio(AudioID::bgm, 0);
    });
    return false;
}

void ListNameScreen::Draw() {
    // 绘制代码
    if(background) {
        background->Draw({0,0,1,1});
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
}

void ListNameScreen::enter() {
    // 进入代码
    core::Explorer::getInstance()->playSound(AudioID::enter,0);
    int i = 0;
    for(auto& button : m_buttons) {
        if(button) {
            button->SetRegion({0, 0, 1, 1});
            if(nameItems.size() > i) {
                button->SetName(nameItems[i]);
            } else {
                button->SetName(NameEntry("Unknown", 0, NameType::Unknow, 0));
            }
            button->SetFontID(FontID::Name);
            button->SetColor(Color(255, 255, 255, 255));
            button->SetAudioID(AudioID::enter);
            button->SetClickFunc([this, i]{ 
                // SwitchToScreenWithFade(ScreenID::NameInfo,i,0.5f);
            });
            button->SetRegion({0.1f, 0.1f + i * 0.1f + 1, 0.9f, 0.1f + (i + 1) * 0.1f+1});
        }
    }
    std::thread([this]{
        int index = 0;
        for(auto& button : m_buttons) {
            if(button) {
                button->MoveTo({0.1f, 0.1f + index * 0.1f, 0.9f, 0.1f + (index + 1) * 0.1f}, true, 50.0f);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            index++;
            if(currentScreenID != ScreenID::ListName)return;
        }
    }).detach();
}

ListNameButton::ListNameButton() {
    // 构造函数
    font= Explorer::getInstance()->getFont(FontID::Name);
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
    if (bitmap) {
        bitmap->Draw(region);
    }
    if (enableText && font) {
        font->RenderTextVerticalBetween(text, region, fontScale,color);
    }
}