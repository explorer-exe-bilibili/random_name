#include "core/render/Texture.h"

#include "core/log.h"
#include "core/render/GLBase.h"
#include <glm/gtc/matrix_transform.hpp>
#include <mutex>


using namespace core;

bool Texture::inited = false;
std::shared_ptr<Shader> Texture::DefaultShaderProgram = nullptr;
VertexArray* Texture::va = nullptr;
VertexBuffer* Texture::vb = nullptr;
IndexBuffer* Texture::ib = nullptr;

std::mutex initMutex;

static const std::string DefaultVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 transform;

out vec2 TexCoord;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

static const std::string DefaultFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";

const float Texture::vertices[20] = {
    // 位置           // 纹理坐标
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // 左上
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // 右上
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f  // 左下
};

const unsigned int Texture::indices[6] = {
    0, 1, 2,
    2, 3, 0
};

void Texture::init() {
    if (inited) return;
    Log<<Level::Info<<"Texture::init() "<<op::endl;
    inited = true;
    DefaultShaderProgram = std::make_shared<Shader>(DefaultVertexShaderSource, DefaultFragmentShaderSource);
    va = new VertexArray;
    // 创建并绑定 VAO
    va->Bind();
    // 创建 VertexBuffer 并填充数据
    vb = new VertexBuffer(vertices, sizeof(vertices));
    // 创建 IndexBuffer 并填充数据
    ib = new IndexBuffer(indices, 6);
    // 设置顶点属性
    va->AddBuffer(*vb, 0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);                 // 位置属性
    va->AddBuffer(*vb, 1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float))); // 纹理坐标属性
    // 解绑 VAO
    VertexArray::Unbind();
    Log<<Level::Info<<"Texture::init() Success"<<op::endl;
    Log<<Level::Info<<"Texture::init() DefaultShaderProgram "<<unsigned int(*DefaultShaderProgram.get())<<op::endl;
}

Texture::Texture(const unsigned char* data, int width, int height)
    : width(width), height(height), textureID(0) {
    init();
    GLCall(glGenTextures(1, &textureID));
    Log<<Level::Info<<"Texture::Texture(const unsigned char* data, int width, int height) "<<textureID<<op::endl;
    if (data == nullptr) {
        Log<<Level::Error<<"Texture::Texture(const unsigned char* data, int width, int height) data is null"<<op::endl;
        return;
    }
    if (width <= 0 || height <= 0) {
        Log<<Level::Error<<"Texture::Texture(const unsigned char* data, int width, int height) width or height is invalid"<<op::endl;
        return;
    }
    if(textureID == 0) {
        Log<<Level::Error<<"Texture::Texture(const unsigned char* data, int width, int height) textureID is 0"<<op::endl;
        return;
    }
    bind();
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

Texture::Texture(const int width, const int height)
    : width(width), height(height), textureID(0) {
    Log<<Level::Info<<"Texture::Texture(const int width, const int height) "<<width<<" "<<height<<op::endl;
    init();
    GLCall(glGenTextures(1, &textureID));
    if(textureID == 0) {
        Log<<Level::Error<<"Texture::Texture(const int width, const int height) textureID is 0"<<op::endl;
        return;
    }
    bind();
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

Texture::Texture(const Texture& other){
    Log<<Level::Info<<"Texture::Texture(const Texture& other) "<<other.textureID<<op::endl;
    init();
    GLCall(glGenTextures(1, &textureID));
    GLuint fbo;
    GLCall(glGenFramebuffers(1,&fbo));
    /// 绑定FBO
    GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
    /// 绑定纹理到FBO
    GLCall(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, other.textureID, 0));
    /// 绑定目标纹理
    GLCall(glBindTexture(GL_TEXTURE_2D, this->textureID));
    /// 复制FBO的颜色缓冲到目标纹理
    GLCall(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width,height));
    /// 解绑FBO
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    GLCall(glDeleteFramebuffers(1,&fbo));
    this->width = other.width;
    this->height = other.height;
}

