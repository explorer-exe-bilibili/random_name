#pragma once
#include <cstdint>
#include <cstddef>
#include <glm/glm.hpp>

namespace core {

class IFontRenderer {
public:
    using TextureId = uint64_t;

    virtual ~IFontRenderer() = default;

    // 在窗口创建并有上下文后调用（OpenGL 需要 context，Vulkan 可能需要 surface 等）
    // windowHandle 可为 GLFWwindow* 或平台句柄（使用 void* 以便抽象）
    virtual bool Initialize(void* windowHandle) = 0;

    // 创建单通道灰度纹理，data 可为 nullptr（创建空白）
    virtual TextureId CreateGlyphTexture(int width, int height, const unsigned char* data) = 0;

    // 删除纹理
    virtual void DeleteTexture(TextureId id) = 0;

    // 绑定纹理（后端实现）
    virtual void BindTexture(TextureId id) = 0;

    // 设置投影矩阵（每帧设置）
    virtual void SetProjection(const glm::mat4& projection) = 0;

    // 设置文字颜色与 alphaMultiplier
    virtual void SetTextColor(const glm::vec3& color, float alphaMultiplier) = 0;

    // 在渲染文本前调用（例如启用混合、绑定 shader）
    virtual void PrepareForText() = 0;

    // 更新顶点缓冲区（Font 每个字更新 6 顶点）
    virtual void UpdateVertexBuffer(const void* data, size_t size) = 0;

    // 绘制当前顶点缓冲区（按顶点数）
    virtual void DrawTriangles(int vertexCount) = 0;

    // 关闭并释放后端资源
    virtual void Shutdown() = 0;
};

} // namespace core
