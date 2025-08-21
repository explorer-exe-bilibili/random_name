#pragma once

#include <functional>

namespace core {

class Renderer {
public:
    enum class Backend { OpenGL, Vulkan, Unknown };

    static Renderer& Get();

    void SetBackend(Backend b) { m_backend = b; }
    Backend GetBackend() const { return m_backend; }

    bool IsOpenGLActive() const { return m_backend == Backend::OpenGL; }

    // Execute callable; when OpenGL backend is active, the GL error helpers will be used.
    void CallWithError(const std::function<void()>& fn, const char* exprStr, const char* file, int line) const;

private:
    Renderer();
    Backend m_backend = Backend::Unknown;
};

} // namespace core
