#pragma once

#include "OpenGL.h"
#include <Core/Containers/List.h>
#include <OpenGL/Resources/OpenGLBuffer.h>

namespace Mani
{
	class OpenGLVertexBuffer;
	class OpenGLIndexBuffer;

	class OpenGLVertexArray
	{
	public:
		//OpenGLVertexArray() = default;

		//OpenGLVertexArray(OpenGLVertexArray&& other) noexcept;
		//OpenGLVertexArray& operator=(OpenGLVertexArray&& other) noexcept;

		void create();
		void destroy();
		void bind() const;

		void addVertexBuffer(OpenGLVertexBuffer&& buffer);
		void setIndexBuffer(OpenGLIndexBuffer&& buffer);
	
		const OpenGLIndexBuffer& getIndexBuffer() const;
	private:
		unsigned int m_vertexArrayObjectId = UINT32_MAX;
		int m_attributeCount = 0;

		List<OpenGLVertexBuffer> m_vertexBuffers;
		OpenGLIndexBuffer m_indexBuffer;
	};
}