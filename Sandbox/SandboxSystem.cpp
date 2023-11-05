#include "SandboxSystem.h"
#include <Core/Application.h>
#include <OpenGL/Shader/OpenGLShaderSystem.h>
#include <GL/glew.h>

using namespace ECSEngine;

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::weak_ptr<OpenGLShaderSystem> shaderSystem = Application::get().getSystemContainer().initializeDependency<OpenGLShaderSystem>();
    if (shaderSystem.expired())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the layer.");
        return;
    }

    m_shader = shaderSystem.lock()->getShader("coloredVertex.glsl");

    float triangleVertices[] =
    {     
        // positions                // colors       // texture coords
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 0.0f,   0.0f, 1.0f // top left
    };

    unsigned int triangleIndices[] =
    {
        0, 1, 2, 0, 2, 3 // first triangle
    };

    std::shared_ptr<OpenGLVertexBuffer> squareVertexBuffer = std::make_shared<OpenGLVertexBuffer>(triangleVertices, (int)sizeof(triangleVertices));
    squareVertexBuffer->layout = {
        { ShaderDataType::Float3, false },
        { ShaderDataType::Float3, false },
        { ShaderDataType::Float2, false }
    };

    std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(triangleIndices, (int)sizeof(triangleIndices));

    m_squareVertexArray = std::make_unique<OpenGLVertexArray>();
    m_squareVertexArray->addVertexBuffer(squareVertexBuffer);
    m_squareVertexArray->setIndexBuffer(indexBuffer);

    m_woodenBoxTexture2D = std::make_unique<OpenGLTexture2D>("Assets/Images/container.jpg");
    m_awesomeFaceTexture2D = std::make_unique<OpenGLTexture2D>("Assets/Images/awesomeface.png");
}

void SandboxSystem::onDeinitialize(EntityRegistry& registry)
{
    m_squareVertexArray.reset();
}

void SandboxSystem::tick(float deltaTime, EntityRegistry& registry)
{
    // setting color state.
    glClearColor(.2f, .3f, .3f, 1.f);

    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_shader == nullptr)
    {
        ECSE_LOG_ERROR(LogOpenGL, "No shader available.");
        return;
    }

    m_woodenBoxTexture2D->bind(0);
    m_awesomeFaceTexture2D->bind(1);
    // set the shader program to be used.
    m_shader->use();
    m_shader->setTextureSlot("inputTexture1", 0);
    m_shader->setTextureSlot("inputTexture2", 1);
    m_squareVertexArray->bind();
    if (const auto& indexBuffer = m_squareVertexArray->getIndexBuffer())
    {
        glDrawElements(GL_TRIANGLES, indexBuffer->getStrideCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        ECSE_ASSERT(false, "no index buffer provided with the vertices");
    }
}
