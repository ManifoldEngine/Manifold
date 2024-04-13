#include "OpenGLVertexArray.h"
#include "OpenGLBuffer.h"
#include "GL/glew.h"

using namespace Mani;

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
		default: MANI_ASSERT(false, "Unknown ShaderDataType"); return 0;
	}
}

OpenGLVertexArray::OpenGLVertexArray()
	: m_vertexArrayObjectId(-1)
{
#if MANI_WEBGL
	glGenVertexArrays(1, &m_vertexArrayObjectId);
#else
	glCreateVertexArrays(1, &m_vertexArrayObjectId);
#endif
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &m_vertexArrayObjectId);
}

void OpenGLVertexArray::addVertexBuffer(std::shared_ptr<OpenGLVertexBuffer> buffer)
{
	bind();
	// we purposefully don't null check buffer. if this is null, then we did something terribly wrong and it should not fail silently.
	buffer->bind();

	// using a unsigned long here since this will be casted to void* which is in 64bits.
	// MSVC complains if we cast an int to a type that is larger (see C4312).
	uint64_t accumulatedOffset = 0;
	for (const auto& layoutElement : buffer->layout)
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
					static_cast<GLint>(OpenGLVertexBuffer::getComponentCount(layoutElement.shaderType)), // GLint size,
					toOpenGLType(layoutElement.shaderType), // GLenum type, 
					layoutElement.isNormalized, // GLboolean normalized,
					static_cast<GLsizei>(buffer->getStrideSize()), // GLsizei stride,
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
					static_cast<GLint>(OpenGLVertexBuffer::getComponentCount(layoutElement.shaderType)), // GLint size,
					toOpenGLType(layoutElement.shaderType), // GLenum type, 
					static_cast<GLsizei>(buffer->getStrideSize()), // GLsizei stride,
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
			//			layoutElement.isNormalized, // GLboolean normalized,
			//			buffer->getStrideSize(), // GLsizei stride,
			//			(const void*)(accumulatedOffset + sizeof(float) * i) // const void* pointer
			//		);
			//	}
			//	break;
			//}

			default:
				MANI_ASSERT(false, "Unknown ShaderDataType");
				break;
		}
	}
	m_vertexBuffers.push_back(buffer);
}

void OpenGLVertexArray::setIndexBuffer(std::shared_ptr<OpenGLIndexBuffer> buffer)
{
	bind();
	// we purposefully don't null check buffer. if this is null, then we did something terribly wrong and it should not fail silently.
	buffer->bind();

	m_indexBuffer = buffer;
}

std::shared_ptr<OpenGLIndexBuffer> OpenGLVertexArray::getIndexBuffer() const
{
	return m_indexBuffer;
}

void OpenGLVertexArray::bind() const
{
	glBindVertexArray(m_vertexArrayObjectId);
}
