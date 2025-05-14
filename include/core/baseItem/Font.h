#pragma once

#include "../render/Texture.h"
#include "base.h"

#include <string>
#include <freetype/freetype.h>
#include <map>
#include <memory>


namespace core
{

class Font{
    static bool inited;
public:
    Font(const std::string& fontPath, bool needPreLoad=true);
    Font(const Font& font)=delete;
    ~Font();

    void RenderText(const std::string& text, core::Point position, float scale, const glm::vec3& color);
    void RenderText(const std::wstring& text, core::Point position, float scale, const glm::vec3& color);
    void RenderChar(wchar_t c, core::Point position, float scale, const glm::vec3& color);

    float GetTextWidth(const std::wstring& text, float scale);
    float GetTextHeight(const std::wstring& text, float scale);
    Character& GetCharacter(wchar_t c);
    float GetFontSize() const;
    unsigned int GetFontID() const{return fontID;}

    bool setCustomerShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
    bool setCustomerShaderProgram(const Shader& shader);

    Font& operator=(const Font& font)=delete;
    bool operator==(const Font&) const;
private:
    bool LoadCharacter(wchar_t c);
    
    static FT_Library ft;
    FT_Face face;
    static std::shared_ptr<Font> spare_font;
    float fontSize; // 字体大小
    std::map<wchar_t, Character> Characters;
    VertexArray VAO;
    VertexBuffer VBO;
    static Shader shader;
    unsigned int fontID;
};
}