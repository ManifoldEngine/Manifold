#include "OpenGLCommandBufferSystem.h"

#include <Core/Debug/Profiling.h>
#include <Core/Async/Parallel.h>

#include <Camera/Camera.h>

#include <Resources/Resource.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/Data/OpenGLMaterial.h>
#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/OpenGLResourceSystem.h>

#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/BoundingSphere.h>

#include <vector>
#include <algorithm>

using namespace Mani;

void OpenGLCommandBufferSystem::onInitialize(ECS::Registry& registry, World& world)
{
	OpenGLCommandBufferCollection& cbs = *registry.addSingle<OpenGLCommandBufferCollection>();
	// release all command buffers so they're ready to receive commands.
	for (auto& buffer : cbs.buffers)
	{
		buffer.isReadyToWrite.release();
	}
}

void OpenGLCommandBufferSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	registry.removeSingle<OpenGLCommandBufferCollection>();
}

void OpenGLCommandBufferSystem::tick(ECS::Registry& registry)
{
	MANI_TIME_SCOPE(OpenGLCommandBufferSystem_tick);

	// camera
	ECS::EntityId cameraId = CameraStatics::getMainCameraId(registry);
	auto [cameraPosition, camera] = registry.getMany<Position, Camera>(cameraId);
	if (camera == nullptr)
	{
		return;
	}

	// gather all draw commands.
	std::array<std::vector<OpenGLCommand>, Application::THREAD_COUNT> threadBuffers;
	ECS::View<Position, Rotation, Scale, MeshComponent> view(registry);
	Mani::parallelFor(view, [&threadBuffers, &registry, cameraPosition, camera](ECS::EntityId entityId, size_t threadIndex)
	{
		MeshComponent& meshComponent = *registry.get<MeshComponent>(entityId);
		auto [position, rotation, scale] = Transform::getTransform(registry, entityId);

		if (const BoundingSphere* boundingSphere = registry.get<BoundingSphere>(entityId))
		{
			if (!CameraStatics::isInView(*camera, *position, *rotation, *scale, *boundingSphere))
			{
				return;
			}
		}

		Resource<OpenGLVertexArray>* vaoRes = registry.get<Resource<OpenGLVertexArray>>(meshComponent.meshHandle);
		if (vaoRes == nullptr || !vaoRes->isReady)
		{
			// resource is not ready yet.
			return;
		}
		
		Resource<OpenGLMaterial>* materialRes = registry.get<Resource<OpenGLMaterial>>(meshComponent.materialHandle);
		if (materialRes == nullptr || !materialRes->isReady)
		{
			// resource is not ready yet.
			return;
		}

		const OpenGLMaterial& material = materialRes->value;
		Resource<OpenGLShader>* shaderRes = registry.get<Resource<OpenGLShader>>(material.shaderId);
		MANI_ASSERT(shaderRes != nullptr, "We expect the shader to exist at this point.");
		
		OpenGLCommand command = {
			.model = Transform::model(*position, *rotation, *scale),

			.vao = &vaoRes->value,
			.shader = &shaderRes->value,
			.color = material.color,
			.rendererId = meshComponent.rendererId,
		};

		for (const auto& texture : material.textures)
		{
			if (Resource<OpenGLTexture2D>* res = registry.get<Resource<OpenGLTexture2D>>(texture.id))
			{
				if (!res->isReady)
				{
					continue;
				}
				command.textures.push_back({ texture.key, &res->value });
			}
		}

		for (const auto& [key, value] : meshComponent.shaderParameters)
		{
			command.customParamaters.push_back({ key, value });
		}

		for (const auto& [key, value] : meshComponent.textureParameters)
		{
			if (Resource<OpenGLTexture2D>* res = registry.get<Resource<OpenGLTexture2D>>(value))
			{
				if (!res->isReady)
				{
					continue;
				}

				auto it = std::find_if(command.textures.begin(), command.textures.end(), [&key](const auto& texture)
				{
					return texture.first == key;
				});
				if (it != command.textures.end())
				{
					*it = { key, &res->value };
				}
				else
				{
					command.textures.push_back({ key, &res->value });
				}
			}
		}

		threadBuffers[threadIndex].emplace_back(command);
	});

	// merge all thread buffers into the command buffer.
	std::vector<OpenGLCommand> commands;
	for (const auto& buffer : threadBuffers)
	{
		commands.insert(commands.end(), buffer.begin(), buffer.end());
	}

	std::sort(commands.begin(), commands.end(), [](const OpenGLCommand& lhs, const OpenGLCommand& rhs) 
	{
		return std::tie(lhs.shader, lhs.vao, lhs.rendererId) < std::tie(rhs.shader, rhs.vao, rhs.rendererId);
	});

	// update command buffers
	OpenGLCommandBufferCollection& cbs = *registry.getSingle<OpenGLCommandBufferCollection>();

	// get next write buffer
	OpenGLCommandBuffer& writeBuffer = cbs.buffers[cbs.writeBuffer];

	// wait for the write buffer to finish rendering if needed.
	writeBuffer.isReadyToWrite.acquire();

	// clear read command buffer
	// write to the current write buffer
	writeBuffer.commands.clear();
	writeBuffer.commands = std::move(commands);
	MANI_LOG_VERBOSE(LogOpenGL, "wrote {} opengl to command buffer", writeBuffer.commands.size());
	// set the next read buffer
	cbs.readBuffer = cbs.writeBuffer;
	// set the next write buffer
	cbs.writeBuffer = (cbs.writeBuffer + 1) % OpenGL::COMMAND_BUFFER_AMOUNT;
}
