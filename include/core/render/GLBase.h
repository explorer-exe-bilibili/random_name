#pragma once

#ifdef DEBUG_MODE
#define ASSERT(x) if (!(x)) __debugbreak()

// 检查OpenGL上下文是否有效的辅助函数声明
bool IsOpenGLContextValid();

#define GLCall(a) \
    if (IsOpenGLContextValid()) { \
        GLClearError(#a,__FILE__,__LINE__); \
        a; \
        ASSERT(GLLogCall(#a,__FILE__,__LINE__)); \
    }
#else
#define ASSERT(x) do { } while (0)
#define GLCall(a) a
#endif
void GLClearError(const char* function, const char* file, int line);

bool GLLogCall(const char* function, const char* file, int line);

void SetOpenGLContextInvalid();
