#pragma once

#include "OpenGL.h"
#include <Core/ManiAssert.h>

namespace Mani
{
	enum class EShaderDataType : uint8_t
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
		EShaderDataType shaderType = EShaderDataType::None;
		bool isNormalized = false;
	};

	class OpenGLVertexBuffer
	{
	public:
		OpenGLVertexBuffer();
		OpenGLVertexBuffer(const float* data, size_t size);
		~OpenGLVertexBuffer();
		
		// buffers can't be copied implicitely.
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = delete;
		OpenGLVertexBuffer(OpenGLVertexBuffer&& other) noexcept;
		OpenGLVertexBuffer& operator=(OpenGLVertexBuffer&& other) noexcept;

		void set(const float* data, size_t size);

		void bind() const;
		size_t getStrideCount() const;
		size_t getStrideSize() const;
		
		std::vector<BufferLayoutElement> layout;
		
		static size_t getShaderDataTypeSize(EShaderDataType type);
		static size_t getComponentCount(EShaderDataType type);
	private:
		void create();

		size_t m_size;
		unsigned int m_vertexBufferObjectId;
	};

	class OpenGLIndexBuffer
	{
	public:
		OpenGLIndexBuffer();
		OpenGLIndexBuffer(const unsigned int* indices, size_t size);
		~OpenGLIndexBuffer();

		// buffers can't be copied implicitely.
		OpenGLIndexBuffer(const OpenGLVertexBuffer&) = delete;
		OpenGLIndexBuffer operator=(const OpenGLVertexBuffer&) = delete;
		
		OpenGLIndexBuffer(OpenGLIndexBuffer&& other) noexcept;
		OpenGLIndexBuffer& operator=(OpenGLIndexBuffer&& other) noexcept;

		void set(const unsigned int* indices, size_t size);

		void bind() const;
		size_t getStrideCount() const;
		size_t getStrideSize() const;

	private:
		void create();

		size_t m_size;
		unsigned int m_indexBufferObjectId;
	};
}