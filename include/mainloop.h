#pragma once
#pragma execution_character_set("utf-8")

#include<glad/glad.h>
#include <GLFW/glfw3.h>

int mainloop(GLFWwindow* window);
void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
