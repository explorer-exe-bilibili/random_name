#pragma once


#define ASSERT(x) if (!(x)) __debugbreak()

#define GLCall(a) GLClearError();\
	a;\
	ASSERT(GLLogCall(#a,__FILE__,__LINE__))


void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);