#include "core/baseItem/Font.h"
#include "core/render/VertexArray.h"
#include FT_FREETYPE_H

#include <iostream>
#include <mutex>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <filesystem>

#include "core/render/GLBase.h"
#include "core/baseItem/Base.h"
#include "core/decrash/OpenGLErrorRecovery.h"
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
uniform float alphaMultiplier;

void main()
{
	float alpha = texture(text, TexCoords).r * alphaMultiplier;
	color = vec4(textColor, alpha);
}
)";

std::shared_ptr<Font> Font::spare_font = nullptr;
Shader Font::shader;

Font::Font(const std::string& fontPath,bool needPreLoad, unsigned int fontSize)
	: fontSize(fontSize), face(nullptr), isOK(false)
{
	std::cout << "loading Font file "<<fontPath<<std::endl;
	if(!std::filesystem::exists(fontPath)){
		return;
	}
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
		spare_font = std::make_shared<Font>("files/fonts/spare.ttf", 0);
	}
	face = nullptr;
	// 加载字体
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
		std::cerr << "无法加载字体: " << fontPath << std::endl;
		FT_Done_FreeType(ft);
		isOK=false;
		return;
	}
	// 设置字体大小
	FT_Set_Pixel_Sizes(face, 0, fontSize);

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
	isOK=true;
}

Font::~Font() {
	// 释放纹理 - 使用OpenGL错误恢复
	if (core::OpenGLErrorRecovery::isContextValid()) {
		for (auto& pair : Characters) {
			try {
				glDeleteTextures(1, &pair.second.TextureID);
			} catch (const std::exception& e) {
				Log << Level::Error << "Exception while deleting texture for character: " << e.what() << op::endl;
			} catch (...) {
				Log << Level::Error << "Unknown exception while deleting texture" << op::endl;
			}
		}
	} else {
		Log << Level::Warn << "OpenGL context invalid, skipping texture deletion in Font destructor" << op::endl;
	}

	// 释放 FreeType 资源
	if (face) {
		FT_Done_Face(face);
	}
}

void Font::RenderText(const std::string& text,float x, float y,float scale, const glm::vec4& color){
	std::wstring wtext = string2wstring(text);
	RenderText(wtext, x, y, scale, color);
}

void Font::RenderText(const std::wstring& text, float x, float y_origin, float scale, const glm::vec4& color) {
	scale = CalculateDynamicScale(scale);
	float y=WindowInfo.height-y_origin-fontSize*scale;
	//检查是否包含未加载的字符
	for (auto c = text.begin(); c != text.end(); ++c) {
		if (!Characters.contains(*c))
		{
			LoadCharacter(*c);
		}
	}
	// 设置正交投影矩阵
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowInfo.width), 0.0f, static_cast<float>(WindowInfo.height));
	// 设置字体着色器中的投影矩阵
	shader.use();
	shader.setMat4("projection", projection);
	shader.setVec3("textColor", color);
	shader.setFloat("alphaMultiplier", color.a);
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
}

void Font::RenderTextBetween(const std::string& text, Region region, float scale, const glm::vec4& color) {
	std::wstring wtext=string2wstring(text);
	RenderTextBetween(wtext, region, scale, color);
}

void Font::RenderTextBetween(const std::wstring& text, Region region, float scale_, const glm::vec4& color) {
	float scale = CalculateDynamicScale(scale_);
	//计算可用的区域宽度
	float availableWidth = region.getxend() - region.getx();
	float availableHeight = region.getyend() - region.gety();

	if (availableWidth <= 0 || availableHeight <= 0 || text.empty()) {
		return; // 没有可用空间或文本为空，直接返回
	}

	// 测量文本宽度，同时加载未加载的字符
	float textWidth = 0.0f;
	std::wstring displayText = text;
	
	for (size_t i = 0; i < text.length(); ++i) {
		wchar_t c = text[i];
		if (!Characters.contains(c)) {
			LoadCharacter(c);
		}
		
		Character ch = Characters[c];
		float charWidth = (ch.Advance >> 6) * scale;
		
		// 检查是否超出可用宽度
		if (textWidth + charWidth > availableWidth) {
			// 截断文本
			displayText = text.substr(0, i);
			break;
		}
		
		textWidth += charWidth;
	}
	
	// 如果displayText为空（即第一个字符就超出区域），则至少显示一个字符
	if (displayText.empty() && !text.empty()) {
		displayText = text.substr(0, 1);
		if (!Characters.contains(displayText[0])) {
			LoadCharacter(displayText[0]);
		}
		textWidth = (Characters[displayText[0]].Advance >> 6) * scale;
	}
	
	// 计算起始位置（水平居中）
	float x = region.getx() + (availableWidth - textWidth) / 2.0f;
	float y = region.gety() + (availableHeight - fontSize * scale) / 2.0f;

	// 渲染截断后的文本
	RenderText(displayText, x, y, scale_, color);
}

