#pragma once

#include <ManiMaths/Fwd.h>

#include <ECS/Entity.h>

#include <memory>
#include <vector>
#include <array>
#include <atomic>

namespace Mani
{
	namespace OpenGL
	{
		constexpr unsigned int COMMAND_BUFFER_AMOUNT = 2;
	}

	struct OpenGLCommand3D
	{
		Mat4f model;
		ECS::EntityId mesh;
		ECS::EntityId material;
	};

	struct OpenGLCommand2D
	{

	};

	template<typename T>
	struct OpenGLCommandBuffer
	{
		std::array<std::vector<T>, OpenGL::COMMAND_BUFFER_AMOUNT> buffers;
		unsigned int readBuffer = OpenGL::COMMAND_BUFFER_AMOUNT;
		unsigned int writeBuffer = 0;
		
		unsigned long long frame = 0;
		std::atomic<unsigned long long> renderFrame = 0;

		void resetReadBuffer()
		{
			readBuffer = OpenGL::COMMAND_BUFFER_AMOUNT;
		}

		bool isReadBufferValid() const
		{
			return readBuffer < OpenGL::COMMAND_BUFFER_AMOUNT;
		}
	};

	typedef OpenGLCommandBuffer<OpenGLCommand3D> OpenGLCommandBuffer3D;
	typedef OpenGLCommandBuffer<OpenGLCommand2D> OpenGLCommandBuffer2D;
}