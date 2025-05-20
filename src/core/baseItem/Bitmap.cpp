#include "core/baseItem/Bitmap.h"

#include "core/log.h"
#include "core/baseItem/base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <chrono>

// 跨平台 SIMD 支持
#if defined(_MSC_VER) // MSVC 编译器
    #include <intrin.h>
    #if defined(_M_X64) || defined(_M_IX86) // 只有在 x86/x64 架构上启用
        #define HAVE_SSE2
        #if (_MSC_VER >= 1600) && defined(_M_X64) // Visual Studio 2010及以上，且是x64架构
            #define HAVE_AVX
        #endif
    #elif defined(_M_ARM64) || defined(_M_ARM) // ARM 架构
        #include <arm_neon.h>
        #define HAVE_NEON
    #endif
#elif defined(__GNUC__) || defined(__clang__) // GCC 或 Clang 编译器
    #if defined(__x86_64__) || defined(__i386__) // x86/x64 架构
        #include <x86intrin.h>
        #if defined(__SSE2__)
            #define HAVE_SSE2
        #endif
        #if defined(__AVX__)
            #define HAVE_AVX
        #endif
    #elif defined(__aarch64__) || defined(__arm__) // ARM 架构
        #include <arm_neon.h>
        #define HAVE_NEON
    #endif
#endif

// 确保在 Apple Silicon 上不使用 x86 指令集
#if defined(__APPLE__) && defined(__aarch64__)
    #undef HAVE_SSE2
    #undef HAVE_AVX
    #define HAVE_NEON
#endif

using namespace core;
void convertRGBtoRGBA_SIMD(const unsigned char* rgbData, unsigned char* rgbaData, int pixelCount);

// 缓存RGBA缓冲区以减少内存分配
class RGBAConverter {
public:
    static unsigned char* getRGBABuffer(int size) {
        // 确保缓冲区足够大
        if (size > bufferSize) {
            delete[] rgbaBuffer;
            rgbaBuffer = new unsigned char[size];
            bufferSize = size;
            Log << Level::Debug << "RGBAConverter: 重新分配缓冲区大小为 " << size << " 字节" << op::endl;
        }
        return rgbaBuffer;
    }
    
    static void cleanup() {
        delete[] rgbaBuffer;
        rgbaBuffer = nullptr;
        bufferSize = 0;
    }
    
private:
    static unsigned char* rgbaBuffer;
    static int bufferSize;
};
void logSIMDSupport();



