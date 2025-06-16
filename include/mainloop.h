#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "core/log.h"
#include "core/baseItem/Base.h"
#include "core/screen/base.h"
#include "core/explorer.h"
#include "core/Config.h"

int mainloop();
void executeRenderingWithRecovery();
void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
void CharEvent(GLFWwindow* window, unsigned int codepoint); // Unicode字符输入回调
void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int init();
int cleanup();

void SetConfigItems();

// FPS相关变量，用于计算和显示帧率
extern double lastFPSUpdateTime; // 上次更新FPS的时间
extern int frameCount;           // 帧计数器
extern double currentFPS;        // 当前FPS值
extern bool showFPS;             // 是否显示FPS