#include "SandboxSystem.h"
#include <Core/Application.h>
#include <OpenGL/Shader/OpenGLShaderSystem.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Core/Components/Transform.h>
#include <ECS/RegistryView.h>

using namespace ECSEngine;

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::weak_ptr<OpenGLShaderSystem> shaderSystem = Application::get().getSystemContainer().initializeDependency<OpenGLShaderSystem>();
    if (shaderSystem.expired())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the layer.");
        return;
    }

    m_shader = shaderSystem.lock()->getShader("base.glsl");

    float squareVertices[] =
    {     
        // positions            // texture coords
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f // top left
    };

    unsigned int squareIndices[] =
    {
        0, 1, 2, 
        0, 2, 3 
    };

    std::shared_ptr<OpenGLVertexBuffer> squareVertexBuffer = std::make_shared<OpenGLVertexBuffer>(squareVertices, (int)sizeof(squareVertices));
    squareVertexBuffer->layout = {
        { ShaderDataType::Float3, false },
        { ShaderDataType::Float2, false }
    };

    std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(squareIndices, (int)sizeof(squareIndices));

    m_squareVertexArray = std::make_unique<OpenGLVertexArray>();
    m_squareVertexArray->addVertexBuffer(squareVertexBuffer);
    m_squareVertexArray->setIndexBuffer(indexBuffer);

    m_woodenBoxTexture2D = std::make_unique<OpenGLTexture2D>("Assets/Images/container.jpg");
    m_awesomeFaceTexture2D = std::make_unique<OpenGLTexture2D>("Assets/Images/awesomeface.png");

    std::srand(std::time(nullptr));
    for (int i = 0; i < 10'000; i++)
    {
        EntityId entityId = registry.create();
        Transform* transform = registry.addComponent<Transform>(entityId);
        double rx = (static_cast<double>(std::rand()) / RAND_MAX) * 3.f;
        double ry = (static_cast<double>(std::rand()) / RAND_MAX) * 3.f;
        double rr = (static_cast<double>(std::rand()) / RAND_MAX) * 359.f;
        transform->position = glm::vec3(rx - 1.5f, ry - 1.5f, .0f);
        transform->rotation = glm::vec3(0.0f, 0.0f, rr);
        transform->scale = glm::vec3(.5f, .5f, .5f);
    }
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

    ECSE_LOG(Log, "{}", 1.f/ deltaTime);

    RegistryView<Transform> view(registry);
    for (const auto& entityId : view)
    {
        if (Transform* transform = registry.getComponent<Transform>(entityId))
        {
            transform->position += glm::vec3(1.f, 0.f, 0.f) * deltaTime;
            if (transform->position.x > 1.5f)
            {
                transform->position += glm::vec3(-3.f, 0.f, .0f);;
            }

            glm::mat4 transformMatrix = transform->calculate();
            m_shader->setFloatMatrix4("transform", glm::value_ptr(transformMatrix));
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
    }
}
