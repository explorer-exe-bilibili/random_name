#include "core/render/GLBase.h"
#include "core/log.h"

#include <iostream>
//#include <GL/glew.h>
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
    
	while (GLenum error = glGetError()) {
		Log<<Level::Warn<<"[OpenGL Error]("<<error<<")" << function << " " << file << ":" << line << op::endl;
	}	
}

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		Log<<Level::Error<<"[OpenGL Error] (" << error << ")" << function << " " << file << ":" << line << op::endl;
		return false;
	}
	return true;
}