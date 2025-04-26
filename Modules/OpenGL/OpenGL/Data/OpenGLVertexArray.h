#pragma once

#include "OpenGL.h"
#include <OpenGL/Data/OpenGLBuffer.h>
#include <vector>

namespace Mani
{
	class OpenGLVertexBuffer;
	class OpenGLIndexBuffer;

	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		// can't implicetely copy a VAO.
		OpenGLVertexArray(const OpenGLVertexArray&) = delete;

		void bind() const;

		void addVertexBuffer(OpenGLVertexBuffer&& buffer);
		void setIndexBuffer(OpenGLIndexBuffer&& buffer);
	
		const OpenGLIndexBuffer& getIndexBuffer() const;
	private:
		unsigned int m_vertexArrayObjectId;
		int m_attributeCount = 0;

		std::vector<OpenGLVertexBuffer> m_vertexBuffers;
		OpenGLIndexBuffer m_indexBuffer;
	};
}