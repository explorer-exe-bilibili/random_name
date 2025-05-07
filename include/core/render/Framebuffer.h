#pragma once

class Framebuffer {
private:
    unsigned int m_RendererID;    // OpenGL帧缓冲区ID
    unsigned int m_TextureID;     // 颜色附件纹理ID
    unsigned int m_RenderbufferID; // 深度和模板缓冲区ID
    int m_Width, m_Height;        // 帧缓冲区尺寸

public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;
    void Resize(int width, int height);
    
    // 获取帧缓冲区生成的纹理，可用于后处理
    unsigned int GetTextureID() const { return m_TextureID; }
    
    // 获取帧缓冲区尺寸
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
};