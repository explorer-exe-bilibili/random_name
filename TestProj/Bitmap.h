#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"
#include "VertexBuffer.h"

class Bitmap
{
    bool IsLoad = false;
    unsigned int texture = 0;
    static std::shared_ptr<Shader> shader;

    // 使用自定义的缓冲区类
    static VertexBuffer* vb;
    static IndexBuffer* ib;
    static VertexArray* va;

public:
    Bitmap() = default;
    ~Bitmap();
    // 加载图片
    bool LoadFromFile(const std::string& path);
    // 绘制图片，指定左上角和右下角坐标
    void Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle = 0.0f) const;
    // 初始化着色器和缓冲对象
    static void init();

private:
    // 顶点数据和索引数据
    static float vertices[20];
    static unsigned int indices[6];

    // 着色器源码
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
};