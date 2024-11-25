#pragma once
#include <Windows.h>
#include <map>
#include <memory>
#include <string>

#include "Shader.h"


class VertexArray;
class IndexBuffer;
class Shader;
class Font;
class Bitmap;
class Buffer;
class ElementBuffer;
class color
{
public:
	float R, G, B, A;
	color(float r, float g, float b, float a) :R(r), G(g), B(b), A(a) {}
	color() :R(0), G(0), B(0), A(0) {}
	color& operator=(unsigned int RGBA);
	operator unsigned int() const;
};
class FpsCounter
{
	// ֡�ʼ������
    double lastTime =0;
	int nbFrames = 0;
	std::atomic<int> FPS = 0;
public:
	void newFrame();
	unsigned int getFps() const { return FPS; }
};

class EasyGL
{
public:
    EasyGL();
    ~EasyGL();

    // ��ʼ�� OpenGL ������
    bool Init(HWND hwnd);
    // ������Դ
    void Cleanup();

    // ��ʼ��Ⱦ
    void BeginRender() const;
    // ������Ⱦ
    void EndRender() const;
    // �����ӿڴ�С
    void Resize(int width, int height);

    // ����λͼ
    void DrawBitmap(float x, float y, const Bitmap& bitmap, float angle = 0.0f) const;
    void DrawBitmap(float x, float y, float width, float height, const Bitmap& bitmap, float angle = 0.0f) const;

    // ���� VAO
    void DrawVAO(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

    // ���������ɫ
    void SetClearColor(const color& color);

    // �Ƿ��ʼ���ɹ�
    bool IsInitialized() const { return isInit; }

private:
    HDC hDC = nullptr;       // �豸������
    HGLRC hRC = nullptr;     // ��Ⱦ������
    bool isInit = false;     // �Ƿ��ʼ���ɹ�

    int windowWidth = 800;   // ���ڿ��
    int windowHeight = 600;  // ���ڸ߶�

    color clearColor = { 0.1f, 0.1f, 0.1f, 1.0f }; // �����ɫ
};