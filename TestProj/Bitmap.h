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

    // ʹ���Զ���Ļ�������
    static VertexBuffer* vb;
    static IndexBuffer* ib;
    static VertexArray* va;

public:
    Bitmap() = default;
    ~Bitmap();
    // ����ͼƬ
    bool LoadFromFile(const std::string& path);
    // ����ͼƬ��ָ�����ϽǺ����½�����
    void Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle = 0.0f) const;
    // ��ʼ����ɫ���ͻ������
    static void init();

private:
    // �������ݺ���������
    static float vertices[20];
    static unsigned int indices[6];

    // ��ɫ��Դ��
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
};