#include "OpenGLFontRenderer.h"
#include "core/render/Shader.h"
#include <glad/glad.h>
#include <iostream>

using namespace core;

static const std::string vertexSrc = R"(
#version 330 core
layout(location = 0) in vec4 vertex;
out vec2 TexCoords;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

static const std::string fragmentSrc = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D text;
uniform vec3 textColor;
uniform float alphaMultiplier;
void main() {
    float alpha = texture(text, TexCoords).r * alphaMultiplier;
    color = vec4(textColor, alpha);
}
)";

OpenGLFontRenderer::OpenGLFontRenderer() {}

OpenGLFontRenderer::~OpenGLFontRenderer() {
    Shutdown();
}

bool OpenGLFontRenderer::Initialize(void* windowHandle) {
    m_window = reinterpret_cast<GLFWwindow*>(windowHandle);
    // 依赖外部已加载 glad 并且已有当前 OpenGL context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GL loader in OpenGLFontRenderer" << std::endl;
        return false;
    }
    // 创建 shader
    m_shader.init(vertexSrc, fragmentSrc);

    // 创建 VAO/VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    // layout location 0: vec4 (x,y, u, v)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_initialized = true;
    // 标记当前 renderer 为 OpenGL，这样 GLBase 的 GLCall 会启用错误检查路径
    core::Renderer::Get().SetBackend(core::Renderer::Backend::OpenGL);
    return true;
}

IFontRenderer::TextureId OpenGLFontRenderer::CreateGlyphTexture(int width, int height, const unsigned char* data) {
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // 单通道
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return static_cast<TextureId>(tex);
}

void OpenGLFontRenderer::DeleteTexture(TextureId id) {
    GLuint tex = static_cast<GLuint>(id);
    if (tex != 0) {
        glDeleteTextures(1, &tex);
    }
}

void OpenGLFontRenderer::BindTexture(TextureId id) {
    GLuint tex = static_cast<GLuint>(id);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void OpenGLFontRenderer::SetProjection(const glm::mat4& projection) {
    m_projection = projection;
    m_shader.use();
    m_shader.setMat4("projection", m_projection);
}

void OpenGLFontRenderer::SetTextColor(const glm::vec3& color, float alphaMultiplier) {
    m_textColor = color;
    m_alpha = alphaMultiplier;
    m_shader.use();
    m_shader.setVec3("textColor", m_textColor);
    m_shader.setFloat("alphaMultiplier", m_alpha);
}

void OpenGLFontRenderer::PrepareForText() {
    if (!m_initialized) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    m_shader.use();
    glBindVertexArray(m_vao);
}

void OpenGLFontRenderer::UpdateVertexBuffer(const void* data, size_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLFontRenderer::DrawTriangles(int vertexCount) {
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void OpenGLFontRenderer::Shutdown() {
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    // 不删除 shader（若需要，可增加 cleanup）
    m_initialized = false;
}
