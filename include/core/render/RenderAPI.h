#pragma once

#include <GLFW/glfw3.h>

namespace core {

class RenderAPI {
public:
    static RenderAPI& Get();

    void Finish();
    void SwapBuffers(GLFWwindow* window);
    void Viewport(int x, int y, int width, int height);
    void ClearColor(float r, float g, float b, float a);
    void Clear(unsigned int mask);

private:
    RenderAPI();
};

} // namespace core
