#pragma once

#include "OpenGL.h"

namespace ECSEngine
{
	class OpenGLVertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* pData, int count);
		~OpenGLVertexBuffer();
		
		// buffers can't be copied implicitely.
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;

		void bind() const;
		int getComponentCount() const;
		int getStrideCount() const;
		int getStrideSize() const;
		
	private:
		// for now hardcoded to vec3
		const int m_componentCount = 3;

		int m_size;
		unsigned int m_vertexBufferObjectId;
	};

	class OpenGLIndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned int* indices, int size);
		~OpenGLIndexBuffer();

		// buffers can't be copied implicitely.
		OpenGLIndexBuffer(const OpenGLVertexBuffer&) = delete;

		void bind() const;
		int getStrideCount() const;
		int getStrideSize() const;

	private:
		int m_size;
		unsigned int m_indexBufferObjectId;
	};
}