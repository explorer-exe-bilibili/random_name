#include "My_Font.h"

#include <GL/glext.h>


My_Font* My_Font::defaultFont = nullptr;

My_Font::My_Font()
{
	init();
}

My_Font::~My_Font()
{
	FT_Done_Face(face);
	FT_Done_FreeType(library);

}

void My_Font::init()
{
	FT_Init_FreeType(&library);
}

void My_Font::resetSize()
{
	FT_Set_Char_Size(face, 0, 16 * 64, 300, 300);
}

void My_Font::load(std::string fontPath, int fontSize)
{

	// �� wchar_t ת��Ϊ���ֽ��ַ���·��
	const std::string fontPathA(fontPath.begin(), fontPath.end());
	if (FT_New_Face(library, fontPathA.c_str(), 0, &face))return;

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // �����ֽڶ���
	// Ԥ���س��õ������ַ����������Ҫ��̬����
	for (wchar_t c = 0x4E00; c <= 0x9FA5; c++) { // ���ú��ַ�Χ
		// �����ַ�
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			triedChars.push_back(c);
			continue;
		}

		// ��������
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

		// ��������ѡ��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// �����ַ���Ϣ
		Character character = {
			texture,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x
		};
		Characters.insert(std::pair(c, character));
	}
}

My_Font::Character My_Font::getCharacter(wchar_t wChar)
{
	for (const auto a : triedChars)
		if (a == wChar)
			return getDefaultCharacter(wChar);
	const auto it = Characters.find(wChar);
	if (it == Characters.end())		// ����δ���ص��ַ�
		return getDefaultCharacter(wChar);
	return it->second;
}

My_Font::Character My_Font::getDefaultCharacter(wchar_t wChar)
{
	return defaultFont->getCharacter(wChar);
}

void My_Font::InitDefaultFont()
{
	defaultFont = new My_Font();
	defaultFont->load(R"(C:\Windows\Fonts\msyh.ttf)", 48);
}
