// Canvas.h
#pragma once
#include <glad/glad.h>
#include"Shader.h"
#include "VertexBuffer.h"

class Canvas {
public:
	// ���캯���������µĻ�������Ӵ��ڿ�Ⱥ͸߶Ȳ���
	Canvas(int width, int height, int screenWidth, int screenHeight);

	// ����������������Դ
	~Canvas();

	// ��ʼ�ڻ����ϻ���
	void Begin();

	// �����ڻ����ϵĻ��ƣ����ش��ڻ���
	void End();

	// ���������ݻ��Ƶ���Ļ�ϵ�ָ��λ��
	void DrawToScreen(float x, float y, float width, float height);

	// �����������ݣ�������Ҫ����ʵ��Ϊ��������һ������
	void Copy(Canvas& targetCanvas);

	// ��ȡ�����Ŀ�Ⱥ͸߶�
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

private:
	// ��ʼ��֡�������
	void InitFramebuffer();

	// ��ʼ����Ļ�ı���
	void InitScreenQuad();
	GLuint fbo; // FBO����ľ��
	GLuint rbo; // ��Ⱦ����������ľ��
	GLuint colorBuffer; // ��ɫ����������ľ��
	GLuint depthBuffer; // ��Ȼ���������ľ��
	GLuint texture; // ���ɵ��������ľ��
	// �����Ŀ�Ⱥ͸߶�
	int width;
	int height;

	// ���ڵĿ�Ⱥ͸߶�
	int screenWidth;
	int screenHeight;

	// ʹ�����Ļ����������ԭʼ�� OpenGL ���� ID
	VertexArray quadVAO;
	VertexBuffer quadVBO;

	// ��ɫ���������ڻ�����Ļ�ı���
	static Shader screenShader;
};
