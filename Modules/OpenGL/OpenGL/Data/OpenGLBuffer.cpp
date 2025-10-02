#include "OpenGLBuffer.h"
#include <GL/gl3w.h>

using namespace Mani;

// OpenGLVertexBuffer Begin
void OpenGLVertexBuffer::create()
{
#ifdef MANI_WEBGL
	glGenBuffers(1, &m_vertexBufferObjectId);
#else
	glCreateBuffers(1, &m_vertexBufferObjectId);
#endif
}

void Mani::OpenGLVertexBuffer::create(const float* data, size_t size)
{
	create();
	set(data, size);
}

void Mani::OpenGLVertexBuffer::destroy()
{
	if (m_vertexBufferObjectId != UINT32_MAX)
	{
		glDeleteBuffers(1, &m_vertexBufferObjectId);
		m_size = 0;
		m_vertexBufferObjectId = UINT32_MAX;
	}
}

OpenGLVertexBuffer::OpenGLVertexBuffer(OpenGLVertexBuffer&& other) noexcept
{
	m_vertexBufferObjectId = other.m_vertexBufferObjectId;
	layout = other.layout;
	m_size = other.m_size;

	other.m_vertexBufferObjectId = 0;
	other.layout.~List();
	other.m_size = 0;
}

OpenGLVertexBuffer& OpenGLVertexBuffer::operator=(OpenGLVertexBuffer&& other) noexcept
{
	m_vertexBufferObjectId = other.m_vertexBufferObjectId;
	layout = other.layout;
	m_size = other.m_size;

	other.m_vertexBufferObjectId = 0;
	other.layout.~List();
	other.m_size = 0;
	return *this;
}

void OpenGLVertexBuffer::set(const float* data, size_t count)
{
	m_size = count;
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, m_size, data, GL_STATIC_DRAW);
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
void OpenGLIndexBuffer::create()
{
#ifdef MANI_WEBGL
	glGenBuffers(1, &m_indexBufferObjectId);
#else
	glCreateBuffers(1, &m_indexBufferObjectId);
#endif
}

void OpenGLIndexBuffer::create(const unsigned int* indices, size_t size)
{
	create();
	set(indices, size);
}

void OpenGLIndexBuffer::destroy()
{
	if (m_indexBufferObjectId != UINT32_MAX)
	{
		glDeleteBuffers(1, &m_indexBufferObjectId);
		m_size = 0;
		m_indexBufferObjectId = UINT32_MAX;
	}
}

OpenGLIndexBuffer::OpenGLIndexBuffer(OpenGLIndexBuffer&& other) noexcept
{
	m_size = other.m_size;
	m_indexBufferObjectId = other.m_indexBufferObjectId;

	other.m_size = 0;
	other.m_indexBufferObjectId = 0;
}

OpenGLIndexBuffer& OpenGLIndexBuffer::operator=(OpenGLIndexBuffer&& other) noexcept
{
	m_size = other.m_size;
	m_indexBufferObjectId = other.m_indexBufferObjectId;

	other.m_size = 0;
	other.m_indexBufferObjectId = 0;
	return *this;
}

void OpenGLIndexBuffer::set(const unsigned int* indices, size_t size)
{
	m_size = size;
	glBindBuffer(GL_ARRAY_BUFFER, m_indexBufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
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
