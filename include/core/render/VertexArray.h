#pragma once
#include <glad/glad.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#pragma execution_character_set("utf-8")

namespace core {

class VertexArray
{
	unsigned int rendererID=0;
public:
	VertexArray();
	VertexArray(const VertexArray& va);
	~VertexArray();

	void Bind() const { glBindVertexArray(rendererID); };
	static void Unbind() { glBindVertexArray(0); };

	// 添加缓冲区并设置属性指针
	void AddBuffer(const VertexBuffer& vb, unsigned int index, unsigned int size, unsigned int type, bool normalized, unsigned int stride, const void* pointer) const;

	void SetAttributePointer(unsigned int index, unsigned int size, unsigned int type, bool normalized, unsigned int stride, const void* pointer) const;
    void SetElementBuffer(const IndexBuffer& ib) const;
    void SetVertexBuffer(const VertexBuffer& vb) const;

	inline unsigned int getRendererID() const { return rendererID; }

	VertexArray& operator=(const VertexArray& va);
};
}