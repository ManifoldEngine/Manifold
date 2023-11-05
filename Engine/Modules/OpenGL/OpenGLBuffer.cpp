#include "OpenGLBuffer.h"
#include <GL/glew.h>

using namespace ECSEngine;

// OpenGLVertexBuffer Begin
OpenGLVertexBuffer::OpenGLVertexBuffer(float* data, int size)
	: m_size(size), m_vertexBufferObjectId(UINT32_MAX)
{
	glCreateBuffers(1, &m_vertexBufferObjectId);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, m_size, data, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_vertexBufferObjectId);
}

void OpenGLVertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectId);
}

int OpenGLVertexBuffer::getStrideCount() const
{
	const int strideSize = getStrideSize();
	if (strideSize > 0)
	{
		return m_size / strideSize;
	}

	return 0;
}

int OpenGLVertexBuffer::getStrideSize() const
{
	int size = 0;
	for (const auto& layoutElement : layout)
	{
		size += getShaderDataTypeSize(layoutElement.shaderType);
	}
	
	return size;
}

int OpenGLVertexBuffer::getShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:    return sizeof(float);
		case ShaderDataType::Float2:   return sizeof(float) * 2;
		case ShaderDataType::Float3:   return sizeof(float) * 3;
		case ShaderDataType::Float4:   return sizeof(float) * 4;
		case ShaderDataType::Mat3:     return sizeof(float) * 3 * 3;
		case ShaderDataType::Mat4:     return sizeof(float) * 4 * 4;
		case ShaderDataType::Int:      return sizeof(int);
		case ShaderDataType::Int2:     return sizeof(int) * 2;
		case ShaderDataType::Int3:     return sizeof(int) * 3;
		case ShaderDataType::Int4:     return sizeof(int) * 4;
		case ShaderDataType::Bool:     return sizeof(bool);
	}

	ECSE_ASSERT(false, "Unknown ShaderDataType");
	return 0;
}

int OpenGLVertexBuffer::getComponentCount(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:    return 1;
		case ShaderDataType::Float2:   return 2;
		case ShaderDataType::Float3:   return 3;
		case ShaderDataType::Float4:   return 4;
		case ShaderDataType::Mat3:     return 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4;
		case ShaderDataType::Int:      return 1;
		case ShaderDataType::Int2:     return 2;
		case ShaderDataType::Int3:     return 3;
		case ShaderDataType::Int4:     return 4;
		case ShaderDataType::Bool:     return 1;
	}

	ECSE_ASSERT(false, "Unknown ShaderDataType");
	return 0;
}
// OpenGLVertexBuffer End

// OpenGLIndexBuffer Begin
OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, int size)
	: m_size(size), m_indexBufferObjectId(UINT32_MAX)
{
	glCreateBuffers(1, &m_indexBufferObjectId);

	glBindBuffer(GL_ARRAY_BUFFER, m_indexBufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}
OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &m_indexBufferObjectId);
}

void OpenGLIndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObjectId);
}

int OpenGLIndexBuffer::getStrideCount() const
{
	return m_size / sizeof(int);
}

int OpenGLIndexBuffer::getStrideSize() const
{
	return sizeof(int);
}
// OpenGLIndexBuffer End
