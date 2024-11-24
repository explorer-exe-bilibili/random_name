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
	unsigned int VBO = 0, VAO = 0, EBO = 0;
	static std::string vertexShaderSource, fragmentShaderSource;
	static std::shared_ptr<Shader> shader;

public:
	Bitmap() = default;
	~Bitmap();
	bool LoadFromFile(const std::string& path);
	void Draw(const glm::vec3& position,float angle=0) const;

	static void init();

private:
	// 设置顶点数据和缓冲，并配置顶点属性
	static float vertices[];
	static unsigned int indices[];
};

