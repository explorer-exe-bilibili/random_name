#include "EasyGL.h"
#include <glad/glad.h>
#include <gl/GL.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Bitmap.h"
#include "VertexBuffer.h"

EasyGL::EasyGL()
{
}

EasyGL::~EasyGL()
{
    Cleanup();
}

bool EasyGL::Init(HWND hwnd)
{
    // 获取设备上下文
    hDC = GetDC(hwnd);
    if (!hDC)
    {
        std::cerr << "获取设备上下文失败！" << std::endl;
        return false;
    }

    // 设置像素格式
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR) };
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // 支持双缓冲
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (!pixelFormat)
    {
        std::cerr << "选择像素格式失败！" << std::endl;
        return false;
    }

    if (!SetPixelFormat(hDC, pixelFormat, &pfd))
    {
        std::cerr << "设置像素格式失败！" << std::endl;
        return false;
    }

    // 创建 OpenGL 渲染上下文
    hRC = wglCreateContext(hDC);
    if (!hRC)
    {
        std::cerr << "创建 OpenGL 渲染上下文失败！" << std::endl;
        return false;
    }

    if (!wglMakeCurrent(hDC, hRC))
    {
        std::cerr << "激活当前 OpenGL 渲染上下文失败！" << std::endl;
        return false;
    }

    // 初始化 GLAD
    if (!gladLoadGL())
    {
        std::cerr << "初始化 GLAD 失败！" << std::endl;
        return false;
    }

    // 设置视口
    RECT rect;
    GetClientRect(hwnd, &rect);
    windowWidth = rect.right - rect.left;
    windowHeight = rect.bottom - rect.top;
    glViewport(0, 0, windowWidth, windowHeight);

    // 启用 Alpha 混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    isInit = true;
    return true;
}

void EasyGL::Cleanup()
{
    if (hRC)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hRC);
        hRC = nullptr;
    }
    if (hDC)
    {
        ReleaseDC(WindowFromDC(hDC), hDC);
        hDC = nullptr;
    }
    isInit = false;
}

void EasyGL::BeginRender() const
{
    if (!isInit) return;

    // 清除颜色缓冲区
    glClearColor(clearColor.R, clearColor.G, clearColor.B, clearColor.A);
    glClear(GL_COLOR_BUFFER_BIT);

    // 设置视口
    glViewport(0, 0, windowWidth, windowHeight);
}

void EasyGL::EndRender() const
{
    if (!isInit) return;

    // 交换前后缓冲区
    SwapBuffers(hDC);
}

void EasyGL::Resize(int width, int height)
{
    if (!isInit) return;

    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, windowWidth, windowHeight);
}

void EasyGL::DrawBitmap(float x, float y, const Bitmap& bitmap, float angle) const
{
    if (!isInit) return;

    // 默认绘制大小为纹理大小，这里可以根据需要调整
    float width = 100.0f;
    float height = 100.0f;
    DrawBitmap(x, y, width, height, bitmap, angle);
}

void EasyGL::DrawBitmap(float x, float y, float width, float height, const Bitmap& bitmap, float angle) const
{
    if (!isInit) return;

    // 将屏幕坐标转换为 OpenGL 坐标（NDC）
    auto screenToNDC = [this](float sx, float sy) {
        float ndcX = (sx / windowWidth) * 2.0f - 1.0f;
        float ndcY = 1.0f - (sy / windowHeight) * 2.0f;
        return glm::vec3(ndcX, ndcY, 0.0f);
        };

    // 左上角和右下角坐标
    glm::vec3 topLeft = screenToNDC(x, y);
    glm::vec3 bottomRight = screenToNDC(x + width, y + height);

    // 绘制位图
    bitmap.Draw(topLeft, bottomRight, angle);
}

void EasyGL::DrawVAO(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    if (!isInit) return;

    // 使用指定的着色器
    shader.use();

    // 绑定 VAO 和 IBO
    va.Bind();
    ib.Bind();

    // 绘制操作
    glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);

    // 解绑 VAO 和 IBO
    VertexArray::Unbind();
    IndexBuffer::Unbind();
}

void EasyGL::SetClearColor(const color& color)
{
    clearColor = color;
}