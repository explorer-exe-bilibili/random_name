#include "mainloop.h"

#include <memory>

#include "core/log.h"
#include "core/explorer.h"
#include "core/render/GLBase.h"
#include "core/baseItem/Base.h"
#include "core/screen/mainScreen.h"
#include "core/Drawer.h"

using namespace core;

screen::Screen *currentScreen = nullptr;

// 窗口大小改变时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // 更新screenInfo的相关变量
    WindowInfo.width = width;
    WindowInfo.height = height;
    WindowInfo.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    // 设置OpenGL视口大小
    GLCall(glViewport(0, 0, width, height));
    
    Log << Level::Info << "Window resized to " << width << "x" << height << op::endl;
}

int mainloop() {
    GLCall(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
    
    // 清除颜色和深度缓冲区
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // 设置视口和投影矩阵（如果需要）
    int width, height;
    glfwGetFramebufferSize(WindowInfo.window, &width, &height);
    GLCall(glViewport(0, 0, width, height));

    currentScreen->Draw();
    // 交换前后缓冲区
    glfwSwapBuffers(WindowInfo.window);

    // 处理事件
    glfwPollEvents();

    return 0;
}

void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            Log << Level::Info << "Key pressed: " << key << op::endl;
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Key released: " << key << op::endl;
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;
                default:
                    break;
            }
            break;
        case GLFW_REPEAT:
            Log << Level::Info << "Key repeated: " << key << op::endl;
            break;
        default:
            break;
    }
}

void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    switch (action) {
        case GLFW_PRESS:
            Log << Level::Info << "Mouse button pressed at (" << x << ", " << y << "): " << button << op::endl;
            if (currentScreen) {
                currentScreen->Click(static_cast<int>(x), static_cast<int>(y));
            }
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Mouse button released at (" << x << ", " << y << "): " << button << op::endl;
            break;
        default:
            break;
    }
}
