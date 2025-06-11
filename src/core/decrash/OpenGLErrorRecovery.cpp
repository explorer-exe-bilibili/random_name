#include "core/decrash/OpenGLErrorRecovery.h"
#include "core/decrash/ErrorRecovery.h"
#include "core/log.h"
#include <stdexcept>

namespace core {

std::atomic<int> OpenGLErrorRecovery::contextLostCount{0};
std::atomic<bool> OpenGLErrorRecovery::isRecoveringContext{false};
std::atomic<bool> OpenGLErrorRecovery::contextValid{true};
std::atomic<int> OpenGLErrorRecovery::swapFailures{0};

bool OpenGLErrorRecovery::checkAndRecoverOpenGLErrors() {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        return true;
    }
    
    try {
        return ErrorRecovery::executeWithRetry<bool>([]() {
            return handleOpenGLError();
        }, {.maxRetries = 3}, ErrorRecovery::ErrorType::OPENGL_CONTEXT);
    } catch (const std::exception& e) {
        Log << Level::Error << "OpenGL错误恢复失败: " << e.what() << op::endl;
        return false;
    }
}

bool OpenGLErrorRecovery::recoverLostContext(GLFWwindow* window) {
    if (isRecoveringContext.load()) {
        return false;
    }
    
    isRecoveringContext = true;
    contextLostCount++;
    contextValid = false;

    Log << Level::Warn << "尝试恢复丢失的 OpenGL 上下文 (第" << contextLostCount.load() << "次)" << op::endl;

    if (contextLostCount.load() > MAX_CONTEXT_LOST) {
        Log << Level::Error << "OpenGL上下文丢失次数过多，放弃恢复" << op::endl;
        isRecoveringContext = false;
        return false;
    }
    
    try {
        // 1. 重新设置当前上下文
        glfwMakeContextCurrent(window);
        
        // 2. 检查上下文是否有效
        if (!glfwGetCurrentContext()) {
            throw std::runtime_error("无法恢复 OpenGL 上下文");
        }
        
        // 3. 重新初始化 OpenGL 状态
        reinitializeOpenGLState();
        
        // 4. 重新加载着色器和纹理
        reloadGraphicsResources();
        
        contextValid = true;
        isRecoveringContext = false;
        Log << Level::Info << "OpenGL 上下文恢复成功" << op::endl;
        return true;
        
    } catch (const std::exception& e) {
        contextValid = false;
        isRecoveringContext = false;
        Log << Level::Error << "OpenGL 上下文恢复失败: " << e.what() << op::endl;
        return false;
    }
}

void OpenGLErrorRecovery::safeSwapBuffers(GLFWwindow* window) {
    if (!window || glfwWindowShouldClose(window)) {
        return;
    }
    
    try {
        if (!glfwGetCurrentContext()) {
            if (!recoverLostContext(window)) {
                throw std::runtime_error("无法恢复 OpenGL 上下文");
            }
        }
        
        // 确保所有 OpenGL 命令完成
        glFinish();
        
        // 检查OpenGL错误
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            Log << Level::Warn << "交换缓冲区前检测到OpenGL错误: " << error << op::endl;
        }
        
        glfwSwapBuffers(window);
        swapFailures = 0; // 重置失败计数
        
    } catch (const std::exception& e) {
        swapFailures++;
        Log << Level::Error << "缓冲区交换失败: " << e.what() 
            << " (失败次数: " << swapFailures.load() << ")" << op::endl;
        
        if (swapFailures.load() >= MAX_SWAP_FAILURES) {
            Log << Level::Error << "缓冲区交换连续失败，程序即将退出" << op::endl;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

bool OpenGLErrorRecovery::isContextValid() {
    return contextValid.load();
}

void OpenGLErrorRecovery::markContextInvalid() {
    contextValid = false;
}

void OpenGLErrorRecovery::reset() {
    contextLostCount = 0;
    isRecoveringContext = false;
    contextValid = true;
    swapFailures = 0;
}

bool OpenGLErrorRecovery::handleOpenGLError() {
    GLenum error = glGetError();
    
    switch (error) {
        case GL_OUT_OF_MEMORY:
            Log << Level::Error << "OpenGL 内存不足" << op::endl;
            freeUnusedResources();
            return false;
            
        case GL_CONTEXT_LOST:
            Log << Level::Error << "OpenGL 上下文丢失" << op::endl;
            contextValid = false;
            throw std::runtime_error("OpenGL 上下文丢失");
            
        case GL_INVALID_OPERATION:
            Log << Level::Warn << "OpenGL 无效操作" << op::endl;
            return true; // 通常可以继续
            
        default:
            Log << Level::Warn << "OpenGL 错误: " << error << op::endl;
            return true; // 其他错误通常可以继续
    }
}

void OpenGLErrorRecovery::freeUnusedResources() {
    Log << Level::Info << "释放未使用的OpenGL资源" << op::endl;
    // 这里可以添加具体的资源清理逻辑
    // 例如删除未使用的纹理、缓冲区等
}

void OpenGLErrorRecovery::reinitializeOpenGLState() {
    Log << Level::Info << "重新初始化OpenGL状态" << op::endl;
    
    // 重新设置基本的OpenGL状态
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 检查错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        Log << Level::Warn << "重新初始化OpenGL状态时发生错误: " << error << op::endl;
    }
}

void OpenGLErrorRecovery::reloadGraphicsResources() {
    Log << Level::Info << "重新加载图形资源" << op::endl;
    // 这里可以添加重新加载着色器、纹理等资源的逻辑
}

} // namespace core
