//#include "Bitmap.h"
//
//#include <glad/glad.h>
//#include <iostream>
//#include <GLFW/glfw3.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//#include <glm/ext/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "OpenGLBase.h"
//#include "Shader.h"
//
//std::string Bitmap::vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;
//
//uniform mat4 transform; // 添加变换矩阵
//
//out vec2 TexCoord;
//
//void main()
//{
//    gl_Position = transform * vec4(aPos, 1.0); // 应用变换
//    TexCoord = aTexCoord;
//}
//)";
//
//std::string Bitmap::fragmentShaderSource = R"(
//#version 330 core
//out vec4 FragColor;
//
//in vec2 TexCoord;
//
//// 纹理采样器
//uniform sampler2D texture1;
//
//void main()
//{
//    FragColor = texture(texture1, TexCoord);
//}
//)";
//
//std::shared_ptr<Shader> Bitmap::shader = nullptr;
//
//float Bitmap::vertices[] = {
//	// 位置          // 纹理坐标
//	0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 右上角
//	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 右下角
//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 左下角
//	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // 左上角
//};
//
//unsigned int Bitmap::indices[] = {
//	0, 1, 3, // 第一个三角形
//	1, 2, 3  // 第二个三角形
//};
//
//Bitmap::~Bitmap()
//{
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);
//}
//
//bool Bitmap::LoadFromFile(const std::string& path)
//{
//	//// 保存当前的着色器程序和 VAO 等状态
//	//GLint previousProgram;
//	//GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram));
//	//GLint previousVAO;
//	//GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO));
//	//GLint previousVBO;
//	//GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BUFFER_BINDING, &previousVBO));
//	////GLint previousEBO;
//	////glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &previousEBO);
//
//	//GLCall(glGenVertexArrays(1, &VAO));
//	//GLCall(glGenBuffers(1, &VBO));
//	//GLCall(glGenBuffers(1, &EBO));
//
//	//GLCall(glBindVertexArray(VAO));
//
//	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
//	//GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
//
//	//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
//	//GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
//
//	//// 位置属性
//	//GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
//	//GLCall(glEnableVertexAttribArray(0));
//	//// 纹理坐标属性
//	//GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
//	//GLCall(glEnableVertexAttribArray(1));
//
//	//// 加载并创建纹理
//	//GLCall(glGenTextures(1, &texture));
//	//GLCall(glBindTexture(GL_TEXTURE_2D, texture));
//	//// 设置纹理参数
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
//	//// 加载图片
//	//int width, height, nrChannels;
//	//unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
//	//if (data) {
//	//	GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
//	//	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
//	//	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
//	//}
//	//else {
//	//	std::cerr << "Failed to load texture" << std::endl;
//	//}
//	//stbi_image_free(data);
//	//IsLoad = true;
//	//// 恢复之前的状态
//	//GLCall(glUseProgram(previousProgram));
//	//GLCall(glBindVertexArray(previousVAO));
//	//GLCall(glBindVertexArray(previousVBO));
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previousEBO);
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	// 位置属性
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	// 纹理坐标属性
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//	// 加载并创建纹理
//	GLuint texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	// 设置纹理参数
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// 加载图片
//	int width, height, nrChannels;
//	stbi_set_flip_vertically_on_load(true); // 翻转图片
//	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
//	if (data) {
//		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else {
//		std::cerr << "Failed to load texture" << std::endl;
//	}
//	stbi_image_free(data);
//	return true;
//}
//
//void Bitmap::Draw(const glm::vec3& position, float angle) const
//{
//	//GLint UsingShader;
//	//GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &UsingShader));
//	//GLint BindingVao;
//	//GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &BindingVao));
//	//if (!shader)return;
//	//shader->use();
//	//shader->setInt("texture1", 0);
//
//	//// 创建平移和旋转矩阵
//	//glm::mat4 transform = glm::mat4(1.0f); // 初始化为单位矩阵
//	//transform = glm::translate(transform, position);
//	//transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f)); // 绕Z轴旋转
//
//	//// 传递矩阵到着色器
//	////glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
//	//shader->setMat4("transform", transform);
//
//	//// 绑定纹理
//	//GLCall(glActiveTexture(GL_TEXTURE0));
//	//GLCall(glBindTexture(GL_TEXTURE_2D, texture));
//
//	//// 绘制矩形
//	//GLCall(glBindVertexArray(VAO));
//	//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
//
//	//// 恢复之前的状态
//	//GLCall(glUseProgram(UsingShader));
//	//GLCall(glBindVertexArray(BindingVao));
//			// 创建平移矩阵
//	shader->use();
//	shader->setInt("texture1", 0);
//	glm::mat4 transform = glm::mat4(1.0f); // 初始化为单位矩阵
//	transform = glm::translate(transform,position);
//
//	shader->setMat4("transform",transform);
//
//
//	// 传递矩阵到着色器
//	//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
//
//	// 绑定纹理
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	// 绘制矩形
//	glBindVertexArray(VAO);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//}
//
//void Bitmap::init()
//{
//	if (shader)return;
//	//// 初始化GLAD
//	//if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
//	//{
//	//	std::cout << "Failed to initialize GLAD" << std::endl;
//	//}
//	stbi_set_flip_vertically_on_load(true); // 翻转图片
//	shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);
//}


