#include"core/render/IndexBuffer.h"
#include"core/log.h"
#include"core/decrash/OpenGLErrorRecovery.h"
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
    if (core::OpenGLErrorRecovery::isContextValid() && rendererID != 0) {
        try {
            GLCall(glDeleteBuffers(1, &rendererID));
        } catch (const std::exception& e) {
            Log << Level::Error << "Exception while deleting index buffer " << rendererID << ": " << e.what() << op::endl;
        } catch (...) {
            Log << Level::Error << "Unknown exception while deleting index buffer " << rendererID << op::endl;
        }
    } else if (rendererID != 0) {
        Log << Level::Warn << "OpenGL context invalid, skipping index buffer " << rendererID << " deletion" << op::endl;
    }
    rendererID = 0;
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib)
{
    Log<<Level::Info<<"IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib) "<<rendererID<<op::endl;
    if (this != &ib)
    {
        if (core::OpenGLErrorRecovery::isContextValid() && rendererID != 0) {
            GLCall(glDeleteBuffers(1, &rendererID));
        }
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
