#include "SandboxSystem.h"
#include <Core/Application.h>
#include <OpenGL/Shader/OpenGLShaderSystem.h>
#include <GL/glew.h>

using namespace ECSEngine;

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::weak_ptr<OpenGLShaderSystem> pShaderSystem = Application::get().getSystemContainer().initializeDependency<OpenGLShaderSystem>();
    if (pShaderSystem.expired())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the layer.");
        return;
    }

    m_pShader = pShaderSystem.lock()->getShader("coloredVertex.glsl");

    float triangleVertices[] =
    {
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top
    };

    unsigned int triangleIndices[] =
    {
        0, 1, 2, // first triangle
    };

    std::shared_ptr<OpenGLVertexBuffer> pSquareVertexBuffer = std::make_shared<OpenGLVertexBuffer>(triangleVertices, (int)sizeof(triangleVertices));
    pSquareVertexBuffer->layout = {
        { ShaderDataType::Float3, false },
        { ShaderDataType::Float3, false }
    };

    std::shared_ptr<OpenGLIndexBuffer> pIndexBuffer = std::make_shared<OpenGLIndexBuffer>(triangleIndices, (int)sizeof(triangleIndices));

    m_pSquareVertexArray = std::make_unique<OpenGLVertexArray>();
    m_pSquareVertexArray->addVertexBuffer(pSquareVertexBuffer);
    m_pSquareVertexArray->setIndexBuffer(pIndexBuffer);
}

void SandboxSystem::onDeinitialize(EntityRegistry& registry)
{
    m_pSquareVertexArray.reset();
}

void SandboxSystem::tick(float deltaTime, EntityRegistry& registry)
{
    // setting color state.
    glClearColor(.2f, .3f, .3f, 1.f);

    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_pShader == nullptr)
    {
        ECSE_LOG_ERROR(LogOpenGL, "No shader available.");
        return;
    }

    // set the shader program to be used.
    m_pShader->use();

    m_pSquareVertexArray->bind();
    if (const auto& pIndexBuffer = m_pSquareVertexArray->getIndexBuffer())
    {
        glDrawElements(GL_TRIANGLES, pIndexBuffer->getStrideCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        ECSE_ASSERT(false, "no index buffer provided with the vertices");
    }
}
