#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../log.h"

#ifdef _DEBUG
# define GLCall(x) do { while (glGetError() != GL_NO_ERROR); x;\
    while (GLenum error = glGetError()) {\
        Log<<Level::Error<<"OpenGL Error: {0}", error;\
        Log<<"Function: {0}"<< #x;\
        Log<<"File: {0}"<< __FILE__;\
        Log<<"Line: {0}"<< __LINE__<<op::endl;\
    }\
} while (0)
#else
# define GLCall(x) do { x; } while (0)
#endif