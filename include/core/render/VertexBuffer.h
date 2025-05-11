#pragma once
#include <glad/glad.h>
#pragma execution_character_set("utf-8")

namespace core {

class VertexBuffer
{
	unsigned int rendererID;
	unsigned int size;
public:
	VertexBuffer(){glGenBuffers(1, &rendererID);}
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(const VertexBuffer& vb);
	~VertexBuffer();

	void Bind() const{ glBindBuffer(GL_ARRAY_BUFFER, rendererID); } ;
	static void Unbind(){glBindBuffer(GL_ARRAY_BUFFER, 0);};

	void BufferData(const void* data, unsigned int size);
	void BufferSubData(unsigned int offset, unsigned int size, const void* data) const;

	inline unsigned int getRendererID() const { return rendererID; }

	VertexBuffer& operator=(const VertexBuffer& vb);
};
}