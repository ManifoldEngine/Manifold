#include "OpenGLVertexArray.h"
#include "OpenGLBuffer.h"
#include "GL/glew.h"

using namespace ECSEngine;

OpenGLVertexArray::OpenGLVertexArray()
	: m_vertexArrayObjectId(-1)
{
	glCreateVertexArrays(1, &m_vertexArrayObjectId);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &m_vertexArrayObjectId);
}

void OpenGLVertexArray::addVertexBuffer(std::shared_ptr<OpenGLBuffer> pBuffer)
{
	bind();
	// we purposefully don't null check pBuffer. if this is null, then we did something terribly wrong and it should not fail silently.
	pBuffer->bind();
	glEnableVertexAttribArray(m_vertexArrayLastIndex);
	glVertexAttribPointer(m_vertexArrayLastIndex, pBuffer->getElementCount(), GL_FLOAT, GL_FALSE, pBuffer->getElementSize(), (void*)0);
	m_vertexArrayLastIndex++;
	m_vertexBuffers.push_back(pBuffer);
}

void OpenGLVertexArray::bind() const
{
	glBindVertexArray(m_vertexArrayObjectId);
}
