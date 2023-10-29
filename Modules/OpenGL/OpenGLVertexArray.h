#pragma once

#include "OpenGL.h"
#include <memory>
#include <vector>

namespace ECSEngine
{
	class OpenGLBuffer;

	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		// can't implicetely copy a VAO.
		OpenGLVertexArray(const OpenGLVertexArray&) = delete;

		void addVertexBuffer(std::shared_ptr<OpenGLBuffer> pBuffer);
		void bind() const;
	private:
		unsigned int m_vertexArrayObjectId;
		int m_vertexArrayLastIndex = 0;
		std::vector<std::shared_ptr<OpenGLBuffer>> m_vertexBuffers;
	};
}