Texture::~Texture() {
    if (textureID != 0) {
        GLCall(glDeleteTextures(1, &textureID));
    }
}

void Texture::bind() const {
    if (textureID == 0) {
        Log<<Level::Error<<"Texture::bind() textureID is 0"<<op::endl;
        return;
    }
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
}

void Texture::Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle) const {
    if (textureID==0) {
        Log<<Level::Error<<"Texture::Draw() textureID is 0"<<op::endl;
        return;
    }

    std::shared_ptr<Shader> shader = customerShaderProgram ? customerShaderProgram : DefaultShaderProgram;
    shader->Bind();
    // 计算尺寸和中心点
    glm::vec3 size = bottomRight - topLeft;
    glm::vec3 center = topLeft + size * 0.5f;

    // 创建变换矩阵
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, center);
    if (angle != 0.0f)
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));
    transform = glm::scale(transform, glm::vec3(size.x, size.y, 1.0f));
    // 设置变换矩阵
    shader->setMat4("transform", transform);
    // 绑定纹理
    bind();
    shader->setInt("texture1", 0);

    // 绘制
    if(customerVAO) {
        customerVAO->Bind();
    } else {
        va->Bind();
    }
    if(customerIBO) {
        customerIBO->Bind();
        GLCall(glDrawElements(GL_TRIANGLES, customerIBO->getCount(), GL_UNSIGNED_INT, nullptr));
    } else {
        ib->Bind();
        GLCall(glDrawElements(GL_TRIANGLES, ib->getCount(), GL_UNSIGNED_INT, nullptr));
    }
    // 解绑 VAO 和 IBO
    VertexArray::Unbind();
    IndexBuffer::Unbind();
}

Texture& Texture::operator=(const Texture& other) {
    Log<<Level::Info<<"Texture& Texture::operator=(const Texture& other) from "<<other.textureID<<op::endl;
    if(this != &other) {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
        }
        this->width = other.width;
        this->height = other.height;
        GLCall(glGenTextures(1, &textureID));
        GLuint fbo;
        GLCall(glGenFramebuffers(1,&fbo));
        /// 绑定FBO
        GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
        /// 绑定纹理到FBO
        GLCall(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, other.textureID, 0));
        /// 绑定目标纹理
        GLCall(glBindTexture(GL_TEXTURE_2D, this->textureID));
        /// 复制FBO的颜色缓冲到目标纹理
        GLCall(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width,height));
        /// 解绑FBO
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        GLCall(glDeleteFramebuffers(1,&fbo));
    }
    return *this;
}

bool Texture::setCustomerShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
    if (customerShaderProgram) 
        customerShaderProgram.reset();
    customerShaderProgram = std::make_shared<Shader>(vertexShader, fragmentShader);
    return true;
}

bool Texture::setCustomerShaderProgram(const Shader& shader) {
    if (customerShaderProgram)
        customerShaderProgram.reset();
    customerShaderProgram = std::make_shared<Shader>(shader);
    return true;
}

void Texture::setCustomerVertexArray(const VertexArray& va) {
    if (customerVAO) 
        customerVAO.reset();
    customerVAO = std::make_shared<VertexArray>(va);
}

void Texture::setCustomerVertexBuffer(const VertexBuffer& vb) {
    if (customerVBO) {
        customerVBO.reset();
    }
    customerVBO = std::make_shared<VertexBuffer>(vb);
}

void Texture::setCustomerIndexBuffer(const IndexBuffer& ib) {
    if (customerIBO) {
        customerIBO.reset();
    }
    customerIBO = std::make_shared<IndexBuffer>(ib);
}

void Texture::setCustomerVertices(const std::vector<float>& vertices) {
    customerVertices = vertices;
}

void Texture::setCustomerIndices(const std::vector<unsigned int>& indices) {
    customerIndices = indices;
}