#include "core/baseItem/Bitmap.h"

#include "core/log.h"
#include "core/baseItem/base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#pragma execution_character_set("utf-8")

using namespace core;

bool Bitmap::Load(const std::string& filePath)
{
    int width, height, channels;
    Log<<Level::Info << "Loading image: " << filePath << op::endl;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
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
    auto screenToNDC = [this](const float sx, const float sy) {
    float ndcX = (sx / screenInfo.width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (sy / screenInfo.height) * 2.0f;
    return glm::vec3(ndcX, ndcY, 0.0f);
    };

	// 左上角和右下角坐标
	glm::vec3 topLeft = screenToNDC(region.x, region.y);
	glm::vec3 bottomRight = screenToNDC(region.xend, region.yend);

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