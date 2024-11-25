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
//uniform mat4 transform; // ��ӱ任����
//
//out vec2 TexCoord;
//
//void main()
//{
//    gl_Position = transform * vec4(aPos, 1.0); // Ӧ�ñ任
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
//// ���������
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
//	// λ��          // ��������
//	0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // ���Ͻ�
//	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // ���½�
//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // ���½�
//	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // ���Ͻ�
//};
//
//unsigned int Bitmap::indices[] = {
//	0, 1, 3, // ��һ��������
//	1, 2, 3  // �ڶ���������
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
//	//// ���浱ǰ����ɫ������� VAO ��״̬
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
//	//// λ������
//	//GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
//	//GLCall(glEnableVertexAttribArray(0));
//	//// ������������
//	//GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
//	//GLCall(glEnableVertexAttribArray(1));
//
//	//// ���ز���������
//	//GLCall(glGenTextures(1, &texture));
//	//GLCall(glBindTexture(GL_TEXTURE_2D, texture));
//	//// �����������
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
//	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
//	//// ����ͼƬ
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
//	//// �ָ�֮ǰ��״̬
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
//	// λ������
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	// ������������
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//	// ���ز���������
//	GLuint texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	// �����������
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// ����ͼƬ
//	int width, height, nrChannels;
//	stbi_set_flip_vertically_on_load(true); // ��תͼƬ
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
//	//// ����ƽ�ƺ���ת����
//	//glm::mat4 transform = glm::mat4(1.0f); // ��ʼ��Ϊ��λ����
//	//transform = glm::translate(transform, position);
//	//transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f)); // ��Z����ת
//
//	//// ���ݾ�����ɫ��
//	////glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
//	//shader->setMat4("transform", transform);
//
//	//// ������
//	//GLCall(glActiveTexture(GL_TEXTURE0));
//	//GLCall(glBindTexture(GL_TEXTURE_2D, texture));
//
//	//// ���ƾ���
//	//GLCall(glBindVertexArray(VAO));
//	//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
//
//	//// �ָ�֮ǰ��״̬
//	//GLCall(glUseProgram(UsingShader));
//	//GLCall(glBindVertexArray(BindingVao));
//			// ����ƽ�ƾ���
//	shader->use();
//	shader->setInt("texture1", 0);
//	glm::mat4 transform = glm::mat4(1.0f); // ��ʼ��Ϊ��λ����
//	transform = glm::translate(transform,position);
//
//	shader->setMat4("transform",transform);
//
//
//	// ���ݾ�����ɫ��
//	//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
//
//	// ������
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	// ���ƾ���
//	glBindVertexArray(VAO);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//}
//
//void Bitmap::init()
//{
//	if (shader)return;
//	//// ��ʼ��GLAD
//	//if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
//	//{
//	//	std::cout << "Failed to initialize GLAD" << std::endl;
//	//}
//	stbi_set_flip_vertically_on_load(true); // ��תͼƬ
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

// ��̬��Ա��������
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
    // λ��           // ��������
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // ����
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // ����
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // ����
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // ����
};

unsigned int Bitmap::indices[6] = {
    0, 1, 3, // ��һ��������
    1, 2, 3  // �ڶ���������
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
    // ��ʼ����ɫ��
    shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);

    // ���ɺͰ� VAO��VBO��EBO
    glGenVertexArrays(1, &Bitmap::VAO);
    glGenBuffers(1, &Bitmap::VBO);
    glGenBuffers(1, &Bitmap::EBO);

    glBindVertexArray(Bitmap::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Bitmap::VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Bitmap::EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // ������������
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

    // �����������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ��������
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

    // ʹ����ɫ��
    shader->use();

    // �����任����
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    if (angle != 0.0f)
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));
    // ���ñ任����
    shader->setMat4("transform", transform);

    // ������
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("texture1", 0);

    // ����
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Bitmap::Draw(const glm::vec3& topLeft, const glm::vec3& bottomRight, float angle) const
{
    if (!IsLoad)
        return;

    // ʹ����ɫ��
    shader->use();

    // ����ߴ磨��Ⱥ͸߶ȣ�
    glm::vec3 size = bottomRight - topLeft;
    // ȷ���ߴ�Ϊ��ֵ
    glm::vec3 absSize = glm::abs(size);

    // �������ĵ�
    glm::vec3 center = topLeft + size * 0.5f;

    // �����任����
    glm::mat4 transform = glm::mat4(1.0f);

    // ƽ�Ƶ�����λ��
    transform = glm::translate(transform, center);

    // ��ת�������Ҫ��
    if (angle != 0.0f)
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));

    // ���ŵ�ָ����С
    transform = glm::scale(transform, glm::vec3(absSize.x, absSize.y, 1.0f));

    // ���ñ任����
    shader->setMat4("transform", transform);

    // ������
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("texture1", 0);

    // ����
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}