void Font::RenderTextCentered(const std::string& text, Region region, float scale, const glm::vec4& color) {
	std::wstring wtext = string2wstring(text);
	RenderTextCentered(wtext, region, scale, color);
}

void Font::RenderTextCentered(const std::wstring& text, Region region, float scale_, const glm::vec4& color) {
	float scale = CalculateDynamicScale(scale_);
	
	// 计算可用的区域宽度和高度
	float availableWidth = region.getxend() - region.getx();
	float availableHeight = region.getyend() - region.gety();

	if (availableWidth <= 0 || availableHeight <= 0 || text.empty()) {
		return; // 没有可用空间或文本为空，直接返回
	}

	// 加载所有未加载的字符
	for (auto c = text.begin(); c != text.end(); ++c) {
		if (!Characters.contains(*c)) {
			LoadCharacter(*c);
		}
	}

	// 测量完整文本的宽度
	float textWidth = 0.0f;
	for (auto c = text.begin(); c != text.end(); ++c) {
		Character ch = Characters[*c];
		textWidth += (ch.Advance >> 6) * scale;
	}
	
	// 计算起始位置（水平和垂直居中）
	float x = region.getx() + (availableWidth - textWidth) / 2.0f;
	float y = region.gety() + (availableHeight - fontSize * scale) / 2.0f;

	// 渲染完整文本（不截断）
	RenderText(text, x, y, scale_, color);
}

void Font::RenderChar(wchar_t text, float x, float y_origin, float scale, const glm::vec4& color) {
	scale = CalculateDynamicScale(scale);
	float y = WindowInfo.height - y_origin - fontSize * scale;
	//检查是否包含未加载的字符
	if (!Characters.contains(text))
	{
		LoadCharacter(text);
	}
	// 设置正交投影矩阵
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowInfo.width), 0.0f, static_cast<float>(WindowInfo.height));
	// 设置字体着色器中的投影矩阵
	shader.use();
	shader.setMat4("projection", projection);
	shader.setVec3("textColor", color);
	shader.setFloat("alphaMultiplier", color.a);
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
}

void Font::RenderCharFitRegion(wchar_t text, float x, float y, float xend, float yend, const glm::vec4& color) {
	// 检查区域有效性
	if (xend <= x || yend <= y) {
		return; // 无效区域，直接返回
	}

	// 检查是否包含未加载的字符
	if (!Characters.contains(text)) {
		LoadCharacter(text);
	}

	Character ch = Characters[text];
	
	// 计算可用区域的宽度和高度
	float availableWidth = xend - x;
	float availableHeight = yend - y;
	
	// 计算字符的实际尺寸（不含缩放）
	float charWidth = ch.Advance >> 6; // 字符的原始宽度（单位为像素）
	
	// 如果字符尺寸为0，则无法渲染
	if (charWidth <= 0) {
		return;
	}
	
	// 计算缩放因子，确保字符能完全填充区域
	float scaleX = availableWidth / charWidth;
	
	float scale=DeCalculateDynamicScale(scaleX);
	x-= ch.Bearing.x * scale;
	y-= ch.Bearing.y * scale/2.0;
	RenderChar(text, x, y, scale, color);
}

