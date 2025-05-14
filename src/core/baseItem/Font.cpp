#include "core/baseItem/Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <mutex>
#include <thread>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "core/render/GLBase.h"

using namespace core;

FT_Library Font::ft;
static unsigned int fontCounter = 0;

std::string vertexShader = R"(
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

std::string fragmentShader = R"(
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

std::shared_ptr<Font> Font::spare_font = nullptr;
Shader Font::shader;

Font::Font(const std::string& fontPath,bool needPreLoad)
{
	std::cout << "loading Font file "<<fontPath<<std::endl;
	if (!shader)
	{
		shader.init(vertexShader, fragmentShader);
	}
	static bool spareIniting = false;
	// 初始化备用字体
	if (!spare_font&&!spareIniting) {
		std::thread([this]
			{
				spareIniting = true;
				spare_font = std::make_shared<Font>
			(R"(D:\Users\explorer\Source\Repos\random_name\random\files\ttfs\spare.ttf)",0);
			}).detach();
	}
	// 初始化 FreeType 字库
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "无法初始化 FreeType 库" << std::endl;
		return;
	}

	// 加载字体
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
		std::cerr << "无法加载字体: " << fontPath << std::endl;
		FT_Done_FreeType(ft);
		return;
	}
	fontSize = 48;
	// 设置字体大小
	FT_Set_Pixel_Sizes(face, 0, 48);

	// 加载字符
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐限制
	if (needPreLoad)
	{
		for (wchar_t c = 0; c < 128; c++) {
			LoadCharacter(c);
		}

		for (wchar_t c = 0x4e00; c < 0x9fa5; c++)
		{
			LoadCharacter(c);
		}
	}
	else
	{
		LoadCharacter(L'?');
	}
	// 设置 VAO 和 VBO
	VAO.Bind();
	VBO.Bind();
	VBO.BufferData(nullptr, sizeof(float) * 6 * 4); // 6 个顶点， 每个顶点 4 个 float

	// 配置顶点属性
	VAO.AddBuffer(VBO, 0, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)0);

	VertexArray::Unbind();
	VertexBuffer::Unbind();
	fontID=fontCounter++;
	std::cout << "Load font finished"<<fontPath<<std::endl;
}

Font::~Font() {
	// 释放纹理
	for (auto& pair : Characters) {
		glDeleteTextures(1, &pair.second.textureID);
	}

	// 释放 FreeType 资源
	FT_Done_Face(face);
}

void core::Font::RenderText(const std::string & text, core::Point position, float scale, const glm::vec3 & color)
{
	RenderText(core::string2wstring(text), position, scale, color);
}

