#include"core/render/IndexBuffer.h"
#include"core/log.h"
#include<glad/glad.h>

using namespace core;

IndexBuffer::IndexBuffer()
    : rendererID(0), count(0)
{
    glGenBuffers(1, &rendererID);
    Log<<Level::Info<<"IndexBuffer::IndexBuffer() "<<rendererID<<op::endl;
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : rendererID(0), count(count)
{
    glGenBuffers(1, &rendererID);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    Log<<Level::Info<<"IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) "<<rendererID<<op::endl;
}

IndexBuffer::IndexBuffer(const IndexBuffer& ib)
    : rendererID(0), count(ib.count)
{
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_COPY_READ_BUFFER, ib.rendererID);
	glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID);
	glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, count * sizeof(unsigned int));
    Log<<Level::Info<<"IndexBuffer::IndexBuffer(const IndexBuffer& ib) "<<rendererID<<op::endl;
}

IndexBuffer::~IndexBuffer()
{
    Log<<Level::Info<<"IndexBuffer::~IndexBuffer() "<<rendererID<<op::endl;
    glDeleteBuffers(1, &rendererID);
    rendererID = 0;
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib)
{
    Log<<Level::Info<<"IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib) "<<rendererID<<op::endl;
    if (this != &ib)
    {
        glDeleteBuffers(1, &rendererID);
        rendererID = 0;
        count = ib.count;
        glGenBuffers(1, &rendererID);
        glBindBuffer(GL_COPY_READ_BUFFER, ib.rendererID);
        glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID);
        glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, count * sizeof(unsigned int));
    }
    return *this;
}
