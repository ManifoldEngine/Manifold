#pragma once

#include <ManiMaths/Fwd.h>

#include <Core/Containers/List.h>
#include <Core/ECS/Entity.h>

#include <RenderAPI/Shader.h>

#include <OpenGL/Data/OpenGLMaterial.h>
#include <OpenGL/Data/OpenGLShader.h>
#include <OpenGL/Data/OpenGLVertexArray.h>
#include <OpenGL/Data/OpenGLTexture.h>

#include <memory>
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
		struct TextureParam
		{
			std::string key;
			OpenGLTexture2D* texture;
		};

		Mat4f model;

		OpenGLVertexArray* vao;
		OpenGLShader* shader;
		Vec4f color = VEC4F::ONE;
		List<TextureParam> textures;
		List<OpenGLMaterial::ShaderParam> customParamaters;

		int rendererId = 0;
	};

	struct OpenGLCommandBuffer
	{
		List<OpenGLCommand> commands;

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