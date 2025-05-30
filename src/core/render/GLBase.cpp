#include "core/render/GLBase.h"
#include "core/log.h"

#include <iostream>
#include <glad/glad.h>

// 用于标记OpenGL上下文状态的全局变量
static bool g_OpenGLContextValid = true;

bool IsOpenGLContextValid() {
    return g_OpenGLContextValid;
}

void SetOpenGLContextInvalid() {
    if (g_OpenGLContextValid) {
        g_OpenGLContextValid = false;
        Log<<Level::Info<<"OpenGL context marked as invalid"<<op::endl;
        
        // 清除所有待处理的OpenGL错误，防止它们在后续调用中被报告
        while (glGetError() != GL_NO_ERROR) {
            // 仅清除错误，不记录
        }
    }
}

void GLClearError(const char* function, const char* file, int line) {
    // 只清除错误，不打印日志
    // 这可以避免日志被大量错误消息淹没
    while (glGetError() != GL_NO_ERROR) {
        // 仅清除错误，不记录
    }
}

bool GLLogCall(const char* function, const char* file, int line)
{
    using namespace core;
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::string errorName;
        switch (error) {
            case GL_INVALID_ENUM:      errorName = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE:     errorName = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: errorName = "GL_INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:    errorName = "GL_STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:   errorName = "GL_STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:     errorName = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: errorName = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            default:                   errorName = "UNKNOWN_ERROR"; break;
        }

        Log << Level::Error << "[OpenGL Error] (" << errorName << ": " << error << "): " 
            << function << " " << file << ":" << line << op::endl;
        return false;
    }
    return true;
}