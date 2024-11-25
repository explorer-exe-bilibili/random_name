#define STB_IMAGE_IMPLEMENTATION
#include "Bitmap.h"
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// ��̬��Ա������ʼ��
std::shared_ptr<Shader> Bitmap::shader = nullptr;
VertexBuffer* Bitmap::vb = nullptr;
IndexBuffer* Bitmap::ib = nullptr;
VertexArray* Bitmap::va = nullptr;

float Bitmap::vertices[20] = {
    // λ��           // ��������
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // ����
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // ����
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // ����
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f  // ����
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

    // ��ʼ����ɫ��
    shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);

    // �������� VAO
    va->Bind();

    // ���� VertexBuffer ���������
    vb = new VertexBuffer(vertices, sizeof(vertices));

    // ���� IndexBuffer ���������
    ib = new IndexBuffer(indices, 6);

    // ���ö�������
    va->AddBuffer(*vb, 0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);                 // λ������
    va->AddBuffer(*vb, 1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float))); // ������������

    // ��� VAO
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
        std::cerr << "�޷���������ͼƬ��" << path << std::endl;
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

    // �����������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ������������
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // �ͷ�ͼ������
    stbi_image_free(data);
    IsLoad = true;

    return true;
}

void Bitmap::Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle) const
{
    if (!IsLoad)
        return;

    // ʹ����ɫ��
    shader->use();

    // ����ߴ�����ĵ�
    glm::vec3 size = bottomRight - topLeft;
    glm::vec3 center = topLeft + size * 0.5f;

    // �����任����
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, center);
    if (angle != 0.0f)
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));
    transform = glm::scale(transform, glm::vec3(size.x, size.y, 1.0f));

    // ���ñ任����
    shader->setMat4("transform", transform);

    // ������
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("texture1", 0);

    // ����
    va->Bind();
    ib->Bind();
    glDrawElements(GL_TRIANGLES, ib->getCount(), GL_UNSIGNED_INT, nullptr);
    // ��� VAO �� IBO
    VertexArray::Unbind();
    IndexBuffer::Unbind();
}