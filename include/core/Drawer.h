#pragma once

#include "core/baseItem/Base.h"
#include <glfw/glfw3.h>
#include "core/render/Shader.h"
#include "core/render/VertexArray.h"
#include "core/render/VertexBuffer.h"
#include "core/render/IndexBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace core{
class Drawer{
public:
    Drawer(GLFWwindow* window);
    ~Drawer();

    static Drawer* getInstance() {
        static Drawer instance(WindowInfo.window);
        return &instance;
    }

    //绘制直线
    void DrawLine(Point start, Point end, Color color, bool dashed=0);
    //绘制方形
    void DrawSquare(Region region, Color color, bool filled=0);
    //绘制圆形
    void DrawCircle(Point center, float radius, Color color, bool filled=0);
    //绘制三角形
    void DrawTriangle(Point p1, Point p2, Point p3, Color color, bool filled=0);
    
    // 将屏幕坐标转换为NDC坐标
    glm::vec2 ScreenToNDC(float x, float y) const;

private:
    GLFWwindow* m_window;
    Shader defaultShader;

    // 初始化默认着色器
    void InitDefaultShader();
};

}