#include "core/screen/nameScreen.h"

#include "core/log.h"
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Font.h"
#include "core/Config.h"

using namespace screen;
using namespace core;

static int nameCounter=0;

void NameScreen::init() {
    
}

void NameScreen::Draw() {
    if(background)
    background->Draw({0, 0, 1, 1});
    
}

void NameScreen::enter() {
    alpha=0;
    
}