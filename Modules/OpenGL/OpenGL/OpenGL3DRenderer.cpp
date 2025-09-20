#include "OpenGL3DRenderer.h"

#include <Core/ManiAssert.h>
#include <Core/Debug/Profiling.h>

#include <OpenGL/Data/OpenGLVertexArray.h>
#include <OpenGL/Data/OpenGLShader.h>

#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/OpenGLRenderContext.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <GLFW/glfw3.h>

using namespace Mani;

// shader parameters
constexpr std::string_view MODEL = "model";
constexpr std::string_view NORMALMATRIX = "normalMatrix";
constexpr std::string_view VIEW = "view";
constexpr std::string_view PROJECTION = "projection";
constexpr std::string_view VIEWPOSITION = "viewPosition";
constexpr std::string_view COLOR = "color";

void OpenGL3DRenderer::render(const OpenGLCommand& command, OpenGLRenderContext& context)
{
	MANI_TIME_SCOPE(OpenGLRenderSystem_tick_renderthread_render);

	MANI_ASSERT(command.vao != nullptr, "Resource was unloaded unexpectedly.");
	MANI_ASSERT(command.shader != nullptr, "Resource was unloaded unexpectedly.");

	const Mat4f& model = command.model;
	const OpenGLVertexArray& vao = *command.vao;
	const OpenGLShader& shader = *command.shader;

	const Mat3f normalMatrix = static_cast<Mat3f>(model).inverse().transpose();

	shader.use();
	// set vertex uniforms
	shader.setFloatMatrix4(MODEL, &(model._00));
	shader.setFloatMatrix3(NORMALMATRIX, &(normalMatrix._00));
	shader.setFloatMatrix4(VIEW, &(context.view._00));
	shader.setFloatMatrix4(PROJECTION, &(context.projection._00));

	// set fragment uniforms
	shader.setFloat3(VIEWPOSITION, context.cameraPosition.x, context.cameraPosition.y, context.cameraPosition.z);

	const Vec4f& color = command.color;
	shader.setFloat4(COLOR, color.x, color.y, color.z, color.w);

	int textureIndex = 0;
	for (auto& [key, texture] : command.textures)
	{
		MANI_ASSERT(texture != nullptr, "Binding null texture.");
		texture->bind(textureIndex);
		shader.setTextureSlot(key, textureIndex);
		textureIndex++;
	}

	vao.bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vao.getIndexBuffer().getStrideCount()), GL_UNSIGNED_INT, nullptr);

	for (auto& [_, texture] : command.textures)
	{
		MANI_ASSERT(texture != nullptr, "Unbinding null texture.");
		texture->unbind();
	}
}

void OpenGL3DRendererSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<OpenGLRenderSystem>();
	OpenGLRenderSystem::registerRenderer(registry, &renderer);
}

void OpenGL3DRendererSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	OpenGLRenderSystem::unregisterRenderer(registry, &renderer);
}
