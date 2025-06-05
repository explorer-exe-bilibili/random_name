#pragma once

#include "../render/Texture.h"
#include "Base.h"


namespace core
{
class Bitmap
{
public:    Bitmap(const std::string& filePath){Load(filePath);}
    Bitmap(int width, int height, bool createTexture = true, bool useRGB = false); // 创建指定大小的空白位图
    Bitmap(){}
    ~Bitmap(){ 
        if(rgbData) {
            delete[] rgbData;
            rgbData = nullptr;
        }
    }
    bool Load(const std::string& filePath);
    bool LoadWebP(const std::string& filePath); // 加载WebP格式图像
    bool CreateFromRGBData(const unsigned char* data, int width, int height, bool createTexture = true, bool directRGB = false);
    void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
      // 在主线程中调用此方法创建纹理
    void CreateTextureFromBuffer();
    bool HasTexture() const { return texture != nullptr; }
    
    // 获取纹理ID（用于帧缓冲操作）
    unsigned int GetTextureID() const { return texture ? texture->getTextureID() : 0; }
    
    void Draw(Region region, float alpha=1.0f);

    inline unsigned int getWidth() const { return texture ? texture->getWidth() : m_width; }
    inline unsigned int getHeight() const { return texture ? texture->getHeight() : m_height; }
    inline operator bool() const { return texture != nullptr || rgbData != nullptr; }
private:
    std::shared_ptr<Texture> texture;
    unsigned char* rgbData = nullptr; // 存储RGB数据，用于延迟创建纹理
    int m_width = 0;
    int m_height = 0;
    bool m_useRGB = false; // 是否使用RGB格式而不是RGBA
};

} // namespace core
