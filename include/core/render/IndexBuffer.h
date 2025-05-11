#pragma once
#include <glad/glad.h>
#pragma execution_character_set("utf-8")

namespace core {

class IndexBuffer
{
	unsigned int rendererID;
	unsigned int count;
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	IndexBuffer(const IndexBuffer& ib);
	~IndexBuffer();

	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID); }
	static void Unbind(){ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

	inline unsigned int getCount() const { return count; }
	inline unsigned int getRendererID() const { return rendererID; }

	IndexBuffer& operator=(const IndexBuffer& ib);
};
}