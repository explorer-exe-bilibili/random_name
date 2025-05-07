#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

// 简单的PNG写入器 - 只用于生成测试图片
void writePNG(const char* filename, int width, int height)
{
    // PNG文件签名
    const uint8_t pngSignature[] = {137, 80, 78, 71, 13, 10, 26, 10};

    // 创建一个简单的64x64彩色图像
    std::vector<uint8_t> rgbData(width * height * 3, 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            // 创建一个蓝色渐变格子图案
            if ((x / 8 + y / 8) % 2 == 0) {
                rgbData[idx] = 30;                 // R
                rgbData[idx + 1] = 30 + y/2;       // G
                rgbData[idx + 2] = 100 + x/2;      // B
            } else {
                rgbData[idx] = 100;                // R
                rgbData[idx + 1] = 150;            // G
                rgbData[idx + 2] = 200;            // B
            }
        }
    }

    // 打开文件准备写入
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法创建PNG文件" << std::endl;
        return;
    }

    // 写入PNG签名
    file.write(reinterpret_cast<const char*>(pngSignature), 8);

    // 这里简化处理，只生成一个纯色图片示例
    // 实际生成PNG需要更多的处理（数据块、CRC校验等）
    std::cout << "无法直接生成PNG文件，请使用其他工具或从网络下载一个测试图片并放到res/textures/logo.png位置" << std::endl;
    file.close();
}

int main() {
    std::cout << "正在生成测试纹理..." << std::endl;
    writePNG("../res/textures/logo.png", 64, 64);
    std::cout << "请注意：生成的是不完整的PNG文件，请获取一个有效的PNG图片用于测试" << std::endl;
    return 0;
}