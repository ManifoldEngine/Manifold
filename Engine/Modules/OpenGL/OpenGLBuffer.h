#pragma once

#include "OpenGL.h"
#include <Core/Assert.h>

namespace ECSEngine
{
	enum class ShaderDataType : uint8_t
	{
		None = 0, 
		Float, 
		Float2, 
		Float3, 
		Float4, 
		Mat3, 
		Mat4, 
		Int, 
		Int2, 
		Int3, 
		Int4, 
		Bool
	};

	struct BufferLayoutElement
	{
		ShaderDataType shaderType = ShaderDataType::None;
		bool bIsNormalized = false;
	};

	class OpenGLVertexBuffer
	{
	public:

		OpenGLVertexBuffer(float* pData, int count);
		~OpenGLVertexBuffer();
		
		// buffers can't be copied implicitely.
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;

		void bind() const;
		int getStrideCount() const;
		int getStrideSize() const;
		
		std::vector<BufferLayoutElement> layout;
		
		static int getShaderDataTypeSize(ShaderDataType type);
		static int getComponentCount(ShaderDataType type);
	private:
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