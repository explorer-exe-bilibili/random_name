#pragma once
#include "core/render/IFontRenderer.h"
#include "core/render/Shader.h"
#include <GLFW/glfw3.h>
#include <memory>

namespace core {

class OpenGLFontRenderer : public IFontRenderer {
public:
    OpenGLFontRenderer();
    ~OpenGLFontRenderer() override;

    bool Initialize(void* windowHandle) override;
    TextureId CreateGlyphTexture(int width, int height, const unsigned char* data) override;
    void DeleteTexture(TextureId id) override;
    void BindTexture(TextureId id) override;
    void SetProjection(const glm::mat4& projection) override;
    void SetTextColor(const glm::vec3& color, float alphaMultiplier) override;
    void PrepareForText() override;
    void UpdateVertexBuffer(const void* data, size_t size) override;
    void DrawTriangles(int vertexCount) override;
    void Shutdown() override;

private:
    GLFWwindow* m_window = nullptr;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    // 使用项目已有 Shader 类（保持一致）
    core::Shader m_shader;
    glm::mat4 m_projection;
    glm::vec3 m_textColor;
    float m_alpha = 1.0f;
    bool m_initialized = false;
};

} // namespace core
