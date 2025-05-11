#pragma once

#include "../render/Texture.h"
#include "base.h"

#include <string>
#include <freetype/freetype.h>
#include <map>
#include <memory>
#pragma execution_character_set("utf-8")

namespace core
{

class Font{
    static FT_Library ft;
    static bool inited;
    static std::shared_ptr<Font> defaultFont;
    static Shader shader;
public:
    Font(const std::string& fontPath, bool needPreLoad=true);
    Font(const Font& font);
    ~Font();

    void RenderText(const std::string& text, core::Point position, int scale, const glm::vec3& color);
    void RenderText(const std::wstring& text, core::Point position, int scale, const glm::vec3& color);
    void RenderChar(wchar_t c, core::Point position, int scale, const glm::vec3& color);

    float GetTextWidth(const std::wstring& text, int scale);
    float GetTextHeight(const std::wstring& text, int scale);
    Character& GetCharacter(wchar_t c);
    int getFontID() const { return Fontid; }

    bool setCustomerShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
    bool setCustomerShaderProgram(const Shader& shader);

    Font& operator=(const Font& font);
private:
    bool LoadCharacter(wchar_t c);

    void LoadDefaultFont();
    
    int Fontid;
    FT_Face face;
    float fontSize;
    std::map<wchar_t, core::Character> characters;
    VertexArray vao;
    VertexBuffer vbo;
    std::shared_ptr<Shader> CustomShaderProgram;
};
}