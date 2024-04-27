#include "OpenGLBuffer.h"
#include <GL/gl3w.h>

using namespace Mani;

// OpenGLVertexBuffer Begin
OpenGLVertexBuffer::OpenGLVertexBuffer(float* data, size_t size)
	: m_size(size), m_vertexBufferObjectId(UINT32_MAX)
{
#ifdef MANI_WEBGL
	glGenBuffers(1, &m_vertexBufferObjectId);
#else
	glCreateBuffers(1, &m_vertexBufferObjectId);
#endif

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

size_t OpenGLVertexBuffer::getStrideCount() const
{
	const size_t strideSize = getStrideSize();
	if (strideSize > 0)
	{
		return m_size / strideSize;
	}

	return 0;
}

size_t OpenGLVertexBuffer::getStrideSize() const
{
	size_t size = 0;
	for (const auto& layoutElement : layout)
	{
		size += getShaderDataTypeSize(layoutElement.shaderType);
	}
	
	return size;
}

size_t OpenGLVertexBuffer::getShaderDataTypeSize(EShaderDataType type)
{
	switch (type)
	{
		case EShaderDataType::Float:    return sizeof(float);
		case EShaderDataType::Float2:   return sizeof(float) * 2;
		case EShaderDataType::Float3:   return sizeof(float) * 3;
		case EShaderDataType::Float4:   return sizeof(float) * 4;
		case EShaderDataType::Mat3:     return sizeof(float) * 3 * 3;
		case EShaderDataType::Mat4:     return sizeof(float) * 4 * 4;
		case EShaderDataType::Int:      return sizeof(int);
		case EShaderDataType::Int2:     return sizeof(int) * 2;
		case EShaderDataType::Int3:     return sizeof(int) * 3;
		case EShaderDataType::Int4:     return sizeof(int) * 4;
		case EShaderDataType::Bool:     return sizeof(bool);
		default: MANI_ASSERT(false, "Unknown EShaderDataType");  return 0;
	}
}

size_t OpenGLVertexBuffer::getComponentCount(EShaderDataType type)
{
	switch (type)
	{
		case EShaderDataType::Float:    return 1;
		case EShaderDataType::Float2:   return 2;
		case EShaderDataType::Float3:   return 3;
		case EShaderDataType::Float4:   return 4;
		case EShaderDataType::Mat3:     return 3 * 3;
		case EShaderDataType::Mat4:     return 4 * 4;
		case EShaderDataType::Int:      return 1;
		case EShaderDataType::Int2:     return 2;
		case EShaderDataType::Int3:     return 3;
		case EShaderDataType::Int4:     return 4;
		case EShaderDataType::Bool:     return 1;
		default: MANI_ASSERT(false, "Unknown EShaderDataType");  return 0;
	}
}
// OpenGLVertexBuffer End

// OpenGLIndexBuffer Begin
OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, size_t size)
	: m_size(size), m_indexBufferObjectId(UINT32_MAX)
{
#ifdef MANI_WEBGL
	glGenBuffers(1, &m_indexBufferObjectId);
#else
	glCreateBuffers(1, &m_indexBufferObjectId);
#endif

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

size_t OpenGLIndexBuffer::getStrideCount() const
{
	return m_size / sizeof(int);
}

size_t OpenGLIndexBuffer::getStrideSize() const
{
	return sizeof(int);
}
// OpenGLIndexBuffer End
