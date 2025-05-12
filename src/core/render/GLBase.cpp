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
    g_OpenGLContextValid = false;
    Log<<Level::Info<<"OpenGL context marked as invalid"<<op::endl;
}

void GLClearError(const char* function, const char* file, int line) {
    // 最多处理10个错误，避免死循环
    int maxErrors = 10;
    int errorCount = 0;
    
	while (GLenum error = glGetError()) {
		Log<<Level::Warn<<"[OpenGL Error]("<<error<<")" << function << " " << file << ":" << line << op::endl;
		
		// 增加计数器并检查是否达到最大值
		if (++errorCount >= maxErrors) {
            Log<<Level::Warn<<"Too many OpenGL errors, breaking loop" << op::endl;
            break;
        }
		
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}	
}

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		Log<<Level::Error<<"[OpenGL Error] (" << error << ")" << function << " " << file << ":" << line << op::endl;
		return false;
	}
	return true;
}