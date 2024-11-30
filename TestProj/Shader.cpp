#include "Shader.h"
#include <iostream>
#include <glad/glad.h>
#include<glm/glm.hpp>

using namespace std;


#define ASSERT(x) if (!(x)) __debugbreak()

#define GLCall(a) GLClearError();\
	a;\
	ASSERT(GLLogCall(#a,__FILE__,__LINE__))


static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}


static unsigned int CompileShader(unsigned int type, const std::string& source) {
	GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)_malloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		_freea(message);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
	return program;
}

Shader::Shader(const string& VertexShader,const string& fragmentShader) {
	init(VertexShader, fragmentShader);
}

Shader::Shader(const Shader& old)
{
	init(old.VertexShader, old.fragmentShader);
}

Shader::Shader()
{
	ID = 0;
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

void Shader::init(const std::string& VertexShader, const std::string& fragmentShader)
{
	this->VertexShader = VertexShader;
	this->fragmentShader = fragmentShader;
	ID = CreateShader(VertexShader, fragmentShader);
}

void Shader::use() const
{
	GLCall(glUseProgram(ID));
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	GLCall(glUniform4fv(location, 1, &value[0]));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

bool Shader::getBool(const std::string& name) const
{
	int value;
	glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), &value);
	return value;
}

int Shader::getInt(const std::string& name) const
{
	int value;
	glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), &value);
	return value;
}

unsigned int Shader::getUInt(const std::string& name) const
{
	GLuint value;
	GLCall(glGetUniformuiv(ID, glGetUniformLocation(ID, name.c_str()), &value));
	return value;
}

float Shader::getFloat(const std::string& name) const
{
	float value;
	glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value);
	return value;
}

glm::vec3 Shader::getVec3(const std::string& name) const
{
	glm::vec3 value;
	glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value[0]);
	return value;
}

glm::vec4 Shader::getVec4(const std::string& name) const
{
	glm::vec4 value;
	glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value[0]);
	return value;
}

glm::mat4 Shader::getMat4(const std::string& name) const
{
	glm::mat4 value;
	glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value[0][0]);
	return value;
}

Shader& Shader::operator=(const Shader& old)
{
	init(old.VertexShader, old.fragmentShader);
	return *this;
}

Shader::operator bool() const
{
	return ID;
}
