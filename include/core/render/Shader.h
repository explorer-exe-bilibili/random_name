#pragma once
#include "GLBase.h"
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

// Forward declaration to avoid circular dependency
namespace core {
    class OpenGLErrorRecovery;
}


namespace core {

class Shader {
    unsigned int ID=0;
    std::string VertexShader="";
	std::string fragmentShader="";
public:
    // 构造函数读取并编译着色器
    Shader(const std::string& VertexShader, const std::string& fragmentShader);
	Shader(const Shader&);
    Shader();
    ~Shader();

	void init(const std::string& VertexShader, const std::string& fragmentShader);

    // 使用程序
    void use() const;
    void Bind() const{use();};

    // 设置uniform变量
    void setInt(const std::string& name, int value) const{GLCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), value));}
    void setFloat(const std::string& name, float value) const{GLCall(glUniform1f(glGetUniformLocation(ID, name.c_str()), value));}
    void set2float(const std::string& name, float value1, float value2) const{GLCall(glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2));}
    void setVec3(const std::string& name, const glm::vec3& value) const{GLCall(glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z));}
    void setVec4(const std::string& name, const glm::vec4& value) const{GLCall(glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w));}
    void setMat4(const std::string& name, const glm::mat4& mat) const{GLCall(glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));}

    //获取uniform变量
	int getInt(const std::string& name) const{
        int value;
        GLCall(glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), &value));
        return value;
    }
	unsigned int getUInt(const std::string& name) const{
        unsigned int value;
        GLCall(glGetUniformuiv(ID, glGetUniformLocation(ID, name.c_str()), &value));
        return value;
    }
	float getFloat(const std::string& name) const{
        float value;
        GLCall(glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value));
        return value;
    }
	glm::vec3 getVec3(const std::string& name) const{
        glm::vec3 value;
        GLCall(glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value[0]));
        return value;
    }
	glm::vec4 getVec4(const std::string& name) const{
        glm::vec4 value;
        GLCall(glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value[0]));
        return value;
    }
	glm::mat4 getMat4(const std::string& name) const{
        glm::mat4 value;
        GLCall(glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value[0][0]));
        return value;
    }
	// 重载赋值运算符
	Shader& operator=(const Shader&);
	operator bool() const{return ID != 0;};
    operator unsigned int() const{return ID;};
};
}