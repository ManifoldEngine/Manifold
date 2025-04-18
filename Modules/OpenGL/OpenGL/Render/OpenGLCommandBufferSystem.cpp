#include "OpenGLCommandBufferSystem.h"

#include <Core/Debug/Profiling.h>
#include <Core/Thread/Parallel.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/Render/OpenGLCommand.h>

#include <RenderAPI/MeshComponent.h>

#include <vector>

using namespace Mani;

void OpenGLCommandBufferSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<OpenGLCommandBuffer3D>();
}

void OpenGLCommandBufferSystem::onDeinitialize(ECS::Registry& registry)
{
	registry.removeSingle<OpenGLCommandBuffer3D>();
}

void OpenGLCommandBufferSystem::tick(float deltaTime, ECS::Registry& registry)
{
	MANI_TIME_SCOPE(OpenGLCommandBufferSystemtick);

	// gather all draw commands.
	std::array<std::vector<OpenGLCommand3D>, Application::THREAD_COUNT> threadBuffers;
	ECS::View<Position, Rotation, Scale, MeshComponent> view(registry);
	Mani::parallelFor(view, [&threadBuffers, &registry](ECS::EntityId entityId, size_t threadIndex)
	{
		MeshComponent& meshComponent = *registry.get<MeshComponent>(entityId);
		auto [position, rotation, scale] = Transform::getTransform(registry, entityId);

		// todo: camera frustrum culling
	
		threadBuffers[threadIndex].emplace_back(OpenGLCommand3D 
		{
			.model = Transform::model(*position, *rotation, *scale),
			.mesh = meshComponent.meshHandle,
			.material = meshComponent.materialHandle
		});
	});

	// merge all thread buffers into the command buffer.
	std::vector<OpenGLCommand3D> commandBuffer;
	for (const auto& buffer : threadBuffers)
	{
		commandBuffer.insert(commandBuffer.end(), buffer.begin(), buffer.end());
	}

	// todo sort the commands

	// update command buffers
	OpenGLCommandBuffer3D& cbs = *registry.getSingle<OpenGLCommandBuffer3D>();
	cbs.resetReadBuffer();

	const unsigned long long renderFrame = cbs.renderFrame;
	const unsigned long long delta = (cbs.frame + 1) - renderFrame; // we want the delta for the buffer we're about to write
	MANI_LOG_VERBOSE(LogOpenGL, "Read and write buffer frame delta: {}", delta);
	if (delta >= OpenGL::COMMAND_BUFFER_AMOUNT)
	{
		// todo: we could potentially sleep instead of just dropping the frame
		MANI_LOG_ERROR(LogOpenGL, "Write buffer has caught up with the read buffer, we're dropping frames.");
		return;
	}

	// write to the current write buffer
	cbs.buffers[cbs.writeBuffer] = std::move(commandBuffer);

	// mark the next read buffer as ready.
	cbs.writeBuffer = (cbs.writeBuffer + 1) % OpenGL::COMMAND_BUFFER_AMOUNT;
	cbs.readBuffer = cbs.writeBuffer;
	cbs.frame++;
}
