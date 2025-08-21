#include "core/render/VertexArray.h"
#include "core/log.h"
#include "core/render/GLBase.h"


using namespace core;

VertexArray::VertexArray() {
    GLCall(glGenVertexArrays(1, &rendererID));
}

VertexArray::VertexArray(const VertexArray& va) {
    // 生成新的顶点数组对象
    GLCall(glGenVertexArrays(1, &rendererID));
    Log<<Level::Info<<"VertexArray::VertexArray(const VertexArray& va) "<<rendererID<<op::endl;
    // 保存当前绑定的VAO，以便操作后恢复
    GLint previousVAO;
    GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO));
    
    // 绑定源VAO
    va.Bind();
    
    // 收集源VAO的属性信息
    // 假设最多支持16个顶点属性
    const int maxAttribs = 16;
    for (int i = 0; i < maxAttribs; i++) {
        GLint enabled = 0;
        GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled));
        
        if (enabled) {
            // 获取源属性配置
            GLint size = 0, type = 0, normalized = 0, stride = 0;
            void* pointer = nullptr;
            
            GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size));
            GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type));
            GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized));
            GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride));
            GLCall(glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer));
            
            // 获取当前绑定的VBO
            GLint currentVBO = 0;
            GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &currentVBO));
            
            // 绑定我们的VAO
            Bind();
            
            // 绑定相同的VBO
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, currentVBO));
            
            // 复制属性配置到我们的VAO
            GLCall(glEnableVertexAttribArray(i));
            GLCall(glVertexAttribPointer(
                i, 
                size, 
                type, 
                normalized == GL_TRUE, 
                stride, 
                pointer
            ));
        }
    }
    
    // 检查索引缓冲区(EBO)是否绑定
    va.Bind();
    GLint elementArrayBufferBinding;
    GLCall(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding));
    
    if (elementArrayBufferBinding) {
        // 如果源VAO有EBO绑定，我们也应该绑定相同的EBO到新VAO
        Bind();
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding));
    }
      // 恢复原来绑定的VAO
    GLCall(glBindVertexArray(previousVAO));
    Log<<Level::Info<<"VertexArray::VertexArray(const VertexArray& va) finished "<<rendererID<<op::endl;
}

VertexArray::~VertexArray() {
    if (rendererID != 0) {
        GLCall(glDeleteVertexArrays(1, &rendererID));
    }
    rendererID = 0;
}

void VertexArray::AddBuffer(const VertexBuffer& vb, unsigned int index, unsigned int size, unsigned int type, bool normalized, unsigned int stride, const void* pointer) const {
    Bind();
    vb.Bind();
    GLCall(glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer));
    GLCall(glEnableVertexAttribArray(index));
    Unbind();
}

void VertexArray::SetElementBuffer(const IndexBuffer& ib) const {
    Log<<Level::Info<<"VertexArray::SetElementBuffer() VAO:"<<rendererID<<"EBO:"<<ib.getRendererID()<<op::endl;
    Bind();
    ib.Bind();
    Unbind();
}

void VertexArray::SetVertexBuffer(const VertexBuffer& vb) const {
    Log<<Level::Info<<"VertexArray::SetVertexBuffer() VAO:"<<rendererID<<"VBO:"<<vb.getRendererID()<<op::endl;
    Bind();
    vb.Bind();
    Unbind();
}

VertexArray& VertexArray::operator=(const VertexArray& va) {
    Log<<Level::Info<<"VertexArray& VertexArray::operator=(const VertexArray& va) "<<this->rendererID<<" from "<<va.rendererID<<op::endl;
    if (this != &va) {
        
        // 先删除当前的VAO
        if(rendererID != 0) {
            GLCall(glDeleteVertexArrays(1, &rendererID));
        }
        
        // 生成新的顶点数组对象
        GLCall(glGenVertexArrays(1, &rendererID));
        
        // 保存当前绑定的VAO，以便操作后恢复
        GLint previousVAO;
        GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO));
        
        // 绑定源VAO
        va.Bind();
        
        // 收集源VAO的属性信息
        // 假设最多支持16个顶点属性
        const int maxAttribs = 16;
        for (int i = 0; i < maxAttribs; i++) {
            GLint enabled = 0;
            GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled));
            
            if (enabled) {
                // 获取源属性配置
                GLint size = 0, type = 0, normalized = 0, stride = 0;
                void* pointer = nullptr;
                
                GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size));
                GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type));
                GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized));
                GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride));
                GLCall(glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer));
                
                // 获取当前绑定的VBO
                GLint currentVBO = 0;
                GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &currentVBO));
                
                // 绑定我们的VAO
                Bind();
                
                // 绑定相同的VBO
                GLCall(glBindBuffer(GL_ARRAY_BUFFER, currentVBO));
                
                // 复制属性配置到我们的VAO
                GLCall(glEnableVertexAttribArray(i));
                GLCall(glVertexAttribPointer(
                    i, 
                    size, 
                    type, 
                    normalized == GL_TRUE, 
                    stride, 
                    pointer
                ));
            }
        }
        
        // 检查索引缓冲区(EBO)是否绑定
        va.Bind();
        GLint elementArrayBufferBinding;
        GLCall(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding));
        
        if (elementArrayBufferBinding) {
            // 如果源VAO有EBO绑定，我们也应该绑定相同的EBO到新VAO
            Bind();
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding));
        }
        
        // 恢复原来绑定的VAO
        GLCall(glBindVertexArray(previousVAO));
    }
    Log<<Level::Info<<"VertexArray& VertexArray::operator=(const VertexArray& va) finished "<<this->rendererID<<op::endl;
    return *this;
}