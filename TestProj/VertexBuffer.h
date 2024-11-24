#pragma once
#include <map>
#include <string>
#include <vector>

class VertexBuffer
{
	unsigned int rendererID;
	unsigned int size;
	void* data;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(const VertexBuffer& vb);
	~VertexBuffer();

	inline unsigned int getRendererID() const { return rendererID; }

	VertexBuffer& operator=(const VertexBuffer& vb);

	void Bind() const;
	static void enableIndexBuffer(unsigned int index, unsigned int size, unsigned int type, 
	                              unsigned int stride, unsigned int offset);
	static void Unbind();
};

class IndexBuffer
{
	unsigned int rendererID;
	std::map<std::string,VertexBuffer> VBs;
	std::string using_name;
	unsigned int* data;
	unsigned int count;
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	IndexBuffer(const IndexBuffer& ib);
	~IndexBuffer();
	void Bind() const;
	static void Unbind();
	void BindVertexBuffer(std::string name,VertexBuffer& vb);
	void unBindVertexBuffer(std::string name);
	void useVertexBuffer(std::string name);

	inline unsigned int getRendererID() const { return rendererID; }

	IndexBuffer& operator=(const IndexBuffer& ib);
};

class VertexArray
{
	unsigned int rendererID;
	std::vector<VertexBuffer> bindVBs;
	std::vector<IndexBuffer> bindIBs;
public:
	VertexArray();
	VertexArray(const VertexArray& va);
	~VertexArray();
	void AddBuffer(const VertexBuffer& vb, const IndexBuffer& ib) const;
	void Bind() const;
	static void unbind();

	inline unsigned int getRendererID() const { return rendererID; }

	VertexArray& operator=(const VertexArray& va);
};