#include "VertexBuffer.h"
//#include <GL/glew.h>
#include <glad/glad.h>
#include <cstring>
#include <stdexcept>

VertexBuffer::VertexBuffer()
	: rendererID(0), size(0),data(nullptr)
{
	this->data = nullptr;
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
    : size(size)
{
    glGenBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    this->data = new char[size];
    std::memcpy(this->data, data, size);
}

VertexBuffer::VertexBuffer(const VertexBuffer& vb)
    : size(vb.size)
{
    glGenBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vb.data, GL_STATIC_DRAW);

    this->data = new char[size];
    std::memcpy(this->data, vb.data, size);
}

VertexBuffer::~VertexBuffer()
{
    delete[] data;
	data = nullptr;
	glDeleteBuffers(1, &rendererID);
    size = 0;
	rendererID = 0;
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vb)
{
	if (this == &vb)
		return *this;
	delete[] data;
	size = vb.size;
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, rendererID);
	glBufferData(GL_ARRAY_BUFFER, size, vb.data, GL_STATIC_DRAW);
	this->data = new char[size];
	std::memcpy(this->data, vb.data, size);
	return *this;
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::enableIndexBuffer(unsigned int index, unsigned int size, unsigned int type, unsigned int stride,
	unsigned int offset)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, GL_FALSE, stride, (const void*)offset);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
}

void VertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IndexBuffer::IndexBuffer() :rendererID(0),data(nullptr),count(0){}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
{
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	this->data = new unsigned int[count];
	std::memcpy(this->data, data, count * sizeof(unsigned int));
	this->count = count;
}

IndexBuffer::IndexBuffer(const IndexBuffer& ib)
{
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib.count * sizeof(unsigned int), ib.data, GL_STATIC_DRAW);
	this->VBs = ib.VBs;
	this->data = new unsigned int[ib.count];
	std::memcpy(this->data, ib.data, ib.count * sizeof(unsigned int));
	this->count = ib.count;
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &rendererID);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);

	try { VBs.at(using_name).Bind(); }
	catch (std::out_of_range) { VBs.begin()->second.Bind(); }
}

void IndexBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	VertexBuffer::Unbind();
}

void IndexBuffer::BindVertexBuffer(std::string name, VertexBuffer& vb)
{
	VBs.insert(std::pair(name, vb));
}

void IndexBuffer::unBindVertexBuffer(std::string name)
{
	VBs.erase(name);
}

void IndexBuffer::useVertexBuffer(std::string name)
{
	using_name = name;
	VBs[using_name].Bind();
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib)
{
	if (this == &ib)
		return *this;
	delete[] data;
	glGenBuffers(1, &rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib.count * sizeof(unsigned int), ib.data, GL_STATIC_DRAW);
	this->data = new unsigned int[ib.count];
	std::memcpy(this->data, ib.data, ib.count * sizeof(unsigned int));
	this->count = ib.count;
	this->VBs = ib.VBs;
	return *this;
}

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &rendererID);
	glBindVertexArray(rendererID);
}

VertexArray::VertexArray(const VertexArray& va)
{
	glGenVertexArrays(1, &rendererID);
	bindIBs = va.bindIBs;
	bindVBs = va.bindVBs;
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &rendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const IndexBuffer& ib) const
{
	GLint BindingID;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &BindingID);
	glBindVertexArray(rendererID);
	vb.Bind();
	ib.Bind();
	VertexBuffer::enableIndexBuffer(0, 2, GL_FLOAT, 2 * sizeof(float), 0);
	glBindVertexArray(BindingID);
}

void VertexArray::Bind() const
{
	glBindVertexArray(rendererID);
	for (auto& vb : bindVBs)
		vb.Bind();
	for (auto& ib : bindIBs)
		ib.Bind();
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
	VertexBuffer::Unbind();
	IndexBuffer::Unbind();
}

VertexArray& VertexArray::operator=(const VertexArray& va)
{
	if (this == &va)
		return *this;
	glDeleteVertexArrays(1, &rendererID);
	glGenVertexArrays(1, &rendererID);
	bindIBs = va.bindIBs;
	bindVBs = va.bindVBs;
	return *this;
}


