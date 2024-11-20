#include<GL/glew.h>
#include "Gp.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <windows.h>
#include <chrono>
#include <iostream>
#include <map>
#include <freetype/freetype.h>
#include <GL/glext.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "My_Font.h"
#include "sth2sth.h"

// 链接OpenGL库
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

bool InitializeFreeType(const char* fontPath, GLuint fontSize);
struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

std::map<wchar_t, Character> Characters;
namespace mywindows {
	extern HWND main_hwnd;
}

Gp::Gp(HWND hwnd){
    if (!initOpenGL(hwnd)) {
    }

    // 初始化 GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return;
    }

    // 初始化 FreeType 和文本渲染
    if (!InitializeFreeType("C:/Windows/Fonts/arial.ttf", 48)) { // 替换为您的字体路径
        return;
    }

    InitializeTextRendering();

    // 加载着色器
    textShader = new Shader("shaders/text.vs", "shaders/text.fs");
    if (textShader->ID != 0) {
        // 设置纹理单元
        textShader->use();
        textShader->setInt("text", 0);
        std::cout << "着色器加载成功。" << std::endl;
    }
    else {
        std::cerr << "着色器加载失败。" << std::endl;
        delete textShader;
        textShader = nullptr;
    }
}

Gp::~Gp() {
    if (hRC) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hRC);
    }
    // 删除 OpenGL 对象
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    for (auto it = Characters.begin(); it != Characters.end(); ++it) {
        glDeleteTextures(1, &it->second.TextureID);
    }
}
LoadText* lt = nullptr;

void Gp::RenderOpenGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 在这里添加OpenGL绘图代码
// 例如，绘制一个三角形
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex3f(0.0f, 1.0f, -5.0f);
    glColor3f(0, 1, 0);
    glVertex3f(-1.0f, -1.0f, -5.0f);
    glColor3f(0, 0, 1);
    glVertex3f(1.0f, -1.0f, -5.0f);
    glEnd();
    lt->drawText();
    SwapBuffers(hDC);
}
bool Gp::initOpenGL(HWND hWnd)
{

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // 支持双缓冲
        PFD_TYPE_RGBA,
        32, // 颜色位数
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24, // 深度缓冲区
        8,  // 模板缓冲区
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    hDC = GetDC(hWnd);
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (pixelFormat == 0) return false;

    if (!SetPixelFormat(hDC, pixelFormat, &pfd)) return false;

    hRC = wglCreateContext(hDC);
    if (hRC == NULL) return false;

    if (!wglMakeCurrent(hDC, hRC)) return false;

    // 初始化OpenGL设置
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 设置视口
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    glViewport(0, 0, width, height);

    // 设置投影矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);

    // 设置模型视图矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 初始化字体
    initFont();

    if (glewInit() != GLEW_OK) {
        return false;
    }

    lt = new LoadText("");
    lt->loadText(L"你", 0.1f, 0.1f);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Initialize shaderProgram
    const char* vertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec4 vertex; // <pos.x, pos.y, texCoord.x, texCoord.y>

	out vec2 TexCoords;

	uniform mat4 projection;

	void main()
	{
	    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	    TexCoords = vertex.zw;
	}
    )";

    const char* fragmentShaderSource = R"(
	#version 330 core
	in vec2 TexCoords;
	out vec4 FragColor;

	uniform sampler2D text;
	uniform vec3 textColor;

	void main()
	{    
	    float alpha = texture(text, TexCoords).r;
	    FragColor = vec4(textColor, alpha);
	}
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 初始化FPS计时
    frameCount = 0;
    fps = 0.0;
    lastTime = std::chrono::steady_clock::now();

    return true;
}
void Gp::RenderText(std::wstring text, float x, float y, float scale, glm::vec3 color) {
    // 启用着色器并设置颜色
    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Gp::initFont()
{
    //HFONT font = CreateFontW(
    //    16,                        // 高度
    //    0,                         // 宽度
    //    0,                         // 款角
    //    0,                         // 文字方向
    //    FW_NORMAL,                 // 字重
    //    FALSE,                     // 斜体
    //    FALSE,                     // 下划线
    //    FALSE,                     // 删除线
    //    ANSI_CHARSET,              // 字符集
    //    OUT_TT_PRECIS,             // 输出精度
    //    CLIP_DEFAULT_PRECIS,       // 裁剪精度
    //    ANTIALIASED_QUALITY,       // 输出质量
    //    FF_DONTCARE | DEFAULT_PITCH, // 家族和间距
    //    L"微软雅黑");                  // 字体名称

    //SelectObject(hDC, font);
    //base = glGenLists(5000);
    //wglUseFontBitmaps(hDC, 0, 5000, base);
    //DeleteObject(font);
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        // 处理错误
    }

    FT_Face face;
    if (FT_New_Face(ft, R"(c:\windows\fonts\arial.ttf)", 0, &face)) {
        // 处理错误
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (wchar_t c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<wchar_t, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
bool InitializeFreeType(const char* fontPath, GLuint fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath, 0, &face)) {
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字形加载时的字形加载遮蔽

    for (GLubyte c = 0; c < 128; c++) {
        // 加载字符的字形
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }

        // 生成纹理
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // 设置纹理选项
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 存储字符信息
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}
void Gp::renderBitmapString(float x, float y, void* font , const std::string text)
{

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(VAO);

    // 遍历每个字符
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x;
        float ypos = y - (ch.Size.y - ch.Bearing.y);

        float w = ch.Size.x;
        float h = ch.Size.y;
        // 更新 VBO 容量
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // 渲染 glyph纹理图
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // 更新内容
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 进位到下一个字符
        x += (ch.Advance >> 6); // 位移值是 1/64 的像素
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Gp::CleanupOpenGL()
{
    if (hRC) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hRC);
    }
    if (hDC) {
        ReleaseDC(NULL, hDC);
    }

    if (base != 0) {
        glDeleteLists(base, 256);
    }
}

void Gp::InitializeTextRendering() {
    // 创建 VAO 和 VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Gp::renderBitmapString(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint screenWidth, GLuint screenHeight) {
    // 激活对应的着色器
    shader.use();
    shader.setVec3("textColor", color);

    // 设置投影矩阵
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
    shader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // 遍历每个字符
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // 更新 VBO 容量
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // 渲染 glyph 纹理图
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // 更新内容
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 进位到下一个字符
        x += (ch.Advance >> 6) * scale; // 位移值是 1/64 的像素
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}