#include"core/render/Shader.h"
#include "core/render/GLBase.h"
#include "core/decrash/OpenGLErrorRecovery.h"
#include "core/log.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>


using namespace core;


static unsigned int compileShader(unsigned int type, const std::string& source) {
    Log<<Level::Info<<"compileShader() "<<source<<op::endl;
    unsigned int id;
    GLCall(id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // 检查编译错误
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = new char[length];
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        Log<<Level::Error<<"Failed to compile shader!"<<op::endl;
        Log<<Level::Error<<message<<op::endl;
        delete[] message;
        GLCall(glDeleteShader(id));
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
        if(ID != 0) {
            if (core::OpenGLErrorRecovery::isContextValid()) {
                GLCall(glDeleteProgram(ID));
            }
        }
        init(shader.VertexShader, shader.fragmentShader);
    }
}

Shader::Shader() : ID(0) {}

Shader::~Shader() {
    Log<<Level::Info<<"Shader::~Shader() "<<ID<<op::endl;
    
    // 只有在ID有效时才删除程序
    if (ID != 0) {
        // 使用OpenGL错误恢复系统安全删除程序        
        if (core::OpenGLErrorRecovery::isContextValid()) {
            try {
                GLCall(glDeleteProgram(ID));
                Log<<Level::Info<<"Shader program "<<ID<<" deleted successfully"<<op::endl;
            } catch (const std::exception& e) {
                Log<<Level::Error<<"Exception occurred while deleting shader program "<<ID<<": "<<e.what()<<op::endl;
            } catch (...) {
                Log<<Level::Error<<"Unknown exception occurred while deleting shader program "<<ID<<op::endl;
            }
        } else {
            Log<<Level::Warn<<"OpenGL context invalid, skipping shader program "<<ID<<" deletion"<<op::endl;
        }
        ID = 0;
    }
    
    Log<<Level::Info<<"Shader::~Shader() finished ID "<<ID<<op::endl;
}

void Shader::init(const std::string& VertexShader, const std::string& fragmentShader) {
    Log<<Level::Info<<"Shader::init() "<<op::endl;
    
    // 检查OpenGL上下文是否有效
    if (!core::OpenGLErrorRecovery::isContextValid()) {
        Log<<Level::Error<<"Shader::init() OpenGL context is invalid"<<op::endl;
        return;
    }
    
    if (ID != 0) {
        GLCall(glDeleteProgram(ID));
    }
    this->VertexShader = VertexShader;
    this->fragmentShader = fragmentShader;

    unsigned int program;
    GLCall(program = glCreateProgram());
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
        GLCall(glDeleteShader(vs));
        return;
    }

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));
    Log<<Level::Info<<"Shader::init() program "<<program<<op::endl;
    // 检查链接错误
    int linkResult;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linkResult));
    if (linkResult == GL_FALSE) {
        int length;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        char* message = new char[length];
        GLCall(glGetProgramInfoLog(program, length, &length, message));
        Log<<Level::Error<<"Shader::init() Failed to link program!"<<op::endl;
        Log<<Level::Error<<message<<op::endl;
        delete[] message;
        GLCall(glDeleteProgram(program));
        return;
    }
    ID = program;
}

void Shader::use() const {
    if(ID == 0) {
        Log<<Level::Error<<"Shader::use() ID is 0"<<op::endl;
        return;
    }
    
    // 检查OpenGL上下文是否有效
    if (!core::OpenGLErrorRecovery::isContextValid()) {
        Log<<Level::Error<<"Shader::use() OpenGL context is invalid"<<op::endl;
        return;
    }
    
    GLCall(glUseProgram(ID));
}

Shader& Shader::operator=(const Shader& shader) {
    Log<<Level::Info<<"Shader::operator=() "<<shader.ID<<" to "<<ID<<op::endl;
    if (this != &shader) {
        if (ID != 0) {
            if (core::OpenGLErrorRecovery::isContextValid()) {
                GLCall(glDeleteProgram(ID));
            }
        }
        init(shader.VertexShader, shader.fragmentShader);
    }
    return *this;
}