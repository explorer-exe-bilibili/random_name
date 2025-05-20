#pragma once

#include "../render/Texture.h"
#include "base.h"


// 前向声明
namespace core {
    // 用于自动清理的静态初始化类
    class BitmapStaticInitializer {
    public:
        BitmapStaticInitializer();
        ~BitmapStaticInitializer();
    };

class Bitmap
{
public:
    Bitmap(const std::string& filePath){Load(filePath);}
    Bitmap(int width, int height, bool createTexture = true); // 创建指定大小的空白位图
    Bitmap(){}
    ~Bitmap(){ if(rgbData) delete[] rgbData; }

    bool Load(const std::string& filePath);
    bool CreateFromRGBData(const unsigned char* data, int width, int height, bool createTexture = true);
    void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
    
    // 在主线程中调用此方法创建纹理
    void CreateTextureFromBuffer();
    bool HasTexture() const { return texture != nullptr; }
    
    void Draw(Region region, float alpha=1.0f);

    inline unsigned int getWidth() const { return texture ? texture->getWidth() : m_width; }
    inline unsigned int getHeight() const { return texture ? texture->getHeight() : m_height; }
    inline operator bool() const { return texture != nullptr || rgbData != nullptr; }
    
    // 静态方法，用于清理全局资源
    static void CleanupStaticResources();
private:
    std::shared_ptr<Texture> texture;
    unsigned char* rgbData = nullptr; // 存储RGB数据，用于延迟创建纹理
    int m_width = 0;
    int m_height = 0;
    
    // 静态初始化器，确保资源在程序退出时被清理
    static BitmapStaticInitializer staticInitializer;
};

} // namespace core
