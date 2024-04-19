#pragma once

#include "OpenGL.h"
#include <Core/ManiAssert.h>

namespace Mani
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
		bool isNormalized = false;
	};

	class OpenGLVertexBuffer
	{
	public:

		OpenGLVertexBuffer(float* data, size_t count);
		~OpenGLVertexBuffer();
		
		// buffers can't be copied implicitely.
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;

		void bind() const;
		size_t getStrideCount() const;
		size_t getStrideSize() const;
		
		std::vector<BufferLayoutElement> layout;
		
		static size_t getShaderDataTypeSize(ShaderDataType type);
		static size_t getComponentCount(ShaderDataType type);
	private:
		size_t m_size;
		unsigned int m_vertexBufferObjectId;
	};

	class OpenGLIndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned int* indices, size_t size);
		~OpenGLIndexBuffer();

		// buffers can't be copied implicitely.
		OpenGLIndexBuffer(const OpenGLVertexBuffer&) = delete;

		void bind() const;
		size_t getStrideCount() const;
		size_t getStrideSize() const;

	private:
		size_t m_size;
		unsigned int m_indexBufferObjectId;
	};
}