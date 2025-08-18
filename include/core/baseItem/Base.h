#pragma once
#include <glm/glm.hpp>
#include "../render/Texture.h"
#include <string>
#include <GLFW/glfw3.h>
#include <filesystem>

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
        bool aspectRatio1to1=false; // 标识是否使用1:1比例
    public:
        Region() : x(0), y(0), xend(0), yend(0) {}
        Region(double x, double y, double xend, double yend, bool Ratio=true, bool AspectRatio1to1=false) :
         x(x), y(y), xend(xend), yend(yend), screenRatio(Ratio), aspectRatio1to1(AspectRatio1to1) {
            // 检测是否是1:1比例模式（通过yend <= 0判断）
            if (yend <= 0) {
                aspectRatio1to1 = true;
                this->yend = 0; // 标准化存储
            }
        }
        Region(const Region& region) : x(region.x), y(region.y), xend(region.xend), yend(region.yend), screenRatio(region.screenRatio), aspectRatio1to1(region.aspectRatio1to1) {}
        Region(const glm::vec4& region) : x(region.x), y(region.y), xend(region.z), yend(region.w), screenRatio(true), aspectRatio1to1(false) {}
        
        float getx() const { return screenRatio ? x * WindowInfo.width : x; }
        float gety() const { return screenRatio ? y * WindowInfo.height : y; }
        float getxend() const { return screenRatio ? xend * WindowInfo.width : xend; }
        float getyend() const;
        float getWidth() const {return getxend()-getx();}
        float getHeight() const {return getyend()-gety();}
        float getRatio() const { return getWidth() / getHeight(); }
        bool isScreenRatio() const { return screenRatio; }
        bool isAspectRatio1to1() const { return aspectRatio1to1; }
        
        void setx(float x) { this->x = x; }
        void sety(float y) { this->y = y; }
        void setxend(float xend) { this->xend = xend; }
        void setyend(float yend) { 
            this->yend = yend; 
            // 如果设置了非零的yend，则退出1:1比例模式
            if (yend > 0) {
                aspectRatio1to1 = false;
            }
        }
        void setRatio(bool Ratio) { this->screenRatio = Ratio; }
        void setAspectRatio1to1(bool enable) { 
            aspectRatio1to1 = enable; 
            if (enable) {
                yend = 0; // 1:1模式下yend存储为0
            }
        }

        float getOriginX() const { return x; }
        float getOriginY() const { return y; }
        float getOriginXEnd() const { return xend; }
        float getOriginYEnd() const { 
            if (aspectRatio1to1) {
                // 对于1:1比例，返回计算后的实际yend值（用于编辑器显示）
                return y + (xend - x); // 保持原始坐标系的1:1比例
            }
            return yend; 
        }
        float getOriginW() const {return xend-x;}
        float getOriginH() const {
            if (aspectRatio1to1) {
                return xend - x; // 1:1比例时高度等于宽度
            }
            return yend-y;
        }

        float getx_() const { return screenRatio ? x * WindowInfo.width : x; }
        float gety_() const { return screenRatio ? y * WindowInfo.height : y; }
        float getxend_() const { return screenRatio ? xend * WindowInfo.width : xend; }
        float getyend_() const { return screenRatio ? yend * WindowInfo.height : yend; }

        operator glm::vec4() const { return glm::vec4(getx(), gety(), getxend(), getyend()); }
    };
    class SubRegion:public core::Region{
        core::Region fatherRegion;
    public:
        float x,xend,y,yend;
        
        // 构造函数
        SubRegion() : core::Region(), x(0), y(0), xend(0), yend(0) {}
        SubRegion(double x, double y, double xend, double yend) 
            : core::Region(x, y, xend, yend), x(x), y(y), xend(xend), yend(yend) {}
        
        void setFatherRegion(core::Region& father){fatherRegion=father;}
        float getx() const ;
        float gety() const ;
        float getxend() const;
        float getyend() const ;
    };
    
    // Region预设名称
    enum class RegionName
    {
        SMALL_WINDOW,
        FULLSCREEN,
        SIMULATE_MOBILE,
        NONE
    };
    inline std::string to_string(RegionName name) {
        switch (name) {
            case RegionName::FULLSCREEN: return "fullscreen";
            case RegionName::SMALL_WINDOW: return "small_window";
            case RegionName::SIMULATE_MOBILE: return "simulate_mobile";
            case RegionName::NONE: return "none";
            default: return "unknown";
        }
    }
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
        bool isScreenRatio() const { return screenRatio; }
        
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
        bool isScreenRatio() const { return screenRatio; }
        
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
    bool stringContains(const std::string& str, const std::string& substr);
    void quit();
    void restart();
    void msgBox(const std::string& title, const std::string& message);

    // 检查路径是否包含非ASCII字符
    bool hasNonASCIICharacters(const std::string& path);
    void checkProgramPathAndWarn();

    // OpenGL版本检测相关函数
    bool checkOpenGLVersion(int requiredMajor, int requiredMinor);
    void showOpenGLVersionError(const std::string& currentVersion, const std::string& requiredVersion);
    bool validateOpenGLSupport();

    // 初始化错误处理函数
    void showGLFWInitError();
    void showGLADInitError();
    void showWindowCreationError();
    void showConfigInitError();
    void showExplorerInitError();

    // 全局变量，用于跟踪程序目录状态
    extern std::filesystem::path g_executableDir;
    extern std::filesystem::path g_userDataDir;
    extern bool g_programDirWritable;
    
    // 初始化目录状态
    void initializeDirectories();
}