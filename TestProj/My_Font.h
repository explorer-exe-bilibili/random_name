#pragma once
#include<windows.h>
#include <GL/gl.h>
#include <ft2build.h>
#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include FT_FREETYPE_H

class My_Font
{
public:
	// �ṹ�屣��ÿ���ַ�������������Ϣ
	struct Character {
		GLuint TextureID;   // �ַ�����ID
		unsigned int SizeX;          // �ַ����
		unsigned int SizeY;          // �ַ��߶�
		int BearingX;       // �ַ����Ļ���ƫ��
		int BearingY;       // �ַ������Ļ���ƫ��
		long Advance;       // �ַ���ˮƽǰ����
	};
	std::vector<wchar_t> triedChars;
private:
	FT_Library library;
	FT_Face face;
	std::map<wchar_t, Character> Characters;
	static My_Font* defaultFont;
public:
	My_Font();
	~My_Font();
	void init();
	void resetSize();
	void load(std::string fontPath, int fontSize);
	Character getCharacter(wchar_t wChar);
	static Character getDefaultCharacter(wchar_t wChar);
	static void InitDefaultFont();

};


