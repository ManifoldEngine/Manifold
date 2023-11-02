#include "OpenGLBuffer.h"
#include <GL/glew.h>

using namespace ECSEngine;

// OpenGLVertexBuffer Begin
OpenGLVertexBuffer::OpenGLVertexBuffer(float* pData, int size)
	: m_size(size)
{
	glCreateBuffers(1, &m_vertexBufferObjectId);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, m_size, pData, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_vertexBufferObjectId);
}

void OpenGLVertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectId);
}

int ECSEngine::OpenGLVertexBuffer::getComponentCount() const
{
	// for now hardcoded to vec3
	return m_componentCount;
}

int OpenGLVertexBuffer::getStrideCount() const
{
	// for now hardcoded to vec3
	return m_size / (m_componentCount * sizeof(float));
}

int OpenGLVertexBuffer::getStrideSize() const
{
	// for now hardcoded to vec3
	return m_componentCount * sizeof(float);
}
// OpenGLVertexBuffer End

// OpenGLIndexBuffer Begin
OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, int size)
	: m_size(size)
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