void Font::RenderText(const std::wstring& text, Point position, float scale, const glm::vec3& color) {
	//检查是否包含未加载的字符
	for (const auto& c : text) {
		if (Characters.find(c) == Characters.end())
		{
			LoadCharacter(c);
		}
	}
	// 设置正交投影矩阵
	glm::mat4 projection = 
		glm::ortho(0.0f, static_cast<float>(screenInfo.width), 0.0f, static_cast<float>(screenInfo.height));
	std::cout << "paint test try"<<std::endl;
	// 设置字体着色器中的投影矩阵
	shader.use();
	shader.setMat4("projection", projection);
	shader.setVec3("textColor", color);
	// 启用混合以处理文本的透明度
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glActiveTexture(GL_TEXTURE0));
	VAO.Bind();
	// 遍历文本中的字符
	for (auto c = text.begin(); c != text.end(); ++c) {
		Character ch = Characters[*c];

		float xpos = position.x + ch.bearing.x * scale;
		float ypos = position.y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		// 更新 VBO
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		// 绑定纹理
		GLCall(glBindTexture(GL_TEXTURE_2D, ch.textureID));


		// 更新顶点缓冲
		VBO.Bind();
		VBO.BufferSubData(0, sizeof(vertices), vertices);
		VertexBuffer::Unbind();

		// 绘制
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

		// 移动到下一个字符位置
		position.x += (ch.advance >> 6) * scale; // 位移单位是1/64像素，所以位移6位
	}

	VertexArray::Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Font::RenderChar(wchar_t text, Point position, float scale, const glm::vec3& color) {
	//检查是否包含未加载的字符
	if (Characters.find(text) == Characters.end())
	{
		LoadCharacter(text);
	}
	// 设置正交投影矩阵
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenInfo.width), 0.0f, static_cast<float>(screenInfo.height));
	std::cout << "paint test try" << std::endl;
	// 设置字体着色器中的投影矩阵
	shader.use();
	shader.setMat4("projection", projection);
	shader.setVec3("textColor", color);
	// 启用混合以处理文本的透明度
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glActiveTexture(GL_TEXTURE0));
	VAO.Bind();
	// 遍历文本中的字符
	Character ch = Characters[text];

	float xpos = position.x + ch.bearing.x * scale;
	float ypos = position.y - (ch.size.y - ch.bearing.y) * scale;

	float w = ch.size.x * scale;
	float h = ch.size.y * scale;

	// 更新 VBO
	float vertices[6][4] = {
		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos,     ypos,       0.0f, 1.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },

		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },
		{ xpos + w, ypos + h,   1.0f, 0.0f }
	};

	// 绑定纹理
	GLCall(glBindTexture(GL_TEXTURE_2D, ch.textureID));

	// 更新顶点缓冲
	VBO.Bind();
	VBO.BufferSubData(0, sizeof(vertices), vertices);
	VertexBuffer::Unbind();

	// 绘制
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	VertexArray::Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

float Font::GetTextHeight(const std::wstring & text, float scale)
{
	// 计算文本高度
	float height = 0.0f;
	for (auto c = text.begin(); c != text.end(); ++c) {
		if (Characters.find(*c) == Characters.end())
		{
			LoadCharacter(*c);
		}
		height = std::max(height, Characters[*c].size.y * scale);
	}
	return height;
}

float Font::GetTextWidth(const std::wstring &text, float scale)
{
	// 计算文本宽度
	float width = 0.0f;
	for (auto c = text.begin(); c != text.end(); ++c) {
		if (Characters.find(*c) == Characters.end())
		{
			LoadCharacter(*c);
		}
		width += Characters[*c].advance * scale;
	}
	return width;
}

bool Font::operator==(const Font& b) const
{
	if (face == b.face)return true;
	return false;
}
std::mutex mx;
bool Font::LoadCharacter(wchar_t c)
{
	// 检查是否已经加载过
	if (Characters.find(c) != Characters.end()) return true;
	std::lock_guard<std::mutex> lock(mx);

	// 加载字符字形
	if (FT_Get_Char_Index(face, c) == 0)
	{
		std::wcout << L"字符不存在: " << c << std::endl;
		if (!spare_font) return false;
		if (FT_Get_Char_Index(spare_font->face, c) == 0) return false;
		Characters.insert(std::make_pair(c, spare_font->GetCharacter(c)));
		return true;
	}
	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
		std::wcerr << L"加载字符失败: " << c << std::endl;
		return false;
	}

	// 创建纹理
	unsigned int textureID;
	glGenTextures(1, &textureID);
	
	// 设置纹理参数
	glBindTexture(GL_TEXTURE_2D, textureID);
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
	glBindTexture(GL_TEXTURE_2D, 0);

	// 存储字符
	Character character = {
		textureID,
		glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
		static_cast<unsigned int>(face->glyph->advance.x)
	};
	Characters.insert(std::make_pair(c, character));
	std::wcout << c << L"\t";
	return true;
}
float Font::GetFontSize() const
{
	return fontSize;
}

Character& Font::GetCharacter(wchar_t c)
{
	if (Characters.find(c) == Characters.end())
	{
		if (!LoadCharacter(c))return Characters[L'?'];
	}
	return Characters[c];
}
