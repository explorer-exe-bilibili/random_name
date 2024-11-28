#pragma once
#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Shader.h"

struct Character {
    GLuint TextureID;   // 字符纹理ID
    glm::ivec2 Size;    // 字符尺寸
    glm::ivec2 Bearing; // 字符基线偏移
    GLuint Advance;     // 字符水平前进量
};

class TextRenderer
{
public:
    std::map<char, Character> Characters;
    Shader TextShader;
    GLuint VAO, VBO;

    TextRenderer(unsigned int width, unsigned int height);
    void Load(const std::string& font, unsigned int fontSize);
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
};