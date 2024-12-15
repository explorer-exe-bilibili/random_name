#define STB_IMAGE_IMPLEMENTATION
#include "Bitmap.h"
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// 静态成员变量初始化
std::shared_ptr<Shader> Bitmap::shader = nullptr;
VertexBuffer* Bitmap::vb = nullptr;
IndexBuffer* Bitmap::ib = nullptr;
VertexArray* Bitmap::va = nullptr;

float Bitmap::vertices[20] = {
    // 位置           // 纹理坐标
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // 左上
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // 右上
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f  // 左下
};

unsigned int Bitmap::indices[6] = {
    0, 1, 2,
    2, 3, 0
};

const char* Bitmap::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 transform;

out vec2 TexCoord;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* Bitmap::fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";

void Bitmap::init()
{
    va = new VertexArray;

    // 初始化着色器
    shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);

    // 创建并绑定 VAO
    va->Bind();

    // 创建 VertexBuffer 并填充数据
    vb = new VertexBuffer(vertices, sizeof(vertices));

    // 创建 IndexBuffer 并填充数据
    ib = new IndexBuffer(indices, 6);

    // 设置顶点属性
    va->AddBuffer(*vb, 0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);                 // 位置属性
    va->AddBuffer(*vb, 1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float))); // 纹理坐标属性

    // 解绑 VAO
    VertexArray::Unbind();
}

Bitmap::~Bitmap()
{
    if (IsLoad)
    {
        glDeleteTextures(1, &texture);
    }
}

bool Bitmap::LoadFromFile(const std::string& path)
{
    stbi_set_flip_vertically_on_load(false);
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        std::cerr << "无法加载纹理图片：" << path << std::endl;
        return false;
    }

    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 释放图像数据
    stbi_image_free(data);
    IsLoad = true;

    return true;
}

void Bitmap::Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle) const
{
    if (!IsLoad)
        return;

    // 使用着色器
    shader->use();

    // 计算尺寸和中心点
    glm::vec3 size = bottomRight - topLeft;
    glm::vec3 center = topLeft + size * 0.5f;

    // 创建变换矩阵
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, center);
    if (angle != 0.0f)
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));
    transform = glm::scale(transform, glm::vec3(size.x, size.y, 1.0f));

    // 设置变换矩阵
    shader->setMat4("transform", transform);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("texture1", 0);

    // 绘制
    va->Bind();
    ib->Bind();
    glDrawElements(GL_TRIANGLES, ib->getCount(), GL_UNSIGNED_INT, nullptr);
    // 解绑 VAO 和 IBO
    VertexArray::Unbind();
    IndexBuffer::Unbind();
}