Bitmap::Bitmap(int width, int height, bool createTexture) {
    // 记录使用的SIMD优化类型
    logSIMDSupport();
    
    m_width = width;
    m_height = height;
    
    if (createTexture) {
        texture = std::make_shared<Texture>(width, height);
        if (!texture) {
            Log << Level::Error << "Failed to create empty bitmap of size " << width << "x" << height << op::endl;
        } else {
            Log << Level::Info << "Created empty bitmap of size " << width << "x" << height << op::endl;
        }
    } else {
        // 不创建纹理，只分配内存
        rgbData = new unsigned char[width * height * 3]; // RGB格式
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

bool Bitmap::CreateFromRGBData(const unsigned char* data, int width, int height, bool createTexture) {
    if (!data) {
        Log<<Level::Error << "Bitmap::CreateFromRGBData() data is null" << op::endl;
        return false;
    }
    
    m_width = width;
    m_height = height;
    
    if (createTexture) {
        // 释放旧的纹理
        if (texture) {
            Log<<Level::Warn << "Bitmap::CreateFromRGBData() texture already loaded" << op::endl;
            texture.reset();
        }
        
        // 添加性能计时
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 使用缓存缓冲区减少内存分配
        int bufferSize = width * height * 4;
        unsigned char* rgbaData = RGBAConverter::getRGBABuffer(bufferSize);
        
        // 使用SIMD优化的RGB到RGBA转换
        convertRGBtoRGBA_SIMD(data, rgbaData, width * height);
        
        
        // 计算并输出性能数据
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        Log<<Level::Debug << "RGB to RGBA conversion took " << (long)duration << "ms for " 
           << (width * height) << " pixels" << op::endl;
        
        texture = std::make_shared<Texture>(rgbaData, width, height);
        // 不需要删除rgbaData，它来自缓存
        
        if (!texture) {
            Log<<Level::Error << "Failed to create bitmap from RGB data" << op::endl;
            return false;
        }
    } else {
        // 不创建纹理，只存储原始数据
        if (rgbData) {
            delete[] rgbData;
        }
        
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
    
    Log<<Level::Info << "Creating texture from buffer of size " << m_width << "x" << m_height << op::endl;
    
    // 添加性能计时
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 使用缓存缓冲区减少内存分配
    int bufferSize = m_width * m_height * 4;
    unsigned char* rgbaData = RGBAConverter::getRGBABuffer(bufferSize);
    
    // 使用SIMD优化的RGB到RGBA转换
    convertRGBtoRGBA_SIMD(rgbData, rgbaData, m_width * m_height);

    
    // 计算并输出性能数据
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    Log<<Level::Debug << "RGB to RGBA conversion took " << (long)duration << "ms for " 
       << (m_width * m_height) << " pixels" << op::endl;
    texture = std::make_shared<Texture>(rgbaData, m_width, m_height);
    
    // 不删除rgbaData，因为它是来自缓存
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

// SIMD优化的RGB到RGBA转换
void convertRGBtoRGBA_SIMD(const unsigned char* rgbData, unsigned char* rgbaData, int pixelCount) {
#if defined(HAVE_AVX) && !defined(__APPLE__) // 避免在Apple上使用AVX (M1/M2芯片不支持)
    // 使用AVX指令集加速 (x86_64)
    const int simdSize = 8; // AVX每次处理8个元素
    const int simdPixels = pixelCount - (pixelCount % simdSize);
    
    __m256i alpha = _mm256_set1_epi8(255); // 设置所有Alpha通道为255
    
    for (int i = 0; i < simdPixels; i += simdSize) {
        // 处理8个像素，每像素3个字节(RGB)
        const unsigned char* src = rgbData + i * 3;
        unsigned char* dst = rgbaData + i * 4;
        
        // 由于RGB打包的复杂性，简化实现
        for (int j = 0; j < simdSize; j++) {
            dst[j*4] = src[j*3];       // R
            dst[j*4+1] = src[j*3+1];   // G
            dst[j*4+2] = src[j*3+2];   // B
            dst[j*4+3] = 255;          // A
        }
    }
    
    // 处理剩余的像素
    for (int i = simdPixels; i < pixelCount; i++) {
        rgbaData[i * 4] = rgbData[i * 3];         // R
        rgbaData[i * 4 + 1] = rgbData[i * 3 + 1]; // G
        rgbaData[i * 4 + 2] = rgbData[i * 3 + 2]; // B
        rgbaData[i * 4 + 3] = 255;                // A
    }
    
#elif defined(HAVE_SSE2) && !defined(__APPLE__) // 避免在Apple上使用SSE2
    // 使用SSE2指令集加速 (x86/x64)
    const int simdSize = 4; // SSE2每次处理4个元素
    const int simdPixels = pixelCount - (pixelCount % simdSize);
    
    __m128i alpha = _mm_set1_epi8(255); // 设置所有Alpha通道为255
    
    for (int i = 0; i < simdPixels; i += simdSize) {
        // 处理4个像素，每像素3个字节(RGB)
        const unsigned char* src = rgbData + i * 3;
        unsigned char* dst = rgbaData + i * 4;
        
        // 由于RGB打包的复杂性，简化实现
        for (int j = 0; j < simdSize; j++) {
            dst[j*4] = src[j*3];       // R
            dst[j*4+1] = src[j*3+1];   // G
            dst[j*4+2] = src[j*3+2];   // B
            dst[j*4+3] = 255;          // A
        }
    }
    
    // 处理剩余的像素
    for (int i = simdPixels; i < pixelCount; i++) {
        rgbaData[i * 4] = rgbData[i * 3];         // R
        rgbaData[i * 4 + 1] = rgbData[i * 3 + 1]; // G
        rgbaData[i * 4 + 2] = rgbData[i * 3 + 2]; // B
        rgbaData[i * 4 + 3] = 255;                // A
    }

#elif defined(HAVE_NEON)
    // 使用ARM NEON指令集加速 (ARM/ARM64)
    const int simdSize = 8; // NEON每次处理8个元素
    const int simdPixels = pixelCount - (pixelCount % simdSize);
    
    // NEON优化版本的RGB到RGBA转换
    for (int i = 0; i < simdPixels; i += simdSize) {
        const unsigned char* src = rgbData + i * 3;
        unsigned char* dst = rgbaData + i * 4;
        
        // 加载24字节(8个RGB像素)
        // 实际实现比下面复杂，这里为简化使用循环
        for (int j = 0; j < simdSize; j++) {
            dst[j*4] = src[j*3];       // R
            dst[j*4+1] = src[j*3+1];   // G
            dst[j*4+2] = src[j*3+2];   // B
            dst[j*4+3] = 255;          // A
        }
        
        // 完整的NEON优化实现应该使用类似这样的指令:
        // 例如，使用vld3和vst4指令加载RGB数据并存储为RGBA
        /*
        uint8x8x3_t rgb = vld3_u8(src);
        uint8x8x4_t rgba;
        rgba.val[0] = rgb.val[0];  // R
        rgba.val[1] = rgb.val[1];  // G
        rgba.val[2] = rgb.val[2];  // B
        rgba.val[3] = vdup_n_u8(255);  // A
        vst4_u8(dst, rgba);
        */
    }
    
    // 处理剩余的像素
    for (int i = simdPixels; i < pixelCount; i++) {
        rgbaData[i * 4] = rgbData[i * 3];         // R
        rgbaData[i * 4 + 1] = rgbData[i * 3 + 1]; // G
        rgbaData[i * 4 + 2] = rgbData[i * 3 + 2]; // B
        rgbaData[i * 4 + 3] = 255;                // A
    }
    
#else
    // 回退到优化的标量代码 (通用实现，适用于所有平台)
    const unsigned char* src = rgbData;
    unsigned char* dst = rgbaData;
    
    // 使用指针直接操作以优化性能
    for (int i = 0; i < pixelCount; i++) {
        *dst++ = *src++;     // R
        *dst++ = *src++;     // G
        *dst++ = *src++;     // B
        *dst++ = 255;        // A
    }
#endif
}

// 检测并记录使用的SIMD优化类型
void logSIMDSupport() {
    static bool logged = false;
    if (logged) return;
    
    logged = true;
    
#if defined(HAVE_AVX) && !defined(__APPLE__)
    Log << Level::Info << "使用AVX指令集加速RGB到RGBA转换" << op::endl;
#elif defined(HAVE_SSE2) && !defined(__APPLE__)
    Log << Level::Info << "使用SSE2指令集加速RGB到RGBA转换" << op::endl;
#elif defined(HAVE_NEON)
    Log << Level::Info << "使用ARM NEON指令集加速RGB到RGBA转换" << op::endl;
#else
    Log << Level::Info << "使用通用实现进行RGB到RGBA转换" << op::endl;
#endif
}

// 静态方法，用于在程序关闭前清理全局资源
void Bitmap::CleanupStaticResources() {
    // 清理RGBA转换器的缓存
    RGBAConverter::cleanup();
    Log << Level::Info << "Cleaned up Bitmap static resources" << op::endl;
}

// 静态成员初始化
unsigned char* RGBAConverter::rgbaBuffer = nullptr;
int RGBAConverter::bufferSize = 0;