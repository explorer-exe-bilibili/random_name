#include "core/screen/nameScreen.h"

#include "core/log.h"
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Font.h"

using namespace screen;
using namespace core;

void NameScreen::init() {
    background = core::Explorer::getInstance()->getBitmap("name");
    
}