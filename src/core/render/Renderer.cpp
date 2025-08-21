#include "core/render/Renderer.h"
#include "core/render/GLBase.h"

namespace core {

Renderer& Renderer::Get() {
    static Renderer instance;
    return instance;
}

Renderer::Renderer() {
    m_backend = Backend::Unknown;
}

void Renderer::CallWithError(const std::function<void()>& fn, const char* exprStr, const char* file, int line) const {
    if (IsOpenGLActive()) {
        GLClearError(exprStr, file, line);
        fn();
        GLLogCall(exprStr, file, line);
    } else {
        // Non-OpenGL backend: just execute without GL error checks
        fn();
    }
}

} // namespace core
