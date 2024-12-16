#pragma once

class VertexBuffer
{
	unsigned int rendererID;
	unsigned int size;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(const VertexBuffer& vb);
	~VertexBuffer();

	void Bind() const;
	static void Unbind();

	void BufferData(const void* data, unsigned int size);
	void BufferSubData(unsigned int offset, unsigned int size, const void* data) const;

	inline unsigned int getRendererID() const { return rendererID; }

	VertexBuffer& operator=(const VertexBuffer& vb);
};

class IndexBuffer
{
	unsigned int rendererID;
	unsigned int count;
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	IndexBuffer(const IndexBuffer& ib);
	~IndexBuffer();

	void Bind() const;
	static void Unbind();

	inline unsigned int getCount() const { return count; }
	inline unsigned int getRendererID() const { return rendererID; }

	IndexBuffer& operator=(const IndexBuffer& ib);
};

class VertexArray
{
	unsigned int rendererID;
public:
	VertexArray();
	VertexArray(const VertexArray& va);
	~VertexArray();

	void Bind() const;
	static void Unbind();

	// 添加缓冲区并设置属性指针
	void AddBuffer(const VertexBuffer& vb, unsigned int index, unsigned int size, unsigned int type, bool normalized, unsigned int stride, const void* pointer) const;

	inline unsigned int getRendererID() const { return rendererID; }

	VertexArray& operator=(const VertexArray& va);
};

class Texture {
public:
	Texture();
	~Texture();

	// 从内存数据加载纹理
	bool LoadFromData(unsigned char* data, int width, int height, unsigned int format);

	// 绑定纹理
	void Bind() const;

	// 解绑纹理
	static void Unbind();

	// 设置纹理参数
	void SetWrapMode(unsigned int wrapS, unsigned int wrapT);
	void SetFilterMode(unsigned int minFilter, unsigned int magFilter);

	// 获取纹理ID
	unsigned int GetID() const;

private:
	unsigned int ID;
};