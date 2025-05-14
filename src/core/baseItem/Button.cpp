#include "core/baseItem/Button.h"

#include "core/explorer.h"
#include "core/log.h"

using namespace core;

Button::Button(const std::string& text, int FontID, const Region& region, const Texture& texture) :
 text(text), FontID(FontID), region(region), texture(texture) {}

Button::~Button() {
     // Clean up resources if needed
}

void Button::Draw() {
    // Draw the button
    try {
        auto& explorer = Explorer::getInstance();
        if (!explorer) {
            Log << "Explorer instance is null" << op::endl;
            return;
        }
        
        try {
            Font& font = explorer->getFont(FontID);
            font.RenderText(text, {0,0}, 1, color::white);
        } catch (const std::exception& e) {
            Log << "Error rendering button text: " << e.what() << op::endl;
        }
    } catch (const std::exception& e) {
        Log << "Error in Button::Draw: " << e.what() << op::endl;
    }
}