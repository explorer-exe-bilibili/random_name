#pragma once
#include <string>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


class Shader {
    unsigned int ID;
    std::string VertexShader;
	std::string fragmentShader;
public:
    // ���캯����ȡ��������ɫ��
    Shader(const std::string& VertexShader, const std::string& fragmentShader);
	Shader(const Shader&);
    Shader();
    ~Shader();

	void init(const std::string& VertexShader, const std::string& fragmentShader);

    // ʹ�ó���
    void use() const;

    // ����uniform����
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    //��ȡuniform����
	bool getBool(const std::string& name) const;
	int getInt(const std::string& name) const;
	unsigned int getUInt(const std::string& name) const;
	float getFloat(const std::string& name) const;
	glm::vec3 getVec3(const std::string& name) const;
	glm::vec4 getVec4(const std::string& name) const;
	glm::mat4 getMat4(const std::string& name) const;
	// ���ظ�ֵ�����
	Shader& operator=(const Shader&);
	operator bool() const;
};
