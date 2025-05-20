#include "core/baseItem/Font.h"
#include "core/baseItem/Bitmap.h"
#include "core/render/GLBase.h"
#include "core/log.h"

#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>

using namespace core;

// 将文字渲染到Bitmap对象上
void Font::RenderTextToBitmap(const std::string& text, std::shared_ptr<Bitmap> bitmap, float x, float y, float scale, const glm::vec4& color) {
    std::wstring wtext = string2wstring(text);
    RenderTextToBitmap(wtext, bitmap, x, y, scale, color);
}

void Font::RenderTextToBitmap(const std::wstring& text, std::shared_ptr<Bitmap> bitmap, float x, float y, float scale, const glm::vec4& color) {
    if (!bitmap) {
        Log << Level::Error << "RenderTextToBitmap: Bitmap is null" << op::endl;
        return;
    }

    if (this == nullptr) {
        Log << Level::Error << "RenderTextToBitmap: Font is null" << op::endl;
        if (spare_font) {
            spare_font->RenderTextToBitmap(text, bitmap, x, y, scale, color);
        }
        return;
    }

    // 确保Bitmap中有纹理
    if (!bitmap->HasTexture()) {
        Log << Level::Info << "RenderTextToBitmap: Creating texture for bitmap" << op::endl;
        bitmap->CreateTextureFromBuffer();
        if (!bitmap->HasTexture()) {
            Log << Level::Error << "RenderTextToBitmap: Failed to create texture for bitmap" << op::endl;
            return;
        }
    }

    // 获取Bitmap的宽度和高度
    unsigned int width = bitmap->getWidth();
    unsigned int height = bitmap->getHeight();

    if (width == 0 || height == 0) {
        Log << Level::Error << "RenderTextToBitmap: Invalid bitmap dimensions" << op::endl;
        return;
    }

    // 创建帧缓冲对象(FBO)
    unsigned int fbo;
    GLCall(glGenFramebuffers(1, &fbo));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    // 获取当前视口
    int viewport[4];
    GLCall(glGetIntegerv(GL_VIEWPORT, viewport));
    
    // 设置视口为Bitmap的大小
    GLCall(glViewport(0, 0, width, height));

    // 将Bitmap的纹理绑定到帧缓冲
    unsigned int textureID = bitmap->HasTexture() ? bitmap->GetTextureID() : 0;
    if (textureID == 0) {
        Log << Level::Error << "RenderTextToBitmap: Invalid texture ID" << op::endl;
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glDeleteFramebuffers(1, &fbo));
        GLCall(glViewport(viewport[0], viewport[1], viewport[2], viewport[3]));
        return;
    }

    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0));

    // 检查帧缓冲状态
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log << Level::Error << "RenderTextToBitmap: Framebuffer not complete" << op::endl;
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glDeleteFramebuffers(1, &fbo));
        GLCall(glViewport(viewport[0], viewport[1], viewport[2], viewport[3]));
        return;
    }

    // 保存当前的混合模式
    GLint blendSrc, blendDst;
    GLboolean blendEnabled;
    GLCall(glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc));
    GLCall(glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst));
    GLCall(blendEnabled = glIsEnabled(GL_BLEND));

    // 设置混合模式，使文字可以正确地叠加到位图上
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // 调整y坐标（在OpenGL中，原点在左下角，而在大多数图像处理中，原点在左上角）
    y = height - y;

    // 设置正交投影矩阵
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    
    // 设置字体着色器
    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec3("textColor", color);
    shader.setFloat("alphaMultiplier", color.a);

    // 检查是否包含未加载的字符
    for (auto c = text.begin(); c != text.end(); ++c) {
        if (!Characters.contains(*c)) {
            LoadCharacter(*c);
        }
    }

    // 准备绘制
    GLCall(glActiveTexture(GL_TEXTURE0));
    VAO.Bind();

    // 遍历文本中的字符
    float xpos = x;
    for (auto c = text.begin(); c != text.end(); ++c) {
        Character ch = Characters[*c];

        float charX = xpos + ch.Bearing.x * scale;
        float charY = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // 更新VBO
        float vertices[6][4] = {
            { charX,     charY + h,   0.0f, 0.0f },
            { charX,     charY,       0.0f, 1.0f },
            { charX + w, charY,       1.0f, 1.0f },

            { charX,     charY + h,   0.0f, 0.0f },
            { charX + w, charY,       1.0f, 1.0f },
            { charX + w, charY + h,   1.0f, 0.0f }
        };

        // 绑定字符纹理
        GLCall(glBindTexture(GL_TEXTURE_2D, ch.TextureID));

        // 更新顶点缓冲
        VBO.Bind();
        VBO.BufferSubData(0, sizeof(vertices), vertices);
        VertexBuffer::Unbind();

        // 绘制
        GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

        // 移动到下一个字符位置
        xpos += (ch.Advance >> 6) * scale; // 位移单位是1/64像素，所以位移6位
    }

    // 解绑VAO和纹理
    VertexArray::Unbind();
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    // 恢复原先的混合模式
    if (!blendEnabled) {
        GLCall(glDisable(GL_BLEND));
    } else {
        GLCall(glBlendFunc(blendSrc, blendDst));
    }

    // 解绑帧缓冲并返回到默认的帧缓冲
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCall(glDeleteFramebuffers(1, &fbo));

    // 恢复原先的视口
    GLCall(glViewport(viewport[0], viewport[1], viewport[2], viewport[3]));

    Log << Level::Info << "RenderTextToBitmap: Successfully rendered text to bitmap" << op::endl;
}
