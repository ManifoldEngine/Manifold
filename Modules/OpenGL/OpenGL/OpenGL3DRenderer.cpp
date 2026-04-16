#include "OpenGL3DRenderer.h"

#include <Core/ManiAssert.h>
#include <Core/Debug/Profiling.h>

#include <OpenGL/Resources/OpenGLVertexArray.h>
#include <OpenGL/Resources/OpenGLShader.h>

#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/OpenGLRenderContext.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <RenderAPI/Shader.h>
#include <RenderAPI/RendererIds.h>

#include <GLFW/glfw3.h>

using namespace Mani;

int OpenGL3DRenderer::getId() const
{
	return Mani::RendererIds::FIRST_PASS;
}

void OpenGL3DRenderer::onBegin(OpenGLRenderContext& context)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
}

void OpenGL3DRenderer::render(const OpenGLCommand& command, OpenGLRenderContext& context)
{
	MANI_TIME_SCOPE("OpenGLRenderSystem_tick_renderthread_render");

	MANI_ASSERT(command.vao != nullptr, "Resource was unloaded unexpectedly.");
	MANI_ASSERT(command.shader != nullptr, "Resource was unloaded unexpectedly.");

	const Mat4f& model = command.model;
	const OpenGLVertexArray& vao = *command.vao;
	const OpenGLShader& shader = *command.shader;

	const Mat3f normalMatrix = static_cast<Mat3f>(model).inverse().transpose();

	shader.use();
	// set vertex uniforms
	shader.setFloatMatrix4(ShaderNames::MANI_MODEL, &(model._00));
	shader.setFloatMatrix3(ShaderNames::MANI_NORMALMATRIX, &(normalMatrix._00));
	shader.setFloatMatrix4(ShaderNames::MANI_VIEW, &(context.view._00));
	shader.setFloatMatrix4(ShaderNames::MANI_PROJECTION, &(context.projection._00));

	// set fragment uniforms
	for (const auto& [key, value] : command.customParamaters)
	{
		shader.setShaderType(key, value);
	}

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

void OpenGL3DRenderer::onEnd(OpenGLRenderContext& context)
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
}

void OpenGL3DRendererSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<OpenGLRenderSystem>();
	OpenGL::registerRenderer(registry, &m_renderer);
}

void OpenGL3DRendererSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	OpenGL::unregisterRenderer(registry, &m_renderer);
}
