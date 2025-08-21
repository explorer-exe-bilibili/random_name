#pragma once

#include <map>
#include <memory>
#include <string>
#include <freetype/freetype.h>
#include <glm/glm.hpp>

#include "../render/IFontRenderer.h"
#include "Base.h"

namespace core
{

struct Character {
    unsigned int     TextureID;  // 字符纹理ID
    glm::ivec2 Size;             // 字符大小
    glm::ivec2 Bearing;          // 基线到字符左部/顶部的偏移值
    unsigned int     Advance;    // 原点距下一个字形原点的水平偏移量
};

class Font {
    static FT_Library ft;
    FT_Face face;
    static std::shared_ptr<Font> spare_font;
public:
    // 注入渲染器（程序启动时设置一次）
    static void SetFontRenderer(IFontRenderer* r) { fontRenderer = r; }
    static IFontRenderer* GetFontRenderer() { return fontRenderer; }

    Font(const std::string& fontPath, bool needPreLoad = true, unsigned int fontSize = 48.0f);
    ~Font(); 
    // 渲染文本
    void RenderText(const std::wstring& text, float x, float y, float scale, const glm::vec4& color);
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    // 居中渲染文本
    void RenderTextBetween(const std::wstring& text, Region region, float scale, const glm::vec4& color);
    void RenderTextBetween(const std::string& text, Region region, float scale, const glm::vec4& color);
    void RenderTextBetween(const std::wstring& text, SubRegion region, float scale, const glm::vec4& color);
    void RenderTextBetween(const std::string& text, SubRegion region, float scale, const glm::vec4& color);

    // 居中渲染文本（不截断）
    void RenderTextCentered(const std::wstring& text, Region region, float scale, const glm::vec4& color);
    void RenderTextCentered(const std::string& text, Region region, float scale, const glm::vec4& color);
    void RenderTextCentered(const std::wstring& text, SubRegion region, float scale, const glm::vec4& color);
    void RenderTextCentered(const std::string& text, SubRegion region, float scale, const glm::vec4& color);
    
    // 垂直渲染文本
    void RenderTextVertical(const std::wstring& text, float x, float y, float scale, const glm::vec4& color);
    void RenderTextVertical(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    // 垂直居中渲染文本
    void RenderTextVerticalBetween(const std::wstring& text, SubRegion region, float scale, const glm::vec4& color);
    void RenderTextVerticalBetween(const std::string& text, SubRegion region, float scale, const glm::vec4& color);
    void RenderTextVerticalBetween(const std::wstring& text, Region region, float scale, const glm::vec4& color);
    void RenderTextVerticalBetween(const std::string& text, Region region, float scale, const glm::vec4& color);

    void RenderChar(wchar_t text, float x, float y, float scale, const glm::vec4& color);
    
    // 渲染字符，自动计算缩放以铺满指定区域
    void RenderCharFitRegion(wchar_t text, Region region, const glm::vec4& color);
    void RenderCharFitRegion(wchar_t text, SubRegion region, const glm::vec4& color);

    // 渲染字符串，自动计算缩放以铺满指定区域（横向填充）
    void RenderStringFitRegion(const std::wstring& text, Region region, const glm::vec4& color);
    void RenderStringFitRegion(const std::string& text, Region region, const glm::vec4& color);
    void RenderStringFitRegion(const std::wstring& text, SubRegion region, const glm::vec4& color);
    void RenderStringFitRegion(const std::string& text, SubRegion region, const glm::vec4& color);

    unsigned int GetFontSize() const;

    bool operator==(const Font&) const;
    bool isLoaded() const{return isOK;};
private:
    bool LoadCharacter(wchar_t c);
    Character GetCharacter(wchar_t c);
    float CalculateDynamicScale(float baseScale) const;
    float DeCalculateDynamicScale(float baseScale) const;

    bool isOK=false;
    unsigned int fontSize; // 字体大小
    std::map<wchar_t, Character> Characters;
    // 渲染器指针（由程序注入）
    static IFontRenderer* fontRenderer;
    // 如果需要的话，添加其他成员变量
};
}