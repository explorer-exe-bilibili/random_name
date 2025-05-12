#pragma once
#include <glad/glad.h>
#include "GLBase.h"


namespace core {

class VertexBuffer
{
	unsigned int rendererID=0;
	unsigned int size=0;
public:	VertexBuffer(){GLCall(glGenBuffers(1, &rendererID));}
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(const VertexBuffer& vb);
	~VertexBuffer();

	void Bind() const{ GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID)); } ;
	static void Unbind(){GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));};

	void BufferData(const void* data, unsigned int size);
	void BufferSubData(unsigned int offset, unsigned int size, const void* data) const;

	inline unsigned int getRendererID() const { return rendererID; }

	VertexBuffer& operator=(const VertexBuffer& vb);
};
}