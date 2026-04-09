#pragma once

#include <ManiMaths/Fwd.h>

#include <Core/Containers/List.h>
#include <Core/ECS/Entity.h>

#include <RenderAPI/Shader.h>

#include <Core/Containers/Array.h>

#include <OpenGL/Resources/OpenGLMaterial.h>
#include <OpenGL/Resources/OpenGLShader.h>
#include <OpenGL/Resources/OpenGLVertexArray.h>
#include <OpenGL/Resources/OpenGLTexture.h>

#include <memory>
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

		const OpenGLVertexArray* vao;
		const OpenGLShader* shader;
		List<TextureParam> textures;
		List<OpenGLMaterial::ShaderParam> customParamaters;

		int rendererId = 0;
	};

	struct OpenGLCommandBuffer
	{
		List<OpenGLCommand> commands;

		std::binary_semaphore* isReadyToWrite = nullptr;
	};

	struct OpenGLCommandBuffers
	{
		Mani::Array<OpenGLCommandBuffer, OpenGL::COMMAND_BUFFER_AMOUNT> buffers;
		
		unsigned int readBuffer = OpenGL::COMMAND_BUFFER_AMOUNT;
		unsigned int writeBuffer = 0;

		bool isReadBufferValid() const
		{
			return readBuffer < OpenGL::COMMAND_BUFFER_AMOUNT;
		}
	};
}