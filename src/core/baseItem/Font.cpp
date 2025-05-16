#include "core/baseItem/Font.h"
#include "core/render/VertexArray.h"
#include FT_FREETYPE_H

#include <iostream>
#include <mutex>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "core/render/GLBase.h"
#include "core/baseItem/Base.h"
#include "core/log.h"

using namespace core;

FT_Library Font::ft = nullptr;

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

std::shared_ptr<Font> Font::spare_font = nullptr;
Shader Font::shader;

Font::Font(const std::string& fontPath,bool needPreLoad)
{
	std::cout << "loading Font file "<<fontPath<<std::endl;
	if (!shader)
	{
		shader.init(vertexShader, fragmentShader);
	}
	// 初始化 FreeType 字库
	if(ft==nullptr) {
		if (FT_Init_FreeType(&ft)) {
			std::cerr << "无法初始化 FreeType 库" << std::endl;
			return;
		}
	}
	static bool spareIniting = false;
	// 初始化备用字体
	if (!spare_font&&!spareIniting) {
		spareIniting = true;
		spare_font = std::make_shared<Font>
			(R"(C:\Users\j1387\source\repos\explorer-exe-bilibili\random_name\files\fonts\spare.ttf)", 0);
	}
	face = nullptr;
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
	std::cout << "Load font finished"<<fontPath<<std::endl;
}

Font::~Font() {
	// 释放纹理
	for (auto& pair : Characters) {
		glDeleteTextures(1, &pair.second.TextureID);
	}

	// 释放 FreeType 资源
	FT_Done_Face(face);
}

void Font::RenderText(const std::string& text,float x, float y,float scale, const glm::vec3& color){
	std::wstring wtext = string2wstring(text);
	RenderText(wtext, x, y, scale, color);
}

void Font::RenderText(const std::wstring& text, float x, float y, float scale, const glm::vec3& color) {
	if(this==nullptr){
		Log<<"Font is nullptr"<<op::endl;
		spare_font->RenderText(text, x, y, scale, color);
		return;
	}
	//检查是否包含未加载的字符
	for (auto c = text.begin(); c != text.end(); ++c) {
		if (!Characters.contains(*c))
		{
			LoadCharacter(*c);
		}
	}
	// 设置正交投影矩阵
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenInfo.width), 0.0f, static_cast<float>(screenInfo.height));
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

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

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
		GLCall(glBindTexture(GL_TEXTURE_2D, ch.TextureID));

		// 更新顶点缓冲
		VBO.Bind();
		VBO.BufferSubData(0, sizeof(vertices), vertices);
		VertexBuffer::Unbind();

		// 绘制
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

		// 移动到下一个字符位置
		x += (ch.Advance >> 6) * scale; // 位移单位是1/64像素，所以位移6位
	}

	VertexArray::Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Font::RendChar(wchar_t text, float x, float y, float scale, const glm::vec3& color) {
	if (this == nullptr) {
		Log << "Font is nullptr" << op::endl;
		spare_font->RendChar(text, x, y, scale, color);
		return;
	}
	//检查是否包含未加载的字符
	if (!Characters.contains(text))
	{
		LoadCharacter(text);
	}
	// 设置正交投影矩阵
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenInfo.width), 0.0f, static_cast<float>(screenInfo.height));
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

	float xpos = x + ch.Bearing.x * scale;
	float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

	float w = ch.Size.x * scale;
	float h = ch.Size.y * scale;

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
	GLCall(glBindTexture(GL_TEXTURE_2D, ch.TextureID));

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

bool Font::operator==(const Font& b) const
{
	if (face == b.face)return true;
	return false;
}
static std::mutex mx;
bool Font::LoadCharacter(wchar_t c)
{
	if (this == nullptr) {
		Log << "Font is nullptr" << op::endl;
		spare_font->LoadCharacter(c);
		return false;
	}
	//检查是否已经加载过
	if (Characters.contains(c))return true;
	mx.lock();
	// 加载字符字形
	if (FT_Get_Char_Index(face,c)==0)
	{
		mx.unlock();
		std::wcout << L"字符不存在: " << c << std::endl;
		if (!spare_font)return false;
		if (FT_Get_Char_Index(spare_font->face, c) == 0)return false;
		Characters.insert(std::pair(c, spare_font->GetCharacter(c)));
		return true;
	}
	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
		mx.unlock();
		std::wcerr << L"加载字符失败: " << c << std::endl;
		return false;
	}
	// 生成纹理
	unsigned int texture;
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

	// 存储字符
	Character character = {
		texture,
		glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
		static_cast<unsigned int>(face->glyph->advance.x)
	};
	Characters.insert(std::pair(c, character));
	std::wcout << c<<L"\t";
	mx.unlock();
	return true;
}

float Font::GetFontSize() const
{
	return fontSize;
}

Character Font::GetCharacter(wchar_t c)
{
	if (this == nullptr) {
		Log << "Font is nullptr" << op::endl;
		return spare_font->GetCharacter(c);
	}
	if (!Characters.contains(c))
	{
		if (!LoadCharacter(c))return Characters[L'?'];
	}
	return Characters[c];
}
