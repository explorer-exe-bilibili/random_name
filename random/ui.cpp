#include "ui.h"

#include "config.h"


int ui::mode=1, ui::ScreenMode=FIRST_SCREEN;
HFONT ui::text=nullptr, ui::icon=nullptr, ui::text_big=nullptr, ui::text_mid=nullptr, ui::text_list=nullptr, ui::icon_mid=nullptr;
bool ui::ScreenModeChanged = true;
int i = config::init();
HistoryScreen* ui::HS = nullptr;
FirstScreen* ui::FS = nullptr;
set2* ui::SS = nullptr;
NameScreen* ui::NS = nullptr;
MenuScreen* ui::MS = nullptr;