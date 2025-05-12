#include"core/render/IndexBuffer.h"
#include"core/log.h"

#include"core/render/GLBase.h"
#include<glad/glad.h>


using namespace core;

IndexBuffer::IndexBuffer()
    : rendererID(0), count(0)
{
    GLCall(glGenBuffers(1, &rendererID));
    Log<<Level::Info<<"IndexBuffer::IndexBuffer() "<<rendererID<<op::endl;
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : rendererID(0), count(count)
{
    GLCall(glGenBuffers(1, &rendererID));
    Bind();
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
    Log<<Level::Info<<"IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) "<<rendererID<<op::endl;
}

IndexBuffer::IndexBuffer(const IndexBuffer& ib)
    : rendererID(0), count(ib.count)
{
	GLCall(glGenBuffers(1, &rendererID));
	GLCall(glBindBuffer(GL_COPY_READ_BUFFER, ib.rendererID));
	GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID));
	GLCall(glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(unsigned int), nullptr, GL_STATIC_DRAW));
	GLCall(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, count * sizeof(unsigned int)));
    Log<<Level::Info<<"IndexBuffer::IndexBuffer(const IndexBuffer& ib) "<<rendererID<<op::endl;
}

IndexBuffer::~IndexBuffer()
{
    Log<<Level::Info<<"IndexBuffer::~IndexBuffer() "<<rendererID<<op::endl;
    GLCall(glDeleteBuffers(1, &rendererID));
    rendererID = 0;
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib)
{
    Log<<Level::Info<<"IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib) "<<rendererID<<op::endl;
    if (this != &ib)
    {
        GLCall(glDeleteBuffers(1, &rendererID));
        rendererID = 0;
        count = ib.count;
        GLCall(glGenBuffers(1, &rendererID));
        GLCall(glBindBuffer(GL_COPY_READ_BUFFER, ib.rendererID));
        GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, rendererID));
        GLCall(glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(unsigned int), nullptr, GL_STATIC_DRAW));
        GLCall(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, count * sizeof(unsigned int)));
    }
    return *this;
}
