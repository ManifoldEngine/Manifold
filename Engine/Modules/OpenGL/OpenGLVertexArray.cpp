#include "OpenGLVertexArray.h"
#include "OpenGLBuffer.h"
#include "GL/glew.h"

using namespace ECSEngine;

GLenum toOpenGLType(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
	}

	ECSE_ASSERT(false, "Unknown ShaderDataType");
	return 0;
}

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

	// using a unsigned long here since this will be casted to void* which is in 64bits.
	// MSVC complains if we cast an int to a type that is larger (see C4312).
	uint64_t accumulatedOffset = 0;
	for (const auto& layoutElement : pBuffer->layout)
	{
		switch (layoutElement.shaderType)
		{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(m_attributeCount);
				glVertexAttribPointer(
					m_attributeCount, // GLuint index, 
					OpenGLVertexBuffer::getComponentCount(layoutElement.shaderType), // GLint size,
					toOpenGLType(layoutElement.shaderType), // GLenum type, 
					layoutElement.bIsNormalized, // GLboolean normalized,
					pBuffer->getStrideSize(), // GLsizei stride,
					(void*)accumulatedOffset // const void* pointer
				);
				accumulatedOffset += OpenGLVertexBuffer::getShaderDataTypeSize(layoutElement.shaderType);
				m_attributeCount++;
				break;
			}

			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(m_attributeCount);
				glVertexAttribIPointer(
					m_attributeCount, // GLuint index, 
					OpenGLVertexBuffer::getComponentCount(layoutElement.shaderType), // GLint size,
					toOpenGLType(layoutElement.shaderType), // GLenum type, 
					pBuffer->getStrideSize(), // GLsizei stride,
					(void*)accumulatedOffset // const void* pointer
				);
				accumulatedOffset += OpenGLVertexBuffer::getShaderDataTypeSize(layoutElement.shaderType);
				m_attributeCount++;
				break;
			}

			// todo: understand wtf is glVertexAttribDivisor.
			//case ShaderDataType::Mat3:
			//case ShaderDataType::Mat4:
			//{
			//	const int componentCount = OpenGLVertexBuffer::getComponentCount(layoutElement.shaderType);
			//	for (int i = 0; i < componentCount; ++i)
			//	{
			//		glEnableVertexAttribArray(m_attributeCount);
			//		glVertexAttribPointer(
			//			m_attributeCount, // GLuint index, 
			//			componentCount, // GLint size,
			//			toOpenGLType(layoutElement.shaderType), // GLenum type, 
			//			layoutElement.bIsNormalized, // GLboolean normalized,
			//			pBuffer->getStrideSize(), // GLsizei stride,
			//			(const void*)(accumulatedOffset + sizeof(float) * i) // const void* pointer
			//		);
			//	}
			//	break;
			//}

			default:
				ECSE_ASSERT(false, "Unknown ShaderDataType");
				break;
		}
	}
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
