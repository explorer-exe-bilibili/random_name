﻿#include "FontManager.h"

#include <GL/glext.h>

void Font::init()
{
	FT_Init_FreeType(&library);
}

void Font::resetSize()
{
	FT_Set_Char_Size(face, 0, 16 * 64, 300, 300);
}

void Font::load(std::string fontPath,int fontSize)
{

	// 将 wchar_t 转换为多字节字符串路径
	std::string fontPathA(fontPath.begin(), fontPath.end());
	if (FT_New_Face(library, fontPathA.c_str(), 0, &face))return;

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐
	// 预加载常用的中文字符，或根据需要动态加载
	for (wchar_t c = 0x4E00; c <= 0x9FA5; c++) { // 常用汉字范围
		// 加载字符
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))continue;

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

		// 保存字符信息
		Character character = {
			texture,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x
		};
		Characters.insert(std::pair<wchar_t, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}
