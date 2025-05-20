#pragma once

#include <map>
#include <memory>
#include <string>
#include <freetype/freetype.h>
#include <glm/glm.hpp>

#include "../render/Shader.h"
#include "../render/VertexArray.h"
#include "Base.h"

namespace core
{
class Bitmap;
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
    Font(const std::string& fontPath, bool needPreLoad = true);
    ~Font();    // 渲染文本
    void RenderText(const std::wstring& text, float x, float y, float scale, const glm::vec4& color);
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    // 居中渲染文本
    void RenderTextBetween(const std::wstring& text, Region region, float scale, const glm::vec4& color);
    void RenderTextBetween(const std::string& text, Region region, float scale, const glm::vec4& color);
    // 垂直渲染文本
    void RenderTextVertical(const std::wstring& text, float x, float y, float scale, const glm::vec4& color);
    void RenderTextVertical(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    // 垂直居中渲染文本
    void RenderTextVerticalBetween(const std::wstring& text, Region region, float scale, const glm::vec4& color);
    void RenderTextVerticalBetween(const std::string& text, Region region, float scale, const glm::vec4& color);

    // 将文字渲染到Bitmap对象上
    void RenderTextToBitmap(const std::wstring& text, std::shared_ptr<Bitmap> bitmap, float x, float y, float scale, const glm::vec4& color);
    void RenderTextToBitmap(const std::string& text, std::shared_ptr<Bitmap> bitmap, float x, float y, float scale, const glm::vec4& color);

    void RenderChar(wchar_t text, float x, float y, float scale, const glm::vec4& color);

    float GetFontSize() const;

    bool operator==(const Font&) const;
private:
    bool LoadCharacter(wchar_t c);
    Character GetCharacter(wchar_t c);


    float fontSize; // 字体大小
    std::map<wchar_t, Character> Characters;
    VertexArray VAO;
    VertexBuffer VBO;
    static Shader shader;
    // 如果需要的话，添加其他成员变量
};
}