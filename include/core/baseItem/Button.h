#pragma once

#include "../render/Texture.h"
#include "Bitmap.h"


namespace core {
class Button {
public:
    Button(const std::string& text="", int FontID=0, const Region& region=Region(), const Texture& texture=Texture());
    Button(const Button& button);
    Button& operator=(const Button& button);
    ~Button();

    void Draw();
    void OnClick(Point point);
    void MoveTo(const Region& region);


    void SetText(const std::string& text) {this->text = text;}
    void SetTexture(const Texture& texture) {this->texture = texture;}
    void SetRegion(const Region& region) {this->region = region;}

    void SetEnableText(bool enable) {this->enableText = enable;}
    void SetEnable(bool enable) {this->enable = enable;}
    void SetEnableTexture(bool enable) {this->enableTexture = enable;}
private:
    Region region;
    bool enableText=true;
    bool enable=true;
    bool enableTexture=true;
    std::string text="";
    Texture texture;
    int FontID=0;
};
}