#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint ID;

    // ���캯����ȡ��������ɫ��
    Shader(const char* vertexPath, const char* fragmentPath);

    // ʹ�ó���
    void use();

    // ����uniform����
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};
