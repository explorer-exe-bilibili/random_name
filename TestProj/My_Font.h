﻿#pragma once

#include <map>
#include <memory>
#include <string>
#include <freetype/freetype.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "VertexBuffer.h"

struct Character {
    Texture texture;         // 字符纹理
    glm::ivec2 Size;         // 字符大小
    glm::ivec2 Bearing;      // 基线到字符左部/顶部的偏移值
    unsigned int Advance;    // 原点距下一个字形原点的水平偏移量
};

class Font {
    FT_Library ft;
    FT_Face face;
    static std::shared_ptr<Font> spare_font;
public:
    Font(const std::string& fontPath, bool needPreLoad = true);
    ~Font();

    void RenderText(const std::wstring& text, float x, float y, float scale, const glm::vec3& color);

    void RendChar(wchar_t text, float x, float y, float scale, const glm::vec3& color);

    float GetFontSize() const;

    void resize(int WindowWidth, int WindowHeight);

    bool operator==(const Font&) const;
private:
    bool LoadCharacter(wchar_t c);
    Character GetCharacter(wchar_t c);


    float fontSize; // 字体大小
    std::map<wchar_t, Character> Characters;
    VertexArray VAO;
    VertexBuffer VBO;
    static Shader shader;
	int width, height;
    // 如果需要的话，添加其他成员变量
};