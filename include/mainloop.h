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

int mainloop();
void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods);

int init();
int cleanup();