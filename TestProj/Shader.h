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
    // 构造函数读取并编译着色器
    Shader(const std::string& VertexShader, const std::string& fragmentShader);
	Shader(const Shader&);
    Shader();
    ~Shader();

	void init(const std::string& VertexShader, const std::string& fragmentShader);

    // 使用程序
    void use() const;

    // 设置uniform变量
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    //获取uniform变量
	bool getBool(const std::string& name) const;
	int getInt(const std::string& name) const;
	unsigned int getUInt(const std::string& name) const;
	float getFloat(const std::string& name) const;
	glm::vec3 getVec3(const std::string& name) const;
	glm::vec4 getVec4(const std::string& name) const;
	glm::mat4 getMat4(const std::string& name) const;
	// 重载赋值运算符
	Shader& operator=(const Shader&);
	operator bool() const;
};
