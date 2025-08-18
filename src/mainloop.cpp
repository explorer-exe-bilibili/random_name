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
#include "core/decrash/ErrorRecovery.h"
#include "core/decrash/OpenGLErrorRecovery.h"
#include "core/decrash/MemoryMonitor.h"

using namespace core;

// 初始化FPS相关变量
double lastFPSUpdateTime = 0.0;
int frameCount = 0;
double currentFPS = 0.0;

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
    static int consecutiveErrors = 0;
    const int MAX_CONSECUTIVE_ERRORS = 10;
    
    try {
        // 检查并恢复 OpenGL 错误
        if (!OpenGLErrorRecovery::checkAndRecoverOpenGLErrors()) {
            consecutiveErrors++;
            if (consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
                Log << Level::Error << "连续 OpenGL 错误过多，程序退出" << op::endl;
                glfwSetWindowShouldClose(WindowInfo.window, GLFW_TRUE);
                return -1;
            }
            return 0;
        }
        
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

        // 安全地执行渲染
        executeRenderingWithRecovery();
        
        // 如果启用了FPS显示，则绘制FPS文本
        if (bools[boolconfig::show_fps]) {
            // 创建FPS文本
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << "FPS: " << currentFPS;
            std::string fpsText = ss.str();
            
            // 使用Explorer获取默认字体并渲染FPS文本
            Font** font = Explorer::getInstance()->getFontPtr(FontID::Default); // 使用默认字体
            if (font && *font) {
                // 在屏幕右上角渲染FPS文本
                (*font)->RenderText(fpsText, 0, 0, 0.5f, color::black);
            }
        }
        
        // 安全地交换缓冲区
        OpenGLErrorRecovery::safeSwapBuffers(WindowInfo.window);

        // 处理事件
        glfwPollEvents();
        
        consecutiveErrors = 0; // 重置错误计数
        
    } catch (const std::exception& e) {
        consecutiveErrors++;
        Log << Level::Error << "主循环异常: " << e.what() 
            << " (连续错误: " << consecutiveErrors << ")" << op::endl;
        
        if (consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
            Log << Level::Error << "连续错误过多，程序退出" << op::endl;
            glfwSetWindowShouldClose(WindowInfo.window, GLFW_TRUE);
            return -1;
        }
        
        // 尝试恢复
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT:
            Log << Level::Info << "Key " << (action == GLFW_PRESS ? "pressed" : "repeated") << ": " << key << op::endl;
            
            // 将键盘输入传递给当前屏幕
            if (screen::Screen::getCurrentScreen()) {
                // 只处理控制键，ASCII可打印字符交给CharEvent处理
                char keyChar = 0;
                
                if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
                    keyChar = '\r';
                }
                else if (key == GLFW_KEY_BACKSPACE) {
                    keyChar = '\b';
                }
                else if (key == GLFW_KEY_ESCAPE) {
                    keyChar = 27;
                }
                // 注意：ASCII可打印字符（字母、数字、标点等）现在由CharEvent处理，避免重复输入
                
                if (keyChar != 0) {
                    if (screen::Screen::getCurrentScreen()->HandleKeyInput(keyChar)) {
                        return; // 如果屏幕处理了输入，就不继续处理全局快捷键
                    }
                }
            }
            
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Key released: " << key << op::endl;
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    // 只有在没有屏幕处理ESC时才退出程序
                    if (!screen::Screen::getCurrentScreen() || 
                        !screen::Screen::getCurrentScreen()->HandleKeyInput(27)) {
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                    }
                    break;
                case GLFW_KEY_F:
                    // 按F键切换FPS显示
                    if (mods & GLFW_MOD_CONTROL) {
                        bools[boolconfig::show_fps] = !bools[boolconfig::show_fps];
                        Log << Level::Info << "FPS display " << (bools[boolconfig::show_fps] ? "enabled" : "disabled") << op::endl;
                    }
                    break;
                case GLFW_KEY_F2:
                    // 按F2切换编辑模式
                    {
                        auto currentScreen = screen::Screen::getCurrentScreen();
                        if (currentScreen) {
                            bool currentEditMode = currentScreen->IsEditModeEnabled();
                            bool newEditMode = !currentEditMode;
                            
                            currentScreen->SetEditMode(newEditMode);
                            
                            Log << Level::Info << "UI Edit mode " << (newEditMode ? "enabled" : "disabled") << op::endl;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    auto currentScreen = screen::Screen::getCurrentScreen();
    
    if (!currentScreen) return;
    
    int mouseX = static_cast<int>(x);
    int mouseY = static_cast<int>(y);
    
    switch (action) {
        case GLFW_PRESS:
            Log << Level::Info << "Mouse button pressed at (" << x << ", " << y << "): " << button << op::endl;
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (currentScreen->IsEditModeEnabled()) {
                    currentScreen->OnEditMouseDown(mouseX, mouseY);
                } else {
                    // 正常点击处理已经在RELEASE事件中完成
                }
            }
            break;
        case GLFW_RELEASE:
            Log << Level::Info << "Mouse button released at (" << x << ", " << y << "): " << button << op::endl;
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (currentScreen->IsEditModeEnabled()) {
                    currentScreen->OnEditMouseUp(mouseX, mouseY);
                } else {
                    currentScreen->Click(mouseX, mouseY);
                }
            }
            break;
        default:
            break;
    }
}

