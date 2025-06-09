#include "core/render/VertexBuffer.h"
#include "core/log.h"
#include "core/render/GLBase.h"
#include "core/OpenGLErrorRecovery.h"
#include <cstdlib>


using namespace core;

VertexBuffer::VertexBuffer(const void* data, unsigned int size) : size(size) {
    GLCall(glGenBuffers(1, &rendererID));
    Bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    Unbind();
    if (rendererID == 0) {
        Log<<Level::Error<<"VertexBuffer::VertexBuffer(const void* data, unsigned int size) rendererID is 0"<<op::endl;
        return;
    }
}

VertexBuffer::VertexBuffer(const VertexBuffer& vb) : size(vb.size) {
    // 生成新的缓冲区ID
    GLCall(glGenBuffers(1, &rendererID));
    if (rendererID == 0) {
        Log<<Level::Error<<"VertexBuffer::VertexBuffer(const VertexBuffer& vb) rendererID is 0"<<op::endl;
        return;
    }
    // 保存当前绑定的缓冲区，以便后续恢复
    GLint previousBuffer=0;
    GLCall(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousBuffer));
    
    // 绑定源缓冲区并获取数据
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vb.rendererID));
    void* data = nullptr;
    
    // 只有在缓冲区有大小时才分配内存
    if (size > 0) {
        data = malloc(size);
        if (data) {
            // 读取源缓冲区的数据
            GLCall(glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
        }
    }
    
    // 绑定新缓冲区并复制数据
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    
    // 释放临时数据
    if (data) {
        free(data);
    }
      // 恢复之前绑定的缓冲区
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, previousBuffer));
    Log<<Level::Info<<"VertexBuffer::VertexBuffer(const VertexBuffer& vb) finished "<<rendererID<<op::endl;
}

VertexBuffer::~VertexBuffer() {
    if (core::OpenGLErrorRecovery::isContextValid() && rendererID != 0) {
        try {
            GLCall(glDeleteBuffers(1, &rendererID));
        } catch (const std::exception& e) {
            Log << Level::Error << "Exception while deleting vertex buffer " << rendererID << ": " << e.what() << op::endl;
        } catch (...) {
            Log << Level::Error << "Unknown exception while deleting vertex buffer " << rendererID << op::endl;
        }
    } else if (rendererID != 0) {
        Log << Level::Warn << "OpenGL context invalid, skipping vertex buffer " << rendererID << " deletion" << op::endl;
    }
}

void VertexBuffer::BufferData(const void* data, unsigned int size) {
    if (rendererID == 0) {
        Log<<Level::Error<<"VertexBuffer::BufferData() rendererID is 0"<<op::endl;
        return;
    }
    Bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    Unbind();
}

void VertexBuffer::BufferSubData(unsigned int offset, unsigned int size, const void* data) const {
    if (rendererID == 0) {
        Log<<Level::Error<<"VertexBuffer::BufferSubData() rendererID is 0"<<op::endl;
        return;
    }
    Bind();
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
    Unbind();
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vb) {
    Log<<Level::Info<<"VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vb) "<<this->rendererID<<" from "<<vb.rendererID<<op::endl;
    if (this != &vb) {
        // 先删除当前的缓冲区
        if (rendererID != 0) {
            if (core::OpenGLErrorRecovery::isContextValid()) {
                GLCall(glDeleteBuffers(1, &rendererID));
            }
        }
        
        // 生成新的缓冲区ID
        GLCall(glGenBuffers(1, &rendererID));
        
        // 绑定源缓冲区并获取数据
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, vb.rendererID));
        void* data = nullptr;
        
        // 只有在缓冲区有大小时才分配内存
        if (size > 0) {
            data = malloc(size);
            if (data) {
                // 读取源缓冲区的数据
                GLCall(glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
            }
        }
        
        // 绑定新缓冲区并复制数据
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
        
        // 释放临时数据
        if (data) {
            free(data);
        }
    }
    Log<<Level::Info<<"VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vb) finished "<<this->rendererID<<op::endl;
    return *this;
}