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
        texture.reset();  // 释放旧的纹理
    }
    int width, height, channels;
    Log<<Level::Info << "Loading image: " << filePath << op::endl;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    if (data)
    {
        texture = std::make_shared<Texture>(data, width, height);
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
    // 检查屏幕信息是否合法
    if (WindowInfo.width <= 0 || WindowInfo.height <= 0) {
        return;
    }

    // 如果区域宽高为零，则使用屏幕大小
    if (region.getxend() == region.getx() || region.getyend() == region.gety()) {
        region.setxend(WindowInfo.width);
        region.setyend(WindowInfo.height);
    }

    // 确保区域有效
    if (region.getxend() < region.getx() || region.getyend() < region.gety()) {
        Log<<Level::Error << "Bitmap::Draw() invalid region: " << region.getx() << "," << region.gety()
            << " to " << region.getxend() << "," << region.getyend() << op::endl;
        return;
    }

    auto screenToNDC = [this](const float sx, const float sy) {
        float ndcX = (sx / WindowInfo.width) * 2.0f - 1.0f;
        float ndcY = 1.0f - (sy / WindowInfo.height) * 2.0f;
        return glm::vec3(ndcX, ndcY, 0.0f);
    };

    // 左上角和右下角坐标
    glm::vec3 topLeft = screenToNDC(region.getx(), region.gety());
    glm::vec3 bottomRight = screenToNDC(region.getxend(), region.getyend());

    texture->Draw(topLeft, bottomRight);
}