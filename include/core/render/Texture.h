#pragma once

#include <glad/glad.h>
#include<glm/glm.hpp>

#include<string>
#include<memory>
#include<vector>

#include"Shader.h"
#include"VertexArray.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"



namespace core {
    class Texture {
    public:
        Texture(const unsigned char* data, const int width, const int height);
        Texture(const int width,const int height);
        Texture(const Texture& texture);
        Texture(){init();}
        ~Texture();

        void bind() const;
        void Bind() const {bind();}
        static void unbind() {
            GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        }

        int getWidth() const {return width;}
        int getHeight() const {return height;}
        unsigned int getTextureID() const {return textureID;}

        /*绘制纹理
        * @param topLeft 左上角坐标
        * @param bottomRight 右下角坐标
        * @param angle 旋转角度
        * */
        void Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle = 0.0f) const;


        operator bool() const {return textureID != 0;}
        Texture& operator=(const Texture& other);

        bool setCustomerShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
        bool setCustomerShaderProgram(const Shader& shader);
        void setCustomerVertexArray(const VertexArray& va);
        void setCustomerVertexBuffer(const VertexBuffer& vb);
        void setCustomerIndexBuffer(const IndexBuffer& ib);
        void setCustomerVertices(const std::vector<float>& vertices);
        void setCustomerIndices(const std::vector<unsigned int>& indices);
    private:
        void init();
        static bool inited;
        
        unsigned int textureID=0;
        int width=0, height=0;

        std::string vertexShaderSource="";
        std::string fragmentShaderSource="";
        static std::shared_ptr<Shader> DefaultShaderProgram;
        static VertexArray* va;
        static VertexBuffer* vb;
        static IndexBuffer* ib;
        static const float vertices[20];
        static const unsigned int indices[6];
        std::shared_ptr<Shader> customerShaderProgram=nullptr;
        std::shared_ptr<VertexArray> customerVAO=nullptr;
        std::shared_ptr<VertexBuffer> customerVBO=nullptr;
        std::shared_ptr<IndexBuffer> customerIBO=nullptr;
        std::vector<float> customerVertices;
        std::vector<unsigned int> customerIndices;
    };
}