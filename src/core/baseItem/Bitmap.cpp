#include "core/baseItem/Bitmap.h"

#include "core/log.h"
#include "core/baseItem/base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <chrono>



using namespace core;

Bitmap::Bitmap(int width, int height, bool createTexture, bool useRGB) {
    m_width = width;
    m_height = height;
    m_useRGB = useRGB;
    
    if (createTexture) {
        texture = std::make_shared<Texture>(width, height, useRGB);
        if (!texture) {
            Log << Level::Error << "Failed to create empty bitmap of size " << width << "x" << height << op::endl;
        } else {
            Log << Level::Info << "Created empty bitmap of size " << width << "x" << height 
                << (useRGB ? " (RGB format)" : " (RGBA format)") << op::endl;
        }
    } else {
        // 不创建纹理，只分配内存
        rgbData = new unsigned char[width * height * 3]; // 总是用RGB格式存储原始数据
        Log << Level::Info << "Created raw buffer bitmap of size " << width << "x" << height << op::endl;
    }
}

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
    }    Log<<Level::Error << "Failed to load image: " << filePath << op::endl;
    Log<<Level::Error << "stbi_load error: " << stbi_failure_reason() << op::endl;
    return false;
}

bool Bitmap::CreateFromRGBData(const unsigned char* data, int width, int height, bool createTexture, bool directRGB) {
    if (!data) {
        Log<<Level::Error << "Bitmap::CreateFromRGBData() data is null" << op::endl;
        return false;
    }
    
    m_width = width;
    m_height = height;
    m_useRGB = directRGB;
    
    // 清理旧的资源
    if (rgbData) {
        delete[] rgbData;
        rgbData = nullptr;
    }
    
    if (createTexture) {
        // 释放旧的纹理
        if (texture) {
            texture.reset();
        }
        
        if (directRGB) {
            // 直接使用RGB数据创建纹理，跳过RGB转RGBA的过程
            texture = std::make_shared<Texture>(data, width, height, true);
            
            Log<<Level::Info << "Created texture directly from RGB data of size " << width << "x" << height << op::endl;
        } else {
            // 从RGB数据创建RGBA数据（添加Alpha通道）
            unsigned char* rgbaData = new unsigned char[width * height * 4];
            for (int i = 0; i < width * height; i++) {
                rgbaData[i * 4] = data[i * 3];         // R
                rgbaData[i * 4 + 1] = data[i * 3 + 1]; // G
                rgbaData[i * 4 + 2] = data[i * 3 + 2]; // B
                rgbaData[i * 4 + 3] = 255;             // A (不透明)
            }
            
            texture = std::make_shared<Texture>(rgbaData, width, height, false);
            delete[] rgbaData; // 立即释放临时数据
            
            Log<<Level::Info << "Created texture from RGB data with RGBA conversion, size " << width << "x" << height << op::endl;
        }
        
        if (!texture) {
            Log<<Level::Error << "Failed to create bitmap from RGB data" << op::endl;
            return false;
        }
    } else {
        // 不创建纹理，只存储原始数据
        rgbData = new unsigned char[width * height * 3];
        memcpy(rgbData, data, width * height * 3);
    }
    
    Log<<Level::Info << "Created bitmap from RGB data of size " << width << "x" << height 
                     << (createTexture ? " with texture" : " without texture") << op::endl;
    return true;
}

void Bitmap::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    // 这个函数暂时不实现，因为在OpenGL中不容易直接设置单个像素
    // 通常应该收集像素更改，然后一次性更新纹理
    // 对于视频帧处理，我们使用CreateFromRGBData更高效
    Log<<Level::Warn << "Bitmap::setPixel() not implemented, use CreateFromRGBData instead" << op::endl;
}

void Bitmap::CreateTextureFromBuffer() {
    if (texture || !rgbData || m_width <= 0 || m_height <= 0) {
        return; // 已经有纹理或没有原始数据
    }
    
    Log<<Level::Info << "Creating texture from buffer of size " << m_width << "x" << m_height 
        << (m_useRGB ? " (direct RGB)" : " (with RGBA conversion)") << op::endl;
    
    // 记录开始时间
    auto start = std::chrono::high_resolution_clock::now();
    
    if (m_useRGB) {
        // 直接使用RGB数据创建纹理，避免RGB到RGBA的转换
        texture = std::make_shared<Texture>(rgbData, m_width, m_height, true);
    } else {
        // 从RGB数据创建RGBA数据（添加Alpha通道）
        unsigned char* rgbaData = new unsigned char[m_width * m_height * 4];
        for (int i = 0; i < m_width * m_height; i++) {
            rgbaData[i * 4] = rgbData[i * 3];         // R
            rgbaData[i * 4 + 1] = rgbData[i * 3 + 1]; // G
            rgbaData[i * 4 + 2] = rgbData[i * 3 + 2]; // B
            rgbaData[i * 4 + 3] = 255;                // A (不透明)
        }
        
        texture = std::make_shared<Texture>(rgbaData, m_width, m_height, false);
        delete[] rgbaData;
    }
    
    // 记录结束时间
    auto end = std::chrono::high_resolution_clock::now();
    
    // 计算耗时
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Log<<Level::Info << "Texture creation took " << (long)duration << " ms" << op::endl;
    
    // 清理RGB数据，因为已经转换成纹理了
    delete[] rgbData;
    rgbData = nullptr;
}

void Bitmap::Draw(Region region, float alpha) {
    // 确保在绘制前有纹理
    if (!texture) {
        if (rgbData) {
            // 从缓冲区创建纹理
            CreateTextureFromBuffer();
        } else {
            Log<<Level::Error << "Bitmap::Draw() texture is null and no buffer available" << op::endl;
            return;
        }
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

    texture->Draw(topLeft, bottomRight, 0, alpha);
}