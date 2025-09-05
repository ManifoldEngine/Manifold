#pragma once

#include <ManiMaths/Fwd.h>

#include <ECS/Entity.h>

#include <RenderAPI/Shader.h>

#include <OpenGL/Data/OpenGLShader.h>
#include <OpenGL/Data/OpenGLVertexArray.h>
#include <OpenGL/Data/OpenGLTexture.h>

#include <memory>
#include <vector>
#include <array>
#include <atomic>
#include <semaphore>

namespace Mani
{
	namespace OpenGL
	{
		constexpr unsigned int COMMAND_BUFFER_AMOUNT = 2;
	}

	struct OpenGLCommand
	{
		Mat4f model;

		OpenGLVertexArray* vao;
		OpenGLShader* shader;
		Vec4f color = VEC4F::ONE;
		std::vector<std::pair<std::string, OpenGLTexture2D*>> textures;
		std::vector<std::pair<std::string, ShaderType>> customParamaters;

		int rendererId = 0;
	};

	struct OpenGLCommandBuffer
	{
		std::vector<OpenGLCommand> commands;

		std::binary_semaphore isReadyToWrite{ 0 };
	};

	struct OpenGLCommandBufferCollection
	{
		std::array<OpenGLCommandBuffer, OpenGL::COMMAND_BUFFER_AMOUNT> buffers;
		
		unsigned int readBuffer = OpenGL::COMMAND_BUFFER_AMOUNT;
		unsigned int writeBuffer = 0;

		bool isReadBufferValid() const
		{
			return readBuffer < OpenGL::COMMAND_BUFFER_AMOUNT;
		}
	};
}