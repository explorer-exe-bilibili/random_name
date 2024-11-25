#pragma once
#include <memory>
#include <string>
#include <glm/vec3.hpp>

class Shader;

class Bitmap
{
	bool IsLoad = false;
	unsigned char* data = nullptr;
	int width = 0, height = 0;
	int channels = 0;
	unsigned int texture = 0;
	static unsigned int VBO, VAO, EBO;
	static std::string vertexShaderSource, fragmentShaderSource;
	static std::shared_ptr<Shader> shader;

public:
	Bitmap() = default;
	~Bitmap();
	bool LoadFromFile(const std::string& path);
	void Draw(const glm::vec3& position,float angle=0) const;

	void Draw(const glm::vec3& position, const glm::vec3& end_position, float angle) const;

	static void init();

private:
	// 设置顶点数据和缓冲，并配置顶点属性
	static float vertices[];
	static unsigned int indices[];
};

