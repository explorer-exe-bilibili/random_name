#include"core/render/Shader.h"

#include "core/log.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#pragma execution_character_set("utf-8")

using namespace core;


static unsigned int compileShader(unsigned int type, const std::string& source) {
    Log<<Level::Info<<"compileShader() "<<source<<op::endl;
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // 检查编译错误
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        Log<<Level::Error<<"Failed to compile shader!"<<op::endl;
        Log<<Level::Error<<message<<op::endl;
        delete[] message;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

Shader::Shader(const std::string& VertexShader, const std::string& fragmentShader) {
    init(VertexShader, fragmentShader);
}

Shader::Shader(const Shader& shader) {
    Log<<Level::Info<<"Shader::Shader(const Shader&) "<<shader.ID<<"to"<<ID<<op::endl;
    if(this != &shader) {
        if(ID!=0)
            glDeleteProgram(ID);
        init(shader.VertexShader, shader.fragmentShader);
    }
}

Shader::Shader() : ID(0) {}

Shader::~Shader() {
    Log<<Level::Info<<"Shader::~Shader() "<<ID<<op::endl;
    glDeleteProgram(ID);
    ID = 0;
}

void Shader::init(const std::string& VertexShader, const std::string& fragmentShader) {
    Log<<Level::Info<<"Shader::init() "<<op::endl;
    if (ID != 0)
        glDeleteProgram(ID);
    this->VertexShader = VertexShader;
    this->fragmentShader = fragmentShader;

    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, VertexShader);
    Log<<Level::Info<<"Shader::init() vs "<<vs<<op::endl;
    if (vs == 0) {
        Log<<Level::Error<<"Shader::init() vs compile error "<<op::endl;
        return;
    }
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    Log<<Level::Info<<"Shader::init() fs "<<fs<<op::endl;
    if (fs == 0) {
        Log<<Level::Error<<"Shader::init() fs compile error "<<op::endl;
        glDeleteShader(vs);
        return;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    Log<<Level::Info<<"Shader::init() program "<<program<<op::endl;
    // 检查链接错误
    int linkResult;
    glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
    if (linkResult == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetProgramInfoLog(program, length, &length, message);
        Log<<Level::Error<<"Shader::init() Failed to link program!"<<op::endl;
        Log<<Level::Error<<message<<op::endl;
        delete[] message;
        glDeleteProgram(program);
        return;
    }
    ID = program;
}

void Shader::use() const {
    Log<<Level::Info<<"Shader::use() "<<ID<<op::endl;
    if(ID == 0) {
        Log<<Level::Error<<"Shader::use() ID is 0"<<op::endl;
    }
    glUseProgram(ID);
}

Shader& Shader::operator=(const Shader& shader) {
    Log<<Level::Info<<"Shader::operator=() "<<shader.ID<<" to "<<ID<<op::endl;
    if (this != &shader) {
        if (ID != 0)
            glDeleteProgram(ID);
        init(shader.VertexShader, shader.fragmentShader);
    }
    return *this;
}