// Bitmap.cpp
#define STB_IMAGE_IMPLEMENTATION

#include "Bitmap.h"
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Shader.h"

// 静态成员变量定义
std::string Bitmap::vertexShaderSource = R"(
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

std::string Bitmap::fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";

std::shared_ptr<Shader> Bitmap::shader = nullptr;
float Bitmap::vertices[20] = {
    // 位置           // 纹理坐标
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // 右上
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // 左下
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // 左上
};

unsigned int Bitmap::indices[6] = {
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
};

unsigned int Bitmap::VAO = 0;
unsigned int Bitmap::VBO = 0;
unsigned int Bitmap::EBO = 0;

Bitmap::~Bitmap()
{
    if (data)
        stbi_image_free(data);
    glDeleteTextures(1, &texture);
    //if (VBO && VAO && EBO) {
    //    glDeleteVertexArrays(1, &VAO);
    //    glDeleteBuffers(1, &VBO);
    //    glDeleteBuffers(1, &EBO);
    //}
}

void Bitmap::init()
{
    // 初始化着色器
    shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);

    // 生成和绑定 VAO、VBO、EBO
    glGenVertexArrays(1, &Bitmap::VAO);
    glGenBuffers(1, &Bitmap::VBO);
    glGenBuffers(1, &Bitmap::EBO);

    glBindVertexArray(Bitmap::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Bitmap::VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Bitmap::EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

bool Bitmap::LoadFromFile(const std::string& path)
{
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载纹理
    GLenum format;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    else {
        std::cerr << "Unknown number of channels: " << channels << std::endl;
        stbi_image_free(data);
        data = nullptr;
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    data = nullptr;

    IsLoad = true;
    return true;
}

void Bitmap::Draw(const glm::vec3& position, float angle) const
{
    if (!IsLoad)
        return;

    // 使用着色器
    shader->use();

    // 创建变换矩阵
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    if (angle != 0.0f)
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));
    // 设置变换矩阵
    shader->setMat4("transform", transform);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("texture1", 0);

    // 绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Bitmap::Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle) const
{
    if (!IsLoad)
        return;

    // 使用着色器
    shader->use();

    // 计算尺寸（宽度和高度）
    glm::vec3 size = bottomRight - topLeft;
    // 确保尺寸为正值
    glm::vec3 absSize = glm::abs(size);

    // 计算中心点
    glm::vec3 center = topLeft + size * 0.5f;

    // 创建变换矩阵
    glm::mat4 transform = glm::mat4(1.0f);

    // 平移到中心位置
    transform = glm::translate(transform, center);

    // 旋转（如果需要）
    if (angle != 0.0f)
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));

    // 缩放到指定大小
    transform = glm::scale(transform, glm::vec3(absSize.x, absSize.y, 1.0f));

    // 设置变换矩阵
    shader->setMat4("transform", transform);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("texture1", 0);

    // 绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}