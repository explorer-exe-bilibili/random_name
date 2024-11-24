#pragma once

//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Shader.h"

struct word {
    unsigned int texture;
    unsigned int VAO;
};

class LoadText
{
public:
    LoadText(std::string rootPath);

    void loadText(const wchar_t* text, GLfloat x, GLfloat y);

    void drawText();

    ~LoadText()
    {
        delete this->shader;
    }
private:
    std::string rootPath;
    
    Shader* shader;
    std::vector< word> textVecs;

    void loadShader(); //¼ÓÔØshaderµÄ
};
