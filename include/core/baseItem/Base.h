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
    } extern WindowInfo, screenInfo;

    struct Color{
        unsigned char r,g,b,a;
        Color() : r(0), g(0), b(0), a(255) {}
        Color(unsigned char r,unsigned char g,unsigned char b,unsigned char a) : r(r), g(g), b(b), a(a) {}
        Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
        Color(const glm::vec3& color) : r(color.r * 255), g(color.g * 255), b(color.b * 255), a(255) {}
        Color(const glm::vec4& color) : r(color.r * 255), g(color.g * 255), b(color.b * 255), a(color.a * 255) {}
        Color(const long value){
            r=(unsigned char)((value>>24)&0xFF);
            g=(unsigned char)((value>>16)&0xFF);
            b=(unsigned char)((value>>8)&0xFF);
            a=(unsigned char)(value&0xFF);
        }
        
        operator glm::vec4() const { return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
        operator glm::vec3() const { return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f); }
        operator unsigned int() const { return (unsigned int)(r << 24 | g << 16 | b << 8 | a); }
    };
    namespace color{extern Color white, black, red, green, blue;}

    class Region {
    protected:
        float x,xend;
        float y,yend;
        bool screenRatio=true;
    public:
        Region() : x(0), y(0), xend(0), yend(0) {}
        Region(double x, double y, double xend, double yend, bool Ratio=true) : x(x), y(y), xend(xend), yend(yend), screenRatio(Ratio) {}
        Region(const Region& region) : x(region.x), y(region.y), xend(region.xend), yend(region.yend), screenRatio(region.screenRatio) {}
        Region(const glm::vec4& region) : x(region.x), y(region.y), xend(region.z), yend(region.w), screenRatio(true) {}
        
        float getx() const { return screenRatio ? x * WindowInfo.width : x; }
        float gety() const { return screenRatio ? y * WindowInfo.height : y; }
        float getxend() const { return screenRatio ? xend * WindowInfo.width : xend; }
        float getyend() const;
        float getWidth() const {return getxend()-getx();}
        float getHeight() const {return getyend()-gety();}
        bool getRatio() const { return screenRatio; }
        
        void setx(float x) { this->x = x; }
        void sety(float y) { this->y = y; }
        void setxend(float xend) { this->xend = xend; }
        void setyend(float yend) { this->yend = yend; }
        void setRatio(bool Ratio) { this->screenRatio = Ratio; }

        float getOriginX() const { return x; }
        float getOriginY() const { return y; }
        float getOriginXEnd() const { return xend; }
        float getOriginYEnd() const { return yend; }

        float getx_() const { return screenRatio ? x * WindowInfo.width : x; }
        float gety_() const { return screenRatio ? y * WindowInfo.height : y; }
        float getxend_() const { return screenRatio ? xend * WindowInfo.width : xend; }
        float getyend_() const { return screenRatio ? yend * WindowInfo.height : yend; }

        operator glm::vec4() const { return glm::vec4(getx(), gety(), getxend(), getyend()); }
    };

    class Point {
        float x,y;
        bool screenRatio=true;
    public:
        Point() : x(0), y(0) {}
        Point(float x, float y, bool Ratio=true) : x(x), y(y), screenRatio(Ratio) {}
        Point(const Point& point) : x(point.x), y(point.y), screenRatio(point.screenRatio) {}
        Point(const glm::vec2& point) : x(point.x), y(point.y), screenRatio(true) {}
        
        float getx() const { return screenRatio ? x * WindowInfo.width : x; }
        float gety() const { return screenRatio ? y * WindowInfo.height : y; }
        bool getRatio() const { return screenRatio; }
        
        void setx(float x) { this->x = x; }
        void sety(float y) { this->y = y; }
        void setRatio(bool Ratio) { this->screenRatio = Ratio; }

        operator glm::vec2() const { if(!screenRatio)return glm::vec2(x, y);
             else return glm::vec2(x/WindowInfo.width, y/WindowInfo.height); }
    };

    class Size
    {
        float width;
        float height;
        bool screenRatio=true;
    public:
        Size() : width(0), height(0) {}
        Size(float w, float h, bool Ratio=true) : width(w), height(h), screenRatio(Ratio) {}
        Size(const Size& size) : width(size.width), height(size.height), screenRatio(size.screenRatio) {}
        Size(const glm::vec2& size) : width(size.x), height(size.y), screenRatio(true) {}
        
        float getw() const { return screenRatio ? width * WindowInfo.width : width; }
        float geth() const { return screenRatio ? height * WindowInfo.height : height; }
        bool getRatio() const { return screenRatio; }
        
        void setw(float w) { width = w; }
        void seth(float h) { height = h; }
        void setRatio(bool Ratio) { this->screenRatio = Ratio; }

        operator glm::vec2() const { if(!screenRatio)return glm::vec2(width, height);
             else return glm::vec2(width/WindowInfo.width, height/WindowInfo.height); }
    };

    std::string wstring2string(const std::wstring& wstr);
    std::wstring string2wstring(const std::string& str);
    size_t utf8_length(const std::string& str);
    void openFile(const std::string& path);
    bool isFileExists(const std::string& path);
    void startFileWithoutWindow(const std::string& path);
    void quit();
    void restart();
}