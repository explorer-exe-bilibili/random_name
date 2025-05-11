#include "mainloop.h"

#include "core/log.h"
#pragma execution_character_set("utf-8")

int mainloop(GLFWwindow* window) {
    // 清除颜色和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 交换前后缓冲区
    glfwSwapBuffers(window);
    
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
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Mouse button released at (" << x << ", " << y << "): " << button << op::endl;
            break;
        default:
            break;
    }
}