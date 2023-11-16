#include "SandboxSystem.h"
#include <Core/Application.h>
#include <OpenGL/Shader/OpenGLShaderSystem.h>
#include <GL/glew.h>

#include <Core/Components/Transform.h>
#include <ECS/RegistryView.h>
#include <Camera/CameraSystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace ECSEngine;

struct RenderComponent
{
    std::shared_ptr<OpenGLVertexArray> vao;
    std::shared_ptr<OpenGLTexture2D> texture;
    std::shared_ptr<OpenGLShader> shader;

    glm::vec3 color;

    glm::vec3 materialAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 materialDiffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 materialSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    
    float shininess = 32.0f;
};

struct Cube {};

struct LightSourceComponent {};

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::weak_ptr<OpenGLShaderSystem> shaderSystem = Application::get().getSystemContainer().initializeDependency<OpenGLShaderSystem>();
    if (shaderSystem.expired())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the layer.");
        return;
    }

    
    std::shared_ptr<OpenGLShader> baseShader = shaderSystem.lock()->getShader("base.glsl");
    std::shared_ptr<OpenGLShader> lightShader = shaderSystem.lock()->getShader("light.glsl");


    float vertices[] = {
        // positions               // normals           // texture coords
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
                                        
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
                                               
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
                                               
         0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
                                               
        -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
                                               
        -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,   0.0f, 1.0f
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
        { ShaderDataType::Float3, true  },
        { ShaderDataType::Float2, false }
    };

    std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(indices, (int)sizeof(indices));

    std::shared_ptr<OpenGLVertexArray> squareVertexArray = std::make_shared<OpenGLVertexArray>();
    squareVertexArray->addVertexBuffer(squareVertexBuffer);
    squareVertexArray->setIndexBuffer(indexBuffer);

    std::shared_ptr<OpenGLTexture2D> woodenBoxTexture2D = std::make_shared<OpenGLTexture2D>("Assets/Images/container.jpg");

    // cube
    EntityId cubeEntityId = registry.create();
    registry.addComponent<Cube>(cubeEntityId);
    Transform* cubeTransform = registry.addComponent<Transform>(cubeEntityId);
    RenderComponent* cubeRender = registry.addComponent<RenderComponent>(cubeEntityId);
    cubeRender->vao = squareVertexArray;
    cubeRender->texture = woodenBoxTexture2D;
    cubeRender->shader = baseShader;
    cubeRender->color = glm::vec3(1.0f, 0.5f, 0.31f);

    // light
    EntityId lightEntityId = registry.create();
    registry.addComponent<LightSourceComponent>(lightEntityId);
    Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
    lightTransform->position = glm::vec3(-5.f, 5.f, -5.f);
    lightTransform->scale = glm::vec3(.1f, .1f, .1f);

    RenderComponent* lightRender = registry.addComponent<RenderComponent>(lightEntityId);
    lightRender->vao = squareVertexArray;
    lightRender->shader = lightShader;
    lightRender->color = glm::vec3(1.f, 1.f, 1.f);

    // floor
    EntityId floorEntityId = registry.create();
    Transform* floorTransform = registry.addComponent<Transform>(floorEntityId);
    floorTransform->position = glm::vec3(0.f, -1.0f, 0.f);
    floorTransform->scale = glm::vec3(20.f, 0.1f, 20.f);
    RenderComponent* floorRender = registry.addComponent<RenderComponent>(floorEntityId);
    floorRender->vao = squareVertexArray;
    floorRender->shader = baseShader;
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
    glm::vec3 cameraPosition = glm::vec3();

    RegistryView<CameraComponent> cameraRegistryView(registry);
    for (const EntityId& entityId : cameraRegistryView)
    {
        if (const CameraComponent* cameraComponent = registry.getComponent<CameraComponent>(entityId))
        {
            view = cameraComponent->view;
            projection = cameraComponent->projection;
        }
        
        if (const Transform* transform = registry.getComponent<Transform>(entityId))
        {
            cameraPosition = transform->position;
        }
    }

    std::vector<glm::vec3> lightSources;
    RegistryView<Transform, LightSourceComponent> lightSourcesView(registry);
    for (const EntityId& entityId : lightSourcesView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        lightSources.push_back(transform->position);
    }

    RegistryView<Transform, Cube> cubesView(registry);
    for (const EntityId& entityId : cubesView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        transform->rotation *= glm::angleAxis(glm::radians(25.f * deltaTime), glm::vec3(1.f, 1.f, 0.f));
    }

    //ECSE_LOG(Log, "{}", 1.f/ deltaTime);

    int index = 0;
    RegistryView<Transform, RenderComponent> registryView(registry);
    for (const auto& entityId : registryView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        RenderComponent* render = registry.getComponent<RenderComponent>(entityId);
        std::shared_ptr<OpenGLShader> shader = render->shader;

        // set the shader program to be used.
        shader->use();
        if (render->texture != nullptr)
        {
            render->texture->bind(0);
            shader->setTextureSlot("inputTexture1", 0);
        }
        
        glm::mat4 modelMatrix = transform->calculate();
        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

        // set vertex uniforms
        shader->setFloatMatrix4("model", glm::value_ptr(modelMatrix));
        shader->setFloatMatrix3("normalMatrix", glm::value_ptr(normalMatrix));
        shader->setFloatMatrix4("view", glm::value_ptr(view));
        shader->setFloatMatrix4("projection", glm::value_ptr(projection));

        const glm::vec3& color = render->color;

        shader->setFloat3("color", color.x, color.y, color.z);
        shader->setFloat3("viewPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);

        // set fragment uniforms
        shader->setFloat3("material.ambient", render->materialAmbient.x, render->materialAmbient.y, render->materialAmbient.z);
        shader->setFloat3("material.diffuse", render->materialDiffuse.x, render->materialDiffuse.y, render->materialDiffuse.z);
        shader->setFloat3("material.specular", render->materialSpecular.x, render->materialSpecular.y, render->materialSpecular.z);
        shader->setFloat("material.shininess", render->shininess);

        shader->setFloat3("light.ambient", 0.2f, 0.2f, 0.2f);
        shader->setFloat3("light.diffuse", 0.5f, 0.5f, 0.5f);
        shader->setFloat3("light.ambient", 1.0f, 1.0f, 1.0f);

        if (lightSources.size() > 0)
        {
            const glm::vec3& lightSourcePosition = lightSources[0];
            shader->setFloat3("light.position", lightSourcePosition.x, lightSourcePosition.y, lightSourcePosition.z);
        }

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
