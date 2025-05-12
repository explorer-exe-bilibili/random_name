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

    struct Character {
        Texture texture;
        glm::vec2 size;
        glm::vec2 bearing;
        unsigned int advance;
        Character(Texture& tx, const glm::vec2& size, const glm::vec2& bearing, unsigned int advance)
            : texture(tx), size(size), bearing(bearing), advance(advance) {}
        Character(const Character& character)
            : texture(character.texture), size(character.size),
              bearing(character.bearing), advance(character.advance) {}
        Character() : texture(), size(0, 0), bearing(0, 0), advance(0) {}
        Character& operator=(const Character& character);
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