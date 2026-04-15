#include "OpenGLUIRenderer.h"

#include <Core/Debug/Profiling.h>

#include <OpenGL/Render/OpenGLRenderSystem.h>
#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/OpenGLRenderContext.h>

#include <OpenGL/Resources/OpenGLVertexArray.h>
#include <OpenGL/Resources/OpenGLShader.h>

#include <Resources/ResourceSystem.h>
#include <RenderAPI/Primitives.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/RendererIds.h>

#include <GLFW/glfw3.h>

#include <variant>

using namespace Mani;

int OpenGLUIRenderer::getId() const
{
    return Mani::RendererIds::UI_PASS;
}

void OpenGLUIRenderer::onBegin(Mani::OpenGLRenderContext& context)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLUIRenderer::render(const OpenGLCommand& command, OpenGLRenderContext& context)
{
    MANI_TIME_SCOPE(RendererUI_render);

    MANI_ASSERT(command.vao != nullptr, "Command with undefined resource");
    MANI_ASSERT(command.shader != nullptr, "Command with undefined resource");

    const Mat4f& model = command.model;
    Mat4f projection = Mani::MAT4F::IDENTITY;
    if (const Mani::ShaderType* value = context.renderContext.shaderParameters.find(Mani::ShaderNames::MANI_UI_PROJECTION))
    {
        projection = std::get<Mat4f>(*value);
    }

    const OpenGLVertexArray& vao = *command.vao;
    const OpenGLShader& shader = *command.shader;

    shader.use();
    shader.setFloatMatrix4(Mani::ShaderNames::MANI_MODEL, &(model._00));
    shader.setFloatMatrix4(Mani::ShaderNames::MANI_PROJECTION, &(projection._00));

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

void OpenGLUIRenderer::onEnd(Mani::OpenGLRenderContext& context)
{
    glDisable(GL_BLEND);
}

void OpenGLUIRendererSystem::onInitialize(ECS::Registry& registry, World& world)
{
    world.initializeDependency<OpenGLRenderSystem>();
    OpenGL::registerRenderer(registry, &m_renderer);
}

void OpenGLUIRendererSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
    OpenGL::unregisterRenderer(registry, &m_renderer);
}
