#pragma once

#include "../render/Texture.h"
#include "base.h"


namespace core
{
class Bitmap
{
public:
    Bitmap(const std::string& filePath){Load(filePath);}
    Bitmap(){}
    ~Bitmap(){}

    bool Load(const std::string& filePath);

    void Draw(Region region);

    inline unsigned int getWidth() const { return texture->getWidth(); }
    inline unsigned int getHeight() const { return texture->getHeight(); }
    inline operator bool() const { return texture != nullptr; }
private:
    std::shared_ptr<Texture> texture;
};

} // namespace core
