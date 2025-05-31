#include "core/screen/SettingScreen.h"
#include "core/log.h"

#define NEXT "c"
#define LAST "b"

using namespace screen;
using namespace core;

void SettingScreen::init() {
    background=core::Explorer::getInstance()->getBitmap(BitmapID::SettingBg);
    buttons.clear();
    std::shared_ptr<Button> next=std::make_shared<Button>(Button());
    std::shared_ptr<Button> last=std::make_shared<Button>(Button());
    next->SetFontID(FontID::Icon);
    next->SetRegion({0.93,0.9,0.96,0.93});
    next->SetColor(Color(0xFFB266));
    next->SetClickFunc([this]{
        changePage(true);
    });
    next->SetEnableBitmap(false);
    next->SetFontScale(0.25);
    next->SetText(NEXT);
    last->SetFontID(FontID::Icon);
    last->SetRegion({0.9,0.9,0.93,0.93});
    last->SetColor(Color(0xFFB266));
    last->SetClickFunc([this]{
        changePage(false);
    });
    last->SetEnableBitmap(false);
    last->SetFontScale(0.25);
    last->SetText(LAST);
    buttons.push_back(next);
    buttons.push_back(last);
}

void SettingScreen::Draw() {
    Screen::Draw();
    if(background)
        background->Draw({0, 0, 1, 1},0.7);
    for(const auto& button : s_buttons) {
        if(button)
            button.Draw(currentPage);
    }

    if(currentPage<titles.size()){
        core::Font* font=core::Explorer::getInstance()->getFont(FontID::Normal);
        font->RenderTextBetween(titles[currentPage], {0,0.05,1,0.1}, 0.7f, Color(255, 255, 255,255));
    }
}

void SettingScreen::enter(int) {
    Log << Level::Info << "进入设置界面" << op::endl;
    loadButtons();
}

bool SettingScreen::Click(int x, int y) {
    if(Screen::Click(x, y))return true;
    for(auto& button : s_buttons) if(button)
        if(button.Click(Point(x, y,false)))return true;
    for(auto& b:buttons)if(b)
        if(b->OnClick(Point(x,y,false)))return true;
    return false;
}

void SettingScreen::changePage(bool forward){
    if(forward){
        if(currentPage<pages)
            currentPage++;
        else 
            currentPage=0; // 如果已经是最后一页，则回到第一页
    }
    else{
        if(currentPage>0)
            currentPage--;
        else 
            currentPage=pages; // 如果已经是第一页，则回到最后一页
    }
    Log<< Level::Info << "当前页: " << currentPage << op::endl;
}