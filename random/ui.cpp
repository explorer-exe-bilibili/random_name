#include "ui.h"

#include "config.h"


int ui::mode=1, ui::ScreenMode=FIRST_SCREEN;
Font* ui::text=nullptr, *ui::icon=nullptr;
bool ui::ScreenModeChanged = true;
int i = config::init();
HistoryScreen* ui::HS = nullptr;
FirstScreen* ui::FS = nullptr;
set2* ui::SS = nullptr;
NameScreen* ui::NS = nullptr;
MenuScreen* ui::MS = nullptr;