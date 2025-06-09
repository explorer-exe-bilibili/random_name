#pragma once

#include <atomic>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace core {

class OpenGLErrorRecovery {
public:
    static bool checkAndRecoverOpenGLErrors();
    static bool recoverLostContext(GLFWwindow* window);
    static void safeSwapBuffers(GLFWwindow* window);
    static bool isContextValid();
    static void markContextInvalid();
    static void reset();

private:
    static std::atomic<int> contextLostCount;
    static std::atomic<bool> isRecoveringContext;
    static std::atomic<bool> contextValid;
    static std::atomic<int> swapFailures;
    
    static bool handleOpenGLError();
    static void freeUnusedResources();
    static void reinitializeOpenGLState();
    static void reloadGraphicsResources();
    
    static const int MAX_CONTEXT_LOST = 5;
    static const int MAX_SWAP_FAILURES = 3;
};

} // namespace core
