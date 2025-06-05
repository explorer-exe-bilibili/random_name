#include "mainloop.h"

#include <memory>
#include <string>
#include <iomanip>
#include <sstream>

#include "core/log.h"
#include "core/explorer.h"
#include "core/render/GLBase.h"
#include "core/baseItem/Base.h"
#include "core/screen/mainScreen.h"
#include "core/Drawer.h"

using namespace core;

// 初始化FPS相关变量
double lastFPSUpdateTime = 0.0;
int frameCount = 0;
double currentFPS = 0.0;
bool showFPS = true; // 默认显示FPS

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

    // FPS计算
    // 获取当前时间
    double currentTime = glfwGetTime();
    frameCount++;
    
    // 每秒更新一次FPS值
    if (currentTime - lastFPSUpdateTime >= 1.0) {
        currentFPS = frameCount / (currentTime - lastFPSUpdateTime);
        // 重置计数器
        frameCount = 0;
        lastFPSUpdateTime = currentTime;
        
        // 输出FPS日志
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << currentFPS;
        Log << Level::Debug << "FPS: " << ss.str() << op::endl;
        Log<<Level::Info<<"Current screen :"<<(int)screen::Screen::getCurrentScreen()->getID()<<op::endl;
    }

    // 绘制场景
    // 使用屏幕管理系统的当前屏幕
    if (screen::Screen::getCurrentScreen()) {
        screen::Screen::getCurrentScreen()->Draw();
    } else {
        Log << Level::Error << "当前屏幕为空，无法绘制" << op::endl;
    }
    
    // 如果启用了FPS显示，则绘制FPS文本
    if (showFPS) {
        // 创建FPS文本
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << "FPS: " << currentFPS;
        std::string fpsText = ss.str();
        
        // 使用Explorer获取默认字体并渲染FPS文本
        Font* font = Explorer::getInstance()->getFont(FontID::Default); // 使用默认字体
        if (font) {
            // 在屏幕右上角渲染FPS文本
            font->RenderText(fpsText, 0, 0, 0.5f, color::black);
        }
    }
    
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
                case GLFW_KEY_F:
                    // 按F键切换FPS显示
                    if (mods & GLFW_MOD_CONTROL) {
                        showFPS = !showFPS;
                        Log << Level::Info << "FPS display " << (showFPS ? "enabled" : "disabled") << op::endl;
                    }
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
            screen::Screen::getCurrentScreen()->Click(static_cast<int>(x), static_cast<int>(y));
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Mouse button released at (" << x << ", " << y << "): " << button << op::endl;
            break;
        default:
            break;
    }
}