// 鼠标移动事件处理
void MouseMoveEvent(GLFWwindow* window, double xpos, double ypos) {
    auto currentScreen = screen::Screen::getCurrentScreen();
    if (!currentScreen) return;
    
    // 只有在编辑模式下且鼠标左键按下时才处理移动事件
    if (currentScreen->IsEditModeEnabled() && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        int mouseX = static_cast<int>(xpos);
        int mouseY = static_cast<int>(ypos);
        currentScreen->OnEditMouseMove(mouseX, mouseY);
    }
}

// 处理Unicode字符输入的回调函数（支持中文等多字节字符）
void CharEvent(GLFWwindow* window, unsigned int codepoint) {
    // 将Unicode码点转换为UTF-8字符串
    std::string utf8_char;
    
    if (codepoint < 0x80) {
        // ASCII字符
        utf8_char += static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
        // 2字节UTF-8
        utf8_char += static_cast<char>(0xC0 | (codepoint >> 6));
        utf8_char += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {
        // 3字节UTF-8 (大部分中文字符)
        utf8_char += static_cast<char>(0xE0 | (codepoint >> 12));
        utf8_char += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        utf8_char += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
        // 4字节UTF-8
        utf8_char += static_cast<char>(0xF0 | (codepoint >> 18));
        utf8_char += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        utf8_char += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        utf8_char += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    
    Log << Level::Debug << "Unicode character input: U+"  << codepoint  << " -> UTF-8: " << utf8_char << op::endl;
    
    // 将UTF-8字符传递给当前屏幕
    if (screen::Screen::getCurrentScreen()) {
        if (screen::Screen::getCurrentScreen()->HandleUnicodeInput(utf8_char)) {
            return; // 如果屏幕处理了输入，就不继续处理
        }
    }
}

void executeRenderingWithRecovery() {
    static int renderFailures = 0;
    const int MAX_RENDER_FAILURES = 5;
    
    try {
        // 绘制场景
        // 使用屏幕管理系统的当前屏幕
        if (screen::Screen::getCurrentScreen()) {
            screen::Screen::getCurrentScreen()->Draw();
            if(bools[boolconfig::debug]) {
                for(auto i=0;i<=4000;i+=50) {
                    Drawer::getInstance()->DrawLine(Point(0,i,false),Point(WindowInfo.width,i,false),Color(255,255,255,255));
                }
            }
        } else {
            Log << Level::Error << "当前屏幕为空，无法绘制" << op::endl;
        }
        
        renderFailures = 0; // 重置失败计数
        
    } catch (const std::exception& e) {
        renderFailures++;
        Log << Level::Warn << "渲染失败: " << e.what() 
            << " (失败次数: " << renderFailures << ")" << op::endl;
        
        if (renderFailures >= MAX_RENDER_FAILURES) {
            Log << Level::Error << "渲染失败次数过多，尝试重新初始化图形资源" << op::endl;
            
            // 重新初始化图形资源
            if (OpenGLErrorRecovery::recoverLostContext(WindowInfo.window)) {
                renderFailures = 0;
            } else {
                throw std::runtime_error("无法恢复图形渲染");
            }
        }
    }
}
