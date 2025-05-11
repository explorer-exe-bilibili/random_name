#include "core/baseItem/Font.h"

#include "core/log.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace core;
FT_Library Font::ft;
bool Font::inited = false;
std::shared_ptr<Font> Font::defaultFont = nullptr;
Shader Font::shader;
int FontIdID = 1;


static std::string vertexShader = R"(
#version 330 core
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
}
)";

static std::string fragmentShader = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	float alpha = texture(text, TexCoords).r;
	color = vec4(textColor, alpha);
}
)";

Font::Font(const std::string& fontPath, bool needPreLoad) : fontSize(48), width(0), height(0)
{
    Log<<Level::Info<<"Font::Font() "<<fontPath<<op::endl;
    if (!inited)
    {
        Log<<Level::Info<<"Font::Font() First Init FreeType"<<op::endl;
        inited = true;
        FT_Init_FreeType(&ft);
        shader.init(vertexShader, fragmentShader);
        LoadDefaultFont();
        Log<<Level::Info<<"Font::Font() Init FreeType Success"<<op::endl;
    }
    Log<<Level::Info<<"Font::Font() Init Font face"<<op::endl;
    FT_New_Face(ft, fontPath.c_str(), 0, &face);
    if (needPreLoad)
    {
        Log<<Level::Info<<"Font::Font() Preload Font"<<op::endl;
        for (wchar_t c = 0; c < 128; c++)
            LoadCharacter(c);
        //加载常用中文
        for (wchar_t c = 0x4E00; c < 0x9FA5; c++)
            LoadCharacter(c);
    }
    else
    {
        Log<<Level::Info<<"Font::Font() Not Preload Font"<<op::endl;
        LoadCharacter('?');
    }
    if(this==defaultFont.get())
    {
        Log<<Level::Info<<"Font::Font() Set Default Font"<<op::endl;
        Fontid=0;
        return;
    }
    vao.Bind();
    vbo.Bind();
    vao.Unbind();
    vbo.Unbind();
    Fontid=FontIdID++;
    Log<<Level::Info<<"Font::Font() Init Font face Success"<<Fontid<<op::endl;
}

Font::Font(const Font& font) : fontSize(font.fontSize), width(font.width), height(font.height)
{
    Log<<Level::Info<<"Font::Font(const Font& font) from"<<font.getFontID()<<op::endl;
    face = font.face;
    characters = font.characters;
    vao = font.vao;
    vbo = font.vbo;
    CustomShaderProgram = font.CustomShaderProgram;
    Fontid=FontIdID++;
    Log<<Level::Info<<"Font::Font(const Font& font) to (finish)"<<Fontid<<op::endl;
}

Font::~Font()
{
    Log<<Level::Info<<"Font::~Font() "<<Fontid<<op::endl;
    FT_Done_Face(face);
    if(this==defaultFont.get())
    {
        inited=false;
        FT_Done_FreeType(ft);
    }
}

void Font::LoadDefaultFont()
{
    if (defaultFont == nullptr)
    {
        defaultFont = std::make_shared<Font>("files/fonts/spare.ttf", false);
        defaultFont->fontSize = 48;
        defaultFont->vao.Bind();
        defaultFont->vbo.Bind();
        defaultFont->vao.Unbind();
        defaultFont->vbo.Unbind();
    }
}

void Font::RenderText(const std::string& text, core::Point position, int scale, const glm::vec3& color)
{
    //检查是否包含有未加载的字符
    for (const auto& c : text)
    {
        if (characters.find(c) == characters.end())
        {
            Log<<Level::Warn<<"Font::RenderText() Character not loaded: "<<c<<op::endl;
            LoadCharacter(c);
        }
    }
    //设置投影矩阵
    glm::mat4 projection = glm::ortho(0.0f, (float)screenInfo.width, 0.0f, (float)screenInfo.height);
    Log<<Level::Info<<"Font::RenderText() "<<text<<op::endl;
    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec3("textColor", color);
    //启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //设置VAO
    vao.Bind();
    //遍历每个字符
    for(auto &c: text)
    {
        Character ch = GetCharacter(c);
        float xpos = width + ch.bearing.x * scale;
        float ypos = height - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        //设置顶点数据
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos,     ypos,       0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f }
        };
        //绑定纹理
        ch.texture.Bind();
        //更新VBO数据
        vbo.Bind();
        vbo.BufferSubData(0, sizeof(vertices), vertices);
        //解绑VBO
        vbo.Unbind();
        //绘制字符
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //更新宽度
        position.x += (ch.advance >> 6) * scale; // Bitshift by 6 to convert from 1/64 pixels to pixels
    }
    VertexArray::Unbind();
    Texture::unbind();
    glDisable(GL_BLEND);
    Log<<Level::Info<<"Font::RenderText() "<< text <<" (finish)"<<op::endl;
}