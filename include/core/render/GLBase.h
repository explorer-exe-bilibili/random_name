#pragma once

#include "core/render/Renderer.h"

#ifdef DEBUG_MODE
#ifdef _WIN32
#define ASSERT(x) if (!(x));
/* __debugbreak()*/

// 保持兼容接口，但将调用通过 Renderer shim 转发，以便 render/ 下可替换后端
#define GLCall(a) \
    core::Renderer::Get().CallWithError([&](){ a; }, #a, __FILE__, __LINE__)

#else
#define ASSERT(x) {}
#define GLCall(a) a
#endif

#else
#define ASSERT(x) do { } while (0)
#define GLCall(a) a
#endif

// 保留原先的错误辅助函数声明，实现仍在 src/core/render/GLBase.cpp
void GLClearError(const char* function, const char* file, int line);
bool GLLogCall(const char* function, const char* file, int line);
void SetOpenGLContextInvalid();
bool IsOpenGLContextValid();
