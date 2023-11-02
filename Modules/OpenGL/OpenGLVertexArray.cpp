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

void OpenGLVertexArray::addVertexBuffer(std::shared_ptr<OpenGLVertexBuffer> pBuffer)
{
	bind();
	// we purposefully don't null check pBuffer. if this is null, then we did something terribly wrong and it should not fail silently.
	pBuffer->bind();
	glEnableVertexAttribArray(m_attributeCount);
	glVertexAttribPointer(m_attributeCount, pBuffer->getComponentCount(), GL_FLOAT, GL_FALSE, pBuffer->getStrideSize(), (void*)0);
	m_attributeCount++;
	m_vertexBuffers.push_back(pBuffer);
}

void OpenGLVertexArray::setIndexBuffer(std::shared_ptr<OpenGLIndexBuffer> pBuffer)
{
	bind();
	// we purposefully don't null check pBuffer. if this is null, then we did something terribly wrong and it should not fail silently.
	pBuffer->bind();

	m_pIndexBuffer = pBuffer;
}

std::shared_ptr<OpenGLIndexBuffer> ECSEngine::OpenGLVertexArray::getIndexBuffer() const
{
	return m_pIndexBuffer;
}

void OpenGLVertexArray::bind() const
{
	glBindVertexArray(m_vertexArrayObjectId);
}
