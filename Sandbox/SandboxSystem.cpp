#include "SandboxSystem.h"
#include <Core/Application.h>
#include <OpenGL/Shader/OpenGLShaderSystem.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Core/Components/Transform.h>
#include <ECS/RegistryView.h>
#include <Camera/CameraSystem.h>
#include <random>

using namespace ECSEngine;

struct Cube {};

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::weak_ptr<OpenGLShaderSystem> shaderSystem = Application::get().getSystemContainer().initializeDependency<OpenGLShaderSystem>();
    if (shaderSystem.expired())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the layer.");
        return;
    }

    
    m_shader = shaderSystem.lock()->getShader("base.glsl");

    float vertices[] = {
        // positions            // texture coords
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f
    };

    const size_t verticesSize = std::size(vertices);
    unsigned int indices[verticesSize];
    for (int i = 0; i < verticesSize; ++i)
    {
        indices[i] = i;
    }

    std::shared_ptr<OpenGLVertexBuffer> squareVertexBuffer = std::make_shared<OpenGLVertexBuffer>(vertices, (int)sizeof(vertices));
    squareVertexBuffer->layout = {
        { ShaderDataType::Float3, false },
        { ShaderDataType::Float2, false }
    };

    std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(indices, (int)sizeof(indices));

    m_squareVertexArray = std::make_unique<OpenGLVertexArray>();
    m_squareVertexArray->addVertexBuffer(squareVertexBuffer);
    m_squareVertexArray->setIndexBuffer(indexBuffer);

    m_woodenBoxTexture2D = std::make_unique<OpenGLTexture2D>("Assets/Images/container.jpg");
    m_awesomeFaceTexture2D = std::make_unique<OpenGLTexture2D>("Assets/Images/awesomeface.png");

    
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    glm::vec3 lowerBound = glm::vec3(-20.f, -20.f, -20.f);
    glm::vec3 upperBound = glm::vec3( 20.f,  20.f,  20.f);
    std::mt19937 gen(UINT32_MAX);
    std::uniform_real_distribution<float> randomX(lowerBound.x, upperBound.x);
    std::uniform_real_distribution<float> randomY(lowerBound.y, upperBound.y);
    std::uniform_real_distribution<float> randomZ(lowerBound.z, upperBound.z);

    for (int i = 0; i < 1'000; i++)
    {
        EntityId entityId = registry.create();
        registry.addComponent<Cube>(entityId);
        Transform* transform = registry.addComponent<Transform>(entityId);
        const glm::vec3 position = glm::vec3(
            randomX(gen),
            randomY(gen),
            randomZ(gen)
        );
        
        transform->position = position;
        transform->rotation = glm::vec3(-55.f, 0.0f, 0.0f);
        transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    }
}

void SandboxSystem::onDeinitialize(EntityRegistry& registry)
{
    m_squareVertexArray.reset();
}

void SandboxSystem::tick(float deltaTime, EntityRegistry& registry)
{
    glEnable(GL_DEPTH_TEST);

    // setting color state.
    glClearColor(.2f, .3f, .3f, 1.f);
    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_shader == nullptr)
    {
        ECSE_LOG_ERROR(LogOpenGL, "No shader available.");
        return;
    }

    glm::mat4 view;
    glm::mat4 projection;

    RegistryView<CameraComponent> cameraRegistryView(registry);
    for (const EntityId& entityId : cameraRegistryView)
    {
        if (const CameraComponent* cameraComponent = registry.getComponent<CameraComponent>(entityId))
        {
            view = cameraComponent->view;
            projection = cameraComponent->projection;
        }
    }

    m_woodenBoxTexture2D->bind(0);
    m_awesomeFaceTexture2D->bind(1);

    // set the shader program to be used.
    m_shader->use();
    // set vertex uniforms
    m_shader->setFloatMatrix4("view", glm::value_ptr(view));
    m_shader->setFloatMatrix4("projection", glm::value_ptr(projection));

    // set fragment uniforms
    m_shader->setTextureSlot("inputTexture1", 0);
    m_shader->setTextureSlot("inputTexture2", 1);

    //ECSE_LOG(Log, "{}", 1.f/ deltaTime);

    int index = 0;
    RegistryView<Transform, Cube> registryView(registry);
    for (const auto& entityId : registryView)
    {
        if (Transform* transform = registry.getComponent<Transform>(entityId))
        {
            const float angle = 20.f * (index % 12) + 1;

            glm::quat qPitch = glm::angleAxis(glm::radians(angle * .5f * deltaTime), glm::vec3(1.f, 0.f, 0.f));
            glm::quat qYaw = glm::angleAxis(glm::radians(angle * deltaTime), glm::vec3(0.f, 1.f, 0.f));
            glm::quat qRoll = glm::angleAxis(glm::radians(angle * deltaTime), glm::vec3(0.f, 0.f, 1.f));
            
            transform->rotation *= qRoll;
            transform->rotation *= qYaw;
            transform->rotation *= qPitch;
            
            glm::mat4 transformMatrix = transform->calculate();
            m_shader->setFloatMatrix4("model", glm::value_ptr(transformMatrix));
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
        index++;
    }
}
