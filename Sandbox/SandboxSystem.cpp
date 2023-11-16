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

struct RenderComponent
{
    std::shared_ptr<OpenGLVertexArray> vao;
    std::shared_ptr<OpenGLTexture2D> texture;
    std::shared_ptr<OpenGLShader> shader;

    glm::vec3 color;
};

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::weak_ptr<OpenGLShaderSystem> shaderSystem = Application::get().getSystemContainer().initializeDependency<OpenGLShaderSystem>();
    if (shaderSystem.expired())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the layer.");
        return;
    }

    
    std::shared_ptr<OpenGLShader> shader = shaderSystem.lock()->getShader("base.glsl");

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

    std::shared_ptr<OpenGLVertexArray> squareVertexArray = std::make_shared<OpenGLVertexArray>();
    squareVertexArray->addVertexBuffer(squareVertexBuffer);
    squareVertexArray->setIndexBuffer(indexBuffer);

    std::shared_ptr<OpenGLTexture2D> woodenBoxTexture2D = std::make_shared<OpenGLTexture2D>("Assets/Images/container.jpg");

    // cube
    EntityId cubeEntityId = registry.create();
    Transform* cubeTransform = registry.addComponent<Transform>(cubeEntityId);
    RenderComponent* cubeRender = registry.addComponent<RenderComponent>(cubeEntityId);
    cubeRender->vao = squareVertexArray;
    cubeRender->texture = woodenBoxTexture2D;
    cubeRender->shader = shader;
    cubeRender->color = glm::vec3(1.0f, 0.5f, 0.31f);

    // light
    EntityId lightEntityId = registry.create();
    Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
    lightTransform->position = glm::vec3(-3.f, 0.f, -3.f);
    RenderComponent* lightRender = registry.addComponent<RenderComponent>(lightEntityId);
    lightRender->vao = squareVertexArray;
    lightRender->shader = shader;
    lightRender->color = glm::vec3(1.f, 1.f, 1.f);

    // floor
    EntityId floorEntityId = registry.create();
    Transform* floorTransform = registry.addComponent<Transform>(floorEntityId);
    floorTransform->position = glm::vec3(0.f, -0.55f, 0.f);
    floorTransform->scale = glm::vec3(20.f, 0.1f, 20.f);
    RenderComponent* floorRender = registry.addComponent<RenderComponent>(floorEntityId);
    floorRender->vao = squareVertexArray;
    floorRender->shader = shader;
    floorRender->color = glm::vec3(.1f, .1f, .1f);

    Transform* cameraTransform = systemContainer.initializeDependency<CameraSystem>().lock()->getCameraTransform(registry);
    if (cameraTransform != nullptr && lightTransform != nullptr)
    {
        cameraTransform->position = glm::vec3(12.67f, 3.04f, -9.13f);
        cameraTransform->rotation = glm::normalize(glm::quat(0.87f, 0.09f, -0.46f, 0.04f));
    }
}

void SandboxSystem::onDeinitialize(EntityRegistry& registry)
{
}

void SandboxSystem::tick(float deltaTime, EntityRegistry& registry)
{
    glEnable(GL_DEPTH_TEST);

    // setting color state.
    glClearColor(.2f, .3f, .3f, 1.f);
    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    //ECSE_LOG(Log, "{}", 1.f/ deltaTime);

    int index = 0;
    RegistryView<Transform, RenderComponent> registryView(registry);
    for (const auto& entityId : registryView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        RenderComponent* render = registry.getComponent<RenderComponent>(entityId);
       
        // set the shader program to be used.
        render->shader->use();
        if (render->texture != nullptr)
        {
            render->texture->bind(0);
            render->shader->setTextureSlot("inputTexture1", 0);
        }
        
        // set vertex uniforms
        render->shader->setFloatMatrix4("view", glm::value_ptr(view));
        render->shader->setFloatMatrix4("projection", glm::value_ptr(projection));

        const glm::vec3& color = render->color;
        // set fragment uniforms
        render->shader->setFloat3("color", color.x, color.y, color.z);
        render->shader->setFloat3("lightColor", 1.0f, 1.0f, 1.0f);

        glm::mat4 transformMatrix = transform->calculate();
        render->shader->setFloatMatrix4("model", glm::value_ptr(transformMatrix));
        render->vao->bind();
        if (const auto& indexBuffer = render->vao->getIndexBuffer())
        {
            glDrawElements(GL_TRIANGLES, indexBuffer->getStrideCount(), GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            ECSE_ASSERT(false, "no index buffer provided with the vertices");
        }
    }
}