bool Font::operator==(const Font& b) const
{
	if (face == b.face)return true;
	return false;
}
static std::mutex mx;
bool Font::LoadCharacter(wchar_t c)
{
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

unsigned int Font::GetFontSize() const
{
	return fontSize;
}

Character Font::GetCharacter(wchar_t c)
{
	if (!Characters.contains(c))
	{
		if (!LoadCharacter(c))return Characters[L'?'];
	}
	return Characters[c];
}

void Font::RenderTextVertical(const std::string& text, float x, float y, float scale, const glm::vec4& color) {
	std::wstring wtext = string2wstring(text);
	RenderTextVertical(wtext, x, y, scale, color);
}

void Font::RenderTextVertical(const std::wstring& text, float x, float y_origin, float scale, const glm::vec4& color) {
	scale = CalculateDynamicScale(scale);
	float y = WindowInfo.height - y_origin - fontSize * scale;
	//检查是否包含未加载的字符
	for (auto c = text.begin(); c != text.end(); ++c) {
		if (!Characters.contains(*c)) {
			LoadCharacter(*c);
		}
	}
	// 设置正交投影矩阵
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowInfo.width), 0.0f, static_cast<float>(WindowInfo.height));
	// 设置字体着色器中的投影矩阵
	shader.use();
	shader.setMat4("projection", projection);
	shader.setVec3("textColor", color);
	shader.setFloat("alphaMultiplier", color.a);
	// 启用混合以处理文本的透明度
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glActiveTexture(GL_TEXTURE0));
	VAO.Bind();
	
	float current_y = y;
	// 垂直方向从上到下渲染文本
	for (auto c = text.begin(); c != text.end(); ++c) {
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = current_y - (ch.Size.y - ch.Bearing.y) * scale;

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

		// 移动到下一个字符位置（向下移动）
		current_y -= ch.Size.y * scale * 1.2f; // 添加一点额外间距
	}

	VertexArray::Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::RenderTextVerticalBetween(const std::string& text, Region region, float scale, const glm::vec4& color) {
	std::wstring wtext = string2wstring(text);
	RenderTextVerticalBetween(wtext, region, scale, color);
}

void Font::RenderTextVerticalBetween(const std::wstring& text, Region region, float scale_, const glm::vec4& color) {
	float scale = CalculateDynamicScale(scale_);

	//计算可用的区域高度
	float availableWidth = region.getxend() - region.getx();
	float availableHeight = region.getyend() - region.gety();

	if (availableWidth <= 0 || availableHeight <= 0 || text.empty()) {
		return; // 没有可用空间或文本为空，直接返回
	}

	// 测量文本高度，同时加载未加载的字符
	float textHeight = 0.0f;
	std::wstring displayText = text;
	
	for (size_t i = 0; i < text.length(); ++i) {
		wchar_t c = text[i];
		if (!Characters.contains(c)) {
			LoadCharacter(c);
		}
		
		Character ch = Characters[c];
		float charHeight = ch.Size.y * scale * 1.2f; // 添加一点额外间距
		
		// 检查是否超出可用高度
		if (textHeight + charHeight > availableHeight) {
			// 截断文本
			displayText = text.substr(0, i);
			break;
		}
		
		textHeight += charHeight;
	}
	
	// 如果displayText为空（即第一个字符就超出区域），则至少显示一个字符
	if (displayText.empty() && !text.empty()) {
		displayText = text.substr(0, 1);
		if (!Characters.contains(displayText[0])) {
			LoadCharacter(displayText[0]);
		}
		textHeight = Characters[displayText[0]].Size.y * scale * 1.2f;
	}
	
	// 计算起始位置（垂直居中）
	float x = region.getx() + (availableWidth - fontSize * scale) / 2.0f;
	float y = region.gety() + (availableHeight - textHeight) / 2.0f;
	
	// 渲染截断后的垂直文本
	RenderTextVertical(displayText, x, y, scale_, color);
}

// 添加一个计算动态缩放因子的私有方法
float Font::CalculateDynamicScale(float baseScale) const {
    // 获取窗口当前尺寸
    float currentWidth = static_cast<float>(WindowInfo.width);
    float currentHeight = static_cast<float>(WindowInfo.height);
    
    // 选择一个参考尺寸（可以根据需要调整）
    const float referenceWidth = 800.0f;
    const float referenceHeight = 600.0f;
    
    // 计算当前窗口与参考尺寸的比例
    float widthRatio = currentWidth / referenceWidth;
    float heightRatio = currentHeight / referenceHeight;
    
    // 使用较小的比例以确保在任何方向上都不会太大
    float scaleFactor = std::min(widthRatio, heightRatio);
    
    // 应用用户提供的基础缩放并返回
    return baseScale * scaleFactor;
}

float Font::DeCalculateDynamicScale(float scaledValue) const {
    // 获取窗口当前尺寸
    float currentWidth = static_cast<float>(WindowInfo.width);
    float currentHeight = static_cast<float>(WindowInfo.height);

    // 选择一个参考尺寸（可以根据需要调整）
    const float referenceWidth = 800.0f;
    const float referenceHeight = 600.0f;

    // 计算当前窗口与参考尺寸的比例
    float widthRatio = currentWidth / referenceWidth;
    float heightRatio = currentHeight / referenceHeight;

    // 使用较小的比例以确保在任何方向上都不会太大
    float scaleFactor = std::min(widthRatio, heightRatio);

    // 反向应用缩放因子
    return scaledValue / scaleFactor;
}