#include "core/Drawer.h"
#include "core/render/GLBase.h"
#include <cmath>
#include <vector>

using namespace core;

Drawer::Drawer(GLFWwindow* window) : m_window(window) {
    
    // 启用Alpha混合（正确设置处理半透明）
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glBlendEquation(GL_FUNC_ADD));
    
    // 初始化默认着色器
    InitDefaultShader();
}

Drawer::~Drawer() {
    // 清理资源
}

void Drawer::InitDefaultShader() {
    // 简单的着色器，用于绘制基本图元
    static const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;

    void main()
    {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
    )";

    static const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 u_Color;

    void main()
    {
        FragColor = u_Color;
    }
    )";

    defaultShader.init(vertexShaderSource, fragmentShaderSource);
}

glm::vec2 Drawer::ScreenToNDC(float x, float y) const {
    // 获取当前窗口尺寸（以确保使用最新的尺寸）
    int width = WindowInfo.width;
    int height = WindowInfo.height;
    
    // 转换坐标
    float ndcX = (x / width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (y / height) * 2.0f;
    return {ndcX, ndcY};
}

void Drawer::DrawLine(Point start, Point end, Color color, bool dashed) {
    glm::vec2 p1 = ScreenToNDC(start.x, start.y);
    glm::vec2 p2 = ScreenToNDC(end.x, end.y);

    float vertices[] = {
        p1.x, p1.y,
        p2.x, p2.y
    };

    // 创建VAO和VBO
    VertexArray va;
    VertexBuffer vb(vertices, sizeof(vertices));

    va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

    // 使用着色器并设置颜色（包括Alpha通道）
    defaultShader.use();
    glm::vec4 colorVec(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    defaultShader.setVec4("u_Color", colorVec);

    // 如果是虚线
    if (dashed) {
        GLCall(glEnable(GL_LINE_STIPPLE));
        GLCall(glLineStipple(1, 0x00FF)); // 设置虚线模式
    }

    // 绘制
    va.Bind();
    GLCall(glDrawArrays(GL_LINES, 0, 2));
    VertexArray::Unbind();

    // 如果启用了虚线，禁用它
    if (dashed) {
        GLCall(glDisable(GL_LINE_STIPPLE));
    }
}

void Drawer::DrawSquare(Region region, Color color, bool filled) {
    glm::vec2 p1 = ScreenToNDC(region.x, region.y);
    glm::vec2 p2 = ScreenToNDC(region.xend, region.yend);

    float vertices[] = {
        p1.x, p1.y,  // 左下角
        p2.x, p1.y,  // 右下角
        p2.x, p2.y,  // 右上角
        p1.x, p2.y   // 左上角
    };

    // 创建VAO和VBO
    VertexArray va;
    VertexBuffer vb(vertices, sizeof(vertices));
    va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

    // 使用着色器并设置颜色（包括Alpha通道）
    defaultShader.use();
    glm::vec4 colorVec(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    defaultShader.setVec4("u_Color", colorVec);

    // 绘制
    va.Bind();
    if (filled) {
        // 使用索引绘制填充矩形
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
        IndexBuffer ib(indices, 6);
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    } else {
        // 绘制未填充矩形
        GLCall(glDrawArrays(GL_LINE_LOOP, 0, 4));
    }
    VertexArray::Unbind();
}

void Drawer::DrawCircle(Point center, float radius, Color color, bool filled) {
    const int segments = 36; // 分段数，可以根据需要调整
    std::vector<float> vertices;

    glm::vec2 centerNDC = ScreenToNDC(center.x, center.y);
    
    // 使用WindowInfo的值来保持一致性
    float radiusX = radius / (WindowInfo.width / 2.0f);
    float radiusY = radius / (WindowInfo.height / 2.0f);

    if (filled) {
        // 如果是填充模式，需要先添加中心点
        vertices.push_back(centerNDC.x);
        vertices.push_back(centerNDC.y);
    }

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.1415926f * i / segments;
        float dx = cos(angle) * radiusX;
        float dy = sin(angle) * radiusY;
        vertices.push_back(centerNDC.x + dx);
        vertices.push_back(centerNDC.y + dy);
    }

    // 创建VAO和VBO
    VertexArray va;
    VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));
    va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

    // 使用着色器并设置颜色（包括Alpha通道）
    defaultShader.use();
    glm::vec4 colorVec(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    defaultShader.setVec4("u_Color", colorVec);

    // 绘制
    va.Bind();
    if (filled) {
        GLCall(glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2));
    } else {
        GLCall(glDrawArrays(GL_LINE_LOOP, 0, segments + 1));
    }
    VertexArray::Unbind();
}

void Drawer::DrawTriangle(Point p1, Point p2, Point p3, Color color, bool filled) {
    glm::vec2 v1 = ScreenToNDC(p1.x, p1.y);
    glm::vec2 v2 = ScreenToNDC(p2.x, p2.y);
    glm::vec2 v3 = ScreenToNDC(p3.x, p3.y);

    float vertices[] = {
        v1.x, v1.y,
        v2.x, v2.y,
        v3.x, v3.y
    };

    // 创建VAO和VBO
    VertexArray va;
    VertexBuffer vb(vertices, sizeof(vertices));
    va.AddBuffer(vb, 0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

    // 使用着色器并设置颜色（包括Alpha通道）
    defaultShader.use();
    glm::vec4 colorVec(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    defaultShader.setVec4("u_Color", colorVec);

    // 绘制
    va.Bind();
    if (filled) {
        GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));
    } else {
        GLCall(glDrawArrays(GL_LINE_LOOP, 0, 3));
    }
    VertexArray::Unbind();
}