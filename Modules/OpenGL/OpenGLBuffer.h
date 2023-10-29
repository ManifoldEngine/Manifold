#pragma once

#include "OpenGL.h"

namespace ECSEngine
{
	class OpenGLBuffer
	{
	public:
		OpenGLBuffer(const float* pData, int count);
		~OpenGLBuffer();
		
		// buffers can't be copied implicitely.
		OpenGLBuffer(const OpenGLBuffer&) = delete;

		void bind() const;
		int getSize() const;
		int getElementCount() const;
		int getElementSize() const;
		
	private:
		int m_count;
		unsigned int m_bufferObjectId;
	};
}