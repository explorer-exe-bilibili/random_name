#include "VertexBuffer.h"
#include <glad/glad.h>

#include "OpenGLBase.h"

VertexBuffer::VertexBuffer()
	: rendererID(0), size(0)
{
	glGenBuffers(1, &rendererID);
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	: rendererID(0), size(size)
{
	glGenBuffers(1, &rendererID);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(const VertexBuffer& vb)
	: rendererID(0), size(vb.size)
{
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_COPY_READ_BUFFER, vb.rendererID);
	glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID);
	glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &rendererID);
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
}

void VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::BufferData(const void* data, unsigned int size)
{
	this->size = size;
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::BufferSubData(unsigned int offset, unsigned int size, const void* data) const
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vb)
{
	if (this != &vb)
	{
		// 复制新的缓冲区
		size = vb.size;
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_COPY_READ_BUFFER, vb.rendererID);
		glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID);
		glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_STATIC_DRAW);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
	}
	return *this;
}

IndexBuffer::IndexBuffer()
	: rendererID(0), count(0)
{
	glGenBuffers(1, &rendererID);
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: rendererID(0), count(count)
{
	glGenBuffers(1, &rendererID);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(const IndexBuffer& ib)
	: rendererID(0), count(ib.count)
{
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_COPY_READ_BUFFER, ib.rendererID);
	glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID);
	glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, count * sizeof(unsigned int));
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &rendererID);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
}

void IndexBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib)
{
	if (this != &ib)
	{
		glDeleteBuffers(1, &rendererID);

		count = ib.count;
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_COPY_READ_BUFFER, ib.rendererID);
		glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID);
		glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, count * sizeof(unsigned int));
	}
	return *this;
}

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &rendererID);
}

VertexArray::VertexArray(const VertexArray& va)
{
	glGenVertexArrays(1, &rendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &rendererID);
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(rendererID));
}

void VertexArray::Unbind()
{
	glBindVertexArray(0);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, unsigned int index, unsigned int size, unsigned int type, bool normalized, unsigned int stride, const void* pointer) const
{
	Bind();
	vb.Bind();
	GLCall(glEnableVertexAttribArray(index));
	GLCall(glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer));
}

VertexArray& VertexArray::operator=(const VertexArray& va)
{
	if (this != &va)
	{
		glDeleteVertexArrays(1, &rendererID);
		glGenVertexArrays(1, &rendererID);
	}
	return *this;
}

Texture::Texture() {
	glGenTextures(1, &ID);
}

Texture::~Texture() {
	glDeleteTextures(1, &ID);
}

bool Texture::LoadFromData(unsigned char* data, int width, int height, GLenum format) {
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void Texture::Bind() const {
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrapMode(GLenum wrapS, GLenum wrapT) {
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFilterMode(GLenum minFilter, GLenum magFilter) {
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::GetID() const {
	return ID;
}