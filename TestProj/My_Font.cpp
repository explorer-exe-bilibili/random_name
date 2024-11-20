#include "My_Font.h"


#include <fstream>
#include <sstream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include<wchar.h>
#include <glm/ext/matrix_clip_space.hpp>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


LoadText::LoadText(std::string rootPath) :rootPath(rootPath)
{
	this->loadShader();
}

// �ı� ����
void LoadText::loadText(const wchar_t* text, GLfloat x, GLfloat y)
{
	std::string fontPath = "C:/Windows/Fonts/arial.ttf";

	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	this->shader->use();
	this->shader->setMat4("projection", projection);
	//������ɫ
	this->shader->setVec3("textColor", glm::vec3(0.0f, 1.0f, 0.0f));

	//��ʼ��FreeType��
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "FreeType��ʼ��ʧ��" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
		std::cout << "�������ʧ��" << std::endl;

	//���������С
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //�����ֽڶ�������
	int lew_w = wcslen(text);

	for (GLubyte i = 0; i < lew_w; i++)
	{
		// �����ַ������� 
		if (FT_Load_Char(face, text[i], FT_LOAD_RENDER))
		{
			std::cout << "���μ���ʧ��" << std::endl;
			continue;
		}
		// ���δ�С
		int font_width = face->glyph->bitmap.width;
		int font_rows = face->glyph->bitmap.rows;
		// �ӻ�׼�ߵ�������/������ƫ��ֵ
		int font_left = face->glyph->bitmap_left;
		int font_top = face->glyph->bitmap_top;
		// ԭ�����һ������ԭ��ľ���
		int font_x = face->glyph->advance.x;
		//
		unsigned char* data = face->glyph->bitmap.buffer;
		// ��������
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_width, font_rows, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		// ��������ѡ��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLfloat xpos = x + font_left;
		GLfloat ypos = y - (font_rows - font_top);

		GLfloat w = font_width;
		GLfloat h = font_rows;
		// ��ÿ���ַ�����VBO
		GLfloat vertices[6][4] = {
			{ xpos,			     ypos + font_rows,   0.0, 0.0 },
			{ xpos,				 ypos,				 0.0, 1.0 },
			{ xpos + font_width, ypos,				 1.0, 1.0 },

			{ xpos,				 ypos + font_rows,   0.0, 0.0 },
			{ xpos + font_width, ypos,				 1.0, 1.0 },
			{ xpos + font_width, ypos + font_rows,   1.0, 0.0 }
		};

		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 4, &vertices[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		word fw1;
		fw1.texture = texture;
		fw1.VAO = VAO;
		this->textVecs.push_back(fw1);

		x += (font_x >> 6);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void LoadText::drawText()
{
	for (int i = 0; i < this->textVecs.size(); i++)
	{
		word wf = this->textVecs[i];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wf.texture);

		glBindVertexArray(wf.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
}

void LoadText::loadShader()
{
	this->shader = new Shader("shaders/text.vs", "shaders/text.fs");
}
