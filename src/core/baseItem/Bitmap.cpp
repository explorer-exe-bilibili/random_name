#include "core/baseItem/Bitmap.h"

#include "core/log.h"
#include "core/baseItem/base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



using namespace core;

bool Bitmap::Load(const std::string& filePath)
{
    if (texture)
    {
        Log<<Level::Warn << "Bitmap::Load() texture already loaded" << op::endl;
        texture.~Texture();
        new (&texture) Texture(); // 使用placement new重新构造对象
    }
    int width, height, channels;
    Log<<Level::Info << "Loading image: " << filePath << op::endl;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    if (data)
    {
        texture = Texture(data, width, height);
        stbi_image_free(data);
        Log<<Level::Info << "Loaded image: " << filePath << op::endl;
        return true;
    }
    Log<<Level::Error << "Failed to load image: " << filePath << op::endl;
    Log<<Level::Error << "stbi_load error: " << stbi_failure_reason() << op::endl;
    return false;
}

void Bitmap::Draw(Region region)
{
    if (!texture)
    {
        Log<<Level::Error << "Bitmap::Draw() texture is null" << op::endl;
        return;
    }

    // 打印区域信息
    Log<<Level::Info << "Bitmap::Draw() region: " << region.x << "," << region.y
        << " to " << region.xend << "," << region.yend 
        << ", screen size: " << screenInfo.width << "x" << screenInfo.height
        << ", bitmap size: " << texture.getWidth() << "x" << texture.getHeight() << op::endl;

    // 检查屏幕信息是否合法
    if (screenInfo.width <= 0 || screenInfo.height <= 0) {
        Log<<Level::Error << "Bitmap::Draw() invalid screen size: " << screenInfo.width << "x" << screenInfo.height << op::endl;
        return;
    }

    // 如果区域宽高为零，则使用屏幕大小
    if (region.xend == region.x || region.yend == region.y) {
        region.xend = screenInfo.width;
        region.yend = screenInfo.height;
        Log<<Level::Info << "Bitmap::Draw() region adjusted to screen size: " << region.x << "," << region.y
            << " to " << region.xend << "," << region.yend << op::endl;
    }

    // 确保区域有效
    if (region.xend < region.x || region.yend < region.y) {
        Log<<Level::Error << "Bitmap::Draw() invalid region: " << region.x << "," << region.y
            << " to " << region.xend << "," << region.yend << op::endl;
        return;
    }

    auto screenToNDC = [this](const float sx, const float sy) {
        float ndcX = (sx / screenInfo.width) * 2.0f - 1.0f;
        float ndcY = 1.0f - (sy / screenInfo.height) * 2.0f;
        return glm::vec3(ndcX, ndcY, 0.0f);
    };

    // 左上角和右下角坐标
    glm::vec3 topLeft = screenToNDC(region.x, region.y);
    glm::vec3 bottomRight = screenToNDC(region.xend, region.yend);

    Log<<Level::Info << "Bitmap::Draw() NDC coords: (" << topLeft.x << "," << topLeft.y 
        << ") to (" << bottomRight.x << "," << bottomRight.y << ")" << op::endl;

    texture.Draw(topLeft, bottomRight);
}

Bitmap& Bitmap::operator=(const Bitmap& bitmap)
{
    Log<<Level::Info << "Bitmap& Bitmap::operator=(const Bitmap& bitmap) " << this->texture << " to " << bitmap.texture << op::endl;
    if (this != &bitmap)
    {
        texture = bitmap.texture;
    }
    return *this;
}