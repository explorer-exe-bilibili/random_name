#pragma once
#include <glm/glm.hpp>
#include "../render/Texture.h"
#include <string>
#include <GLFW/glfw3.h>

namespace core
{
    struct ScreenInfo
    {
        int width;
        int height;
        float aspectRatio;
        float dpi;
        float pixelRatio;
        GLFWwindow* window;
        ScreenInfo() : width(0), height(0), aspectRatio(0), dpi(0), pixelRatio(1), window(nullptr) {}
        ScreenInfo(int w, int h, float ar, float d, float pr, GLFWwindow* win)
            : width(w), height(h), aspectRatio(ar), dpi(d), pixelRatio(pr), window(win) {}
        ScreenInfo(const ScreenInfo& info)
            : width(info.width), height(info.height), aspectRatio(info.aspectRatio),
              dpi(info.dpi), pixelRatio(info.pixelRatio), window(info.window) {}
    } extern screenInfo;

    struct Color{
        char r,g,b,a;
        Color() : r(0), g(0), b(0), a(0) {}
        Color(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) {}
        Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
        Color(const glm::vec3& color) : r(color.r * 255), g(color.g * 255), b(color.b * 255), a(255) {}
        Color(const glm::vec4& color) : r(color.r * 255), g(color.g * 255), b(color.b * 255), a(color.a * 255) {}
        
        operator glm::vec4() const { return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
        operator glm::vec3() const { return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f); }
    };
    namespace color{extern Color white, black, red, green, blue;}
    struct Region {
        float x,xend;
        float y,yend;
        Region() : x(0), y(0), xend(0), yend(0) {}
        Region(float x, float y, float xend, float yend) : x(x), y(y), xend(xend), yend(yend) {}
        Region(const Region& region) : x(region.x), y(region.y), xend(region.xend), yend(region.yend) {}
        Region(const glm::vec4& region) : x(region.x), y(region.y), xend(region.z), yend(region.w) {}
        operator glm::vec4() const { return glm::vec4(x, y, xend, yend); }
    };

    struct Point {
        float x,y;
        Point() : x(0), y(0) {}
        Point(float x, float y) : x(x), y(y) {}
        Point(const Point& point) : x(point.x), y(point.y) {}
        Point(const glm::vec2& point) : x(point.x), y(point.y) {}
        operator glm::vec2() const { return glm::vec2(x, y); }
    };

    struct Size
    {
        int width;
        int height;
        Size() : width(0), height(0) {}
        Size(int w, int h) : width(w), height(h) {}
        Size(const Size& size) : width(size.width), height(size.height) {}
        Size(const glm::vec2& size) : width(size.x), height(size.y) {}
        operator glm::vec2() const { return glm::vec2(width, height); }
    };
    std::string wstring2string(const std::wstring& wstr);
    std::wstring string2wstring(const std::string& str);
}