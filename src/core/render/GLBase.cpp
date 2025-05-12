#include "core/render/GLBase.h"
#include "core/log.h"

#include <iostream>
//#include <GL/glew.h>
#include <glad/glad.h>
void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		Log<<Level::Error<<"[OpenGL Error] (" << error << ")" << function << " " << file << ":" << line << op::endl;
		return false;
	}
	return true;
}