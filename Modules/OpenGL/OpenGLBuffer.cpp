#include "OpenGLBuffer.h"
#include <GL/glew.h>

using namespace ECSEngine;

OpenGLBuffer::OpenGLBuffer(const float* pData, int inCount)
	: m_count(inCount)
{
	glCreateBuffers(1, &m_bufferObjectId);

	bind();
	glBufferData(GL_ARRAY_BUFFER, m_count * sizeof(float), pData, GL_STATIC_DRAW);
}

OpenGLBuffer::~OpenGLBuffer()
{
	glDeleteBuffers(1, &m_bufferObjectId);
}

void OpenGLBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjectId);
}

int OpenGLBuffer::getSize() const
{
	return m_count * sizeof(float);
}

int OpenGLBuffer::getElementCount() const
{
	// for now hardcoded to vec3
	return 3;
}

int OpenGLBuffer::getElementSize() const
{
	return getElementCount() * sizeof(float);
}
