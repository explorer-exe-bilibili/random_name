#pragma once

#include "../render/Texture.h"
#include "base.h"
#pragma execution_character_set("utf-8")

namespace core
{
class Bitmap
{
public:
    Bitmap(const std::string& filePath){Load(filePath);}
    Bitmap(const Bitmap& bitmap){texture=bitmap.texture;}
    Bitmap(){}
    ~Bitmap(){}

    bool Load(const std::string& filePath);

    void Draw(Region region);

    Bitmap& operator=(const Bitmap& bitmap);

    inline unsigned int getWidth() const { return texture.getWidth(); }
    inline unsigned int getHeight() const { return texture.getHeight(); }
    inline operator bool() const { return texture; }
private:
    Texture texture;
};

} // namespace core
