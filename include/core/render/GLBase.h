#pragma once

#ifdef DEBUG_MODE
#define ASSERT(x) if (!(x)) __debugbreak()

#define GLCall(a) GLClearError();\
	a;\
	ASSERT(GLLogCall(#a,__FILE__,__LINE__))
#else
#define ASSERT(x) do { } while (0)
#define GLCall(a) a
#endif
void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);