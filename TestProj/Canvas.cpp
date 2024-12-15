// Canvas.cpp
#include "Canvas.h"
#include <iostream>

Shader Canvas::screenShader;

// ��Ļ�ı��εĶ�����ɫ��
const char* screenVertexShaderSource = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
	gl_Position = vec4(aPos, 0.0, 1.0);
	TexCoords = aTexCoords;
}
)";

// ��Ļ�ı��ε�Ƭ����ɫ��
const char* screenFragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
	FragColor = texture(screenTexture, TexCoords);
}
)";


Canvas::Canvas(int width, int height, int screenWidth, int screenHeight)
	: width(width), height(height), screenWidth(screenWidth), screenHeight(screenHeight),
	fbo(0), texture(0), rbo(0)
{
	// ������Ļ��ɫ��
	if (!screenShader)screenShader.init(screenFragmentShaderSource, screenFragmentShaderSource);

	InitFramebuffer();
	InitScreenQuad();
}

Canvas::~Canvas()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &texture);
	glDeleteRenderbuffers(1, &rbo);
}

void Canvas::InitFramebuffer()
{
	// 1������FBO����
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 2��������ɫ���������󲢸��ӵ�FBO
	glGenRenderbuffers(1, &colorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);

	// 3��������Ȼ��������󲢸��ӵ�FBO
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	// 4������������󲢸��ӵ�FBO
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// ���FBO�Ƿ�����
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// �������
		std::cout << "Framebuffer is not complete!" << std::endl;
	}

}

void Canvas::InitScreenQuad()
{
	float quadVertices[6 * 4] = { 0 };

	quadVAO.Bind();
	quadVBO.Bind();
	quadVBO.BufferData(quadVertices, sizeof(quadVertices));

	// λ������
	quadVAO.AddBuffer(quadVBO, 0, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)0);
	// ������������
	quadVAO.AddBuffer(quadVBO, 1, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	quadVBO.Unbind();
	quadVAO.Unbind();
}

void Canvas::Begin()
{
	// ���Ƴ�����FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
}

void Canvas::End()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);

}

void Canvas::DrawToScreen(float x, float y, float drawWidth, float drawHeight)
{
	float x1 = (x / screenWidth) * 2.0f - 1.0f;
	float y1 = (y / screenHeight) * 2.0f - 1.0f;
	float x2 = ((x + drawWidth) / screenWidth) * 2.0f - 1.0f;
	float y2 = ((y + drawHeight) / screenHeight) * 2.0f - 1.0f;

	float quadVertices[] = {
		x1, y2,   0.0f, 1.0f,
		x1, y1,   0.0f, 0.0f,
		x2, y1,   1.0f, 0.0f,

		x1, y2,   0.0f, 1.0f,
		x2, y1,   1.0f, 0.0f,
		x2, y2,   1.0f, 1.0f
	};
	screenShader.use();

	quadVBO.Bind();
	quadVBO.BufferSubData(0, sizeof(quadVertices), quadVertices);
	quadVBO.BufferData(quadVertices, sizeof(quadVertices));

	screenShader.setInt("screenTexture", 0);
	glDisable(GL_DEPTH_TEST);
	quadVAO.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	quadVBO.Unbind();

	// �������� VAO
	glBindTexture(GL_TEXTURE_2D, 0);
	quadVAO.Unbind();

	// �ָ� OpenGL ״̬
	glUseProgram(0);
}

void Canvas::Copy(Canvas& targetCanvas)
{
	// ������ǰ�������ݵ�Ŀ�껭��
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetCanvas.fbo);
	glBlitFramebuffer(0, 0, width, height, 0, 0, targetCanvas.width, targetCanvas.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}