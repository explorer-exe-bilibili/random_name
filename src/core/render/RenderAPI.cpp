#include <glad/glad.h>
#include "core/render/RenderAPI.h"
#include "core/render/GLBase.h"

namespace core {

RenderAPI& RenderAPI::Get() {
    static RenderAPI instance;
    return instance;
}

RenderAPI::RenderAPI() {}

void RenderAPI::Finish() {
    GLCall(glFinish());
}

void RenderAPI::SwapBuffers(GLFWwindow* window) {
    glfwSwapBuffers(window);
}

void RenderAPI::Viewport(int x, int y, int width, int height) {
    GLCall(glViewport(x, y, width, height));
}

void RenderAPI::ClearColor(float r, float g, float b, float a) {
    GLCall(glClearColor(r, g, b, a));
}

void RenderAPI::Clear(unsigned int mask) {
    GLCall(glClear(mask));
}

} // namespace core
