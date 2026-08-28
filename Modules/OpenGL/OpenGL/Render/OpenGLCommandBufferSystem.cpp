#include "OpenGLCommandBufferSystem.h"

#include <Core/Debug/Profiling.h>
#include <Core/Async/Parallel.h>

#include <Camera/Camera.h>

#include <Resources/Resources.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/Resources/OpenGLMaterial.h>
#include <OpenGL/Resources/OpenGLResourceSystem.h>
#include <OpenGL/Render/OpenGLCommand.h>

#include <RenderAPI/Components/MeshRendering.h>
#include <RenderAPI/Components/SpriteRendering.h>
#include <RenderAPI/Components/BoundingSphere.h>

using namespace Mani;

void OpenGLCommandBufferSystem::onInitialize(ECS::Registry& registry, World& world)
{
	OpenGLCommandBuffers& cbs = registry.addSinglePinned<OpenGLCommandBuffers>();
	// release all command buffers so they're ready to receive commands.
	for (SizeT i = 0; i < OpenGL::COMMAND_BUFFER_AMOUNT; i++)
	{
		OpenGLCommandBuffer& buffer = cbs.buffers[i];
		buffer.isReadyToWrite = m_semaphores[i].getPtr();
		buffer.isReadyToWrite->release();
	}
}

void OpenGLCommandBufferSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	registry.removeSinglePinned<OpenGLCommandBuffers>();
	for (SizeT i = 0; i < OpenGL::COMMAND_BUFFER_AMOUNT; i++)
	{
		m_semaphores[i].getPtr()->acquire();
	}
}

void OpenGLCommandBufferSystem::tick(ECS::Registry& registry)
{
	// camera
	ECS::EntityId cameraId = Cameras::getMainCameraId(registry);
	MANI_ASSERT(cameraId != ECS::INVALID_ID, "Trying to render without a camera");
	auto cameraPosition = registry.get<Position>(cameraId);
	auto camera = registry.get<Camera>(cameraId);

	// gather all draw commands.
	std::array<List<OpenGLCommand>, Application::THREAD_COUNT> threadBuffers;
	ECS::ConstView<Position, Rotation, Scale, MeshRendering> view(registry);
	Mani::parallelFor(view, [&threadBuffers, &registry, &cameraPosition, &camera](SizeT threadIdx, ECS::EntityId entityId, const Position& position, const Rotation& rotation, const Scale& scale, const MeshRendering& meshRendering)
	{
		if (Ref<BoundingSphere> bounds = registry.find<BoundingSphere>(entityId))
		{
			if (!Cameras::isInView(*camera, position, rotation, scale, *bounds))
			{
				return;
			}
		}

		const Resource<OpenGLVertexArray>* vaoRes = registry.findPinned<Resource<OpenGLVertexArray>>(meshRendering.meshResourceId);
		if (vaoRes == nullptr || !Resources::isReady(registry, meshRendering.meshResourceId))
		{
			// resource is not ready yet.
			return;
		}
		
		const Resource<OpenGLMaterial>* materialRes = registry.findPinned<Resource<OpenGLMaterial>>(meshRendering.materialResourceId);
		if (materialRes == nullptr || !Resources::isReady(registry, meshRendering.materialResourceId))
		{
			// resource is not ready yet.
			return;
		}

		const Resource<OpenGLShader>* shaderRes = registry.findPinned<Resource<OpenGLShader>>(materialRes->value.shaderId);
		if (shaderRes == nullptr || !Resources::isReady(registry, materialRes->value.shaderId))
		{
			return;
		}

		OpenGLCommand command = {
			.model = Transform::model(position, rotation, scale),

			.vao = &vaoRes->value,
			.shader = &shaderRes->value,
			.rendererId = meshRendering.rendererId,
		};

		for (const auto& texture : materialRes->value.textures)
		{
			if (Resource<OpenGLTexture2D>* res = registry.findPinned<Resource<OpenGLTexture2D>>(texture.id))
			{
				if (!Resources::isReady(registry, texture.id))
				{
					continue;
				}
				command.textures.add({ texture.key, &res->value });
			}
		}

		// textures parameters can override existing textures in the material if they share the same key
		for (const auto& [key, resourceId] : meshRendering.textureParameters)
		{
			if (Resource<OpenGLTexture2D>* res = registry.findPinned<Resource<OpenGLTexture2D>>(resourceId))
			{
				if (!Resources::isReady(registry, resourceId))
				{
					continue;
				}

				const SizeT index = command.textures.indexOfIf([&key](const auto& texture)
				{
					return texture.key == key;
				});

				if (index != INDEX_NONE)
				{
					command.textures[index].texture = &res->value;
				}
				else
				{
					command.textures.add({ key, &res->value });
				}
			}
		}

		for (const auto& shaderParam : materialRes->value.shaderParameters)
		{
			command.customParamaters.add({ shaderParam.key, shaderParam.value });
		}

		for (const auto& [key, value] : meshRendering.shaderParameters)
		{
			command.customParamaters.add({ key, value });
		}

		if (Ref<SpriteRendering> spriteRendering = registry.find<SpriteRendering>(entityId))
		{
			command.customParamaters.add({ ShaderNames::MANI_SPRITE_WORLD_SIZE, spriteRendering->size });

			if (Resources::isReady(registry, spriteRendering->textureId))
			{
				auto& openglTex = registry.getPinned<Resource<OpenGLTexture2D>>(spriteRendering->textureId);
				command.textures.add({ ShaderNames::MANI_SPRITE_TEXTURE, &openglTex.value});

				const auto& tex = registry.getPinned<Resource<Texture>>(spriteRendering->textureId);
				command.customParamaters.add({ ShaderNames::MANI_SPRITE_TEXTURE_SIZE, tex.value.size });
			}
		}

		threadBuffers[threadIdx].add(std::move(command));
	});

	// merge all thread buffers into the command buffer.
	List<OpenGLCommand> commands;
	for (auto&& buffer : threadBuffers)
	{
		commands.append(std::move(buffer));
	}

	commands.sort([](const OpenGLCommand& lhs, const OpenGLCommand& rhs)
	{
		return std::tie(lhs.rendererId, lhs.shader, lhs.vao) < std::tie(rhs.rendererId, rhs.shader, rhs.vao);
	});

	// update command buffers
	OpenGLCommandBuffers& cbs = registry.getSinglePinned<OpenGLCommandBuffers>();

	// get next write buffer
	OpenGLCommandBuffer& writeBuffer = cbs.buffers[cbs.writeBuffer];

	MANI_ASSERT(writeBuffer.isReadyToWrite != nullptr, "Invalid buffer");
	// wait for the write buffer to finish rendering if needed.
	writeBuffer.isReadyToWrite->acquire();

	// clear read command buffer
	// write to the current write buffer
	writeBuffer.commands.clear();
	writeBuffer.commands = std::move(commands);
	MANI_LOG_VERBOSE(LogOpenGL, "wrote {} opengl to command buffer", writeBuffer.commands.count());
	// set the next read buffer
	cbs.readBuffer = cbs.writeBuffer;
	// set the next write buffer
	cbs.writeBuffer = (cbs.writeBuffer + 1) % OpenGL::COMMAND_BUFFER_AMOUNT;
}
