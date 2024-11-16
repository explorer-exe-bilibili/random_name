﻿#pragma once
#include<windows.h>
#include <GL/gl.h>
#include <ft2build.h>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include FT_FREETYPE_H

class Font
{
	// 结构体保存每个字符的纹理和相关信息
	struct Character {
		GLuint TextureID;   // 字符纹理ID
		unsigned int SizeX;          // 字符宽度
		unsigned int SizeY;          // 字符高度
		int BearingX;       // 字符左侧的基线偏移
		int BearingY;       // 字符顶部的基线偏移
		long Advance;       // 字符的水平前进量
	};

	FT_Library library;
	FT_Face face;
	std::map<wchar_t, Character> Characters;
public:
	Font();
	~Font();
	void init();
	void resetSize();
	void load(std::string fontPath, int fontSize);
};

