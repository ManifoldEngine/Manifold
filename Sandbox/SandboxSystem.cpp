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
#include <random>

using namespace ECSEngine;

struct RenderComponent
{
    std::shared_ptr<OpenGLVertexArray> vao;
    std::shared_ptr<OpenGLShader> shader;

    std::shared_ptr<OpenGLTexture2D> materialDiffuseMap = nullptr;
    std::shared_ptr<OpenGLTexture2D> materialSpecularMap = nullptr;
    
    float shininess = 64.0f;
};

struct Cube {};

struct LightComponent 
{
    glm::vec3 color = glm::vec3(1.0f);
};

struct DirectionalLightComponent
{
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);

    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);
};

struct PointLightComponent
{
    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotlightComponent
{
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    float cutOff = glm::cos(glm::radians(12.5f));
    float outterCutOff = glm::cos(glm::radians(17.5f));

    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.0f);
};

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::weak_ptr<OpenGLShaderSystem> shaderSystem = Application::get().getSystemContainer().initializeDependency<OpenGLShaderSystem>();
    if (shaderSystem.expired())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the layer.");
        return;
    }

    std::shared_ptr<OpenGLShader> baseShader = shaderSystem.lock()->getShader("baseLit.glsl");
    std::shared_ptr<OpenGLShader> lightShader = shaderSystem.lock()->getShader("flatColor.glsl");

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
    squareVertexBuffer->layout = 
    {
        { ShaderDataType::Float3, false },
        { ShaderDataType::Float3, true  },
        { ShaderDataType::Float2, false }
    };

    std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(indices, (int)sizeof(indices));

    std::shared_ptr<OpenGLVertexArray> squareVertexArray = std::make_shared<OpenGLVertexArray>();
    squareVertexArray->addVertexBuffer(squareVertexBuffer);
    squareVertexArray->setIndexBuffer(indexBuffer);

    std::shared_ptr<OpenGLTexture2D> woodenBoxTexture2D = std::make_shared<OpenGLTexture2D>("Assets/Images/container2.png");
    std::shared_ptr<OpenGLTexture2D> woodenBoxSpecularTexture2D = std::make_shared<OpenGLTexture2D>("Assets/Images/container2_specular.png");
    

    glm::vec3 lowerBound = glm::vec3(-4.f, 0.f, -5.f);
    glm::vec3 upperBound = glm::vec3(4.f, 1.f, 4.f);
    std::mt19937 gen(UINT32_MAX);
    std::uniform_real_distribution<float> randomX(lowerBound.x, upperBound.x);
    std::uniform_real_distribution<float> randomY(lowerBound.y, upperBound.y);
    std::uniform_real_distribution<float> randomZ(lowerBound.z, upperBound.z);
    for (int i = 0; i < 10; ++i)
    {
        // cube
        EntityId cubeEntityId = registry.create();
        registry.addComponent<Cube>(cubeEntityId);
        Transform* cubeTransform = registry.addComponent<Transform>(cubeEntityId);
        
        const glm::vec3 position = glm::vec3(
            randomX(gen),
            randomY(gen),
            randomZ(gen)
        );

        cubeTransform->position = position;

        RenderComponent* cubeRender = registry.addComponent<RenderComponent>(cubeEntityId);
        cubeRender->vao = squareVertexArray;
        cubeRender->shader = baseShader;
        cubeRender->materialDiffuseMap = woodenBoxTexture2D;
        cubeRender->materialSpecularMap = woodenBoxSpecularTexture2D;
        cubeRender->shininess = 64.f;
    }
    //{
    //    // light
    //    EntityId lightEntityId = registry.create();
    //    registry.addComponent<LightComponent>(lightEntityId);
    //    DirectionalLightComponent* lightComponent = registry.addComponent<DirectionalLightComponent>(lightEntityId);
    //    lightComponent->direction = glm::vec3(-0.2f, -1.0f, 0.1f);

    //    Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
    //    lightTransform->position = glm::vec3(0.f, 6.f, 0.f);
    //    lightTransform->scale = glm::vec3(.1f, .1f, .1f);
    //    RenderComponent* lightRender = registry.addComponent<RenderComponent>(lightEntityId);
    //    lightRender->vao = squareVertexArray;
    //    lightRender->shader = lightShader;
    //}
    {
        // light
        EntityId lightEntityId = registry.create();
        registry.addComponent<LightComponent>(lightEntityId);
        PointLightComponent* pointLigh = registry.addComponent<PointLightComponent>(lightEntityId);
        pointLigh->ambient = glm::vec3(.01f, .01f, .01f);
        pointLigh->diffuse = glm::vec3(.75f, .75f, .85f);

        Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
        lightTransform->position = glm::vec3(-1.f, 1.f, -1.f);
        lightTransform->scale = glm::vec3(.1f, .1f, .1f);
        RenderComponent* lightRender = registry.addComponent<RenderComponent>(lightEntityId);
        lightRender->vao = squareVertexArray;
        lightRender->shader = lightShader;
    }
    //{
    //    // light
    //    EntityId lightEntityId = registry.create();
    //    registry.addComponent<LightComponent>(lightEntityId);
    //    registry.addComponent<PointLightComponent>(lightEntityId);
    //    Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
    //    lightTransform->position = glm::vec3(-10.f, 1.f, -10.f);
    //    lightTransform->scale = glm::vec3(.1f, .1f, .1f);
    //    RenderComponent* lightRender = registry.addComponent<RenderComponent>(lightEntityId);
    //    lightRender->vao = squareVertexArray;
    //    lightRender->shader = lightShader;
    //}
  

    // floor
    EntityId floorEntityId = registry.create();
    Transform* floorTransform = registry.addComponent<Transform>(floorEntityId);
    floorTransform->position = glm::vec3(0.f, -1.0f, 0.f);
    floorTransform->scale = glm::vec3(20.f, 0.1f, 20.f);
    RenderComponent* floorRender = registry.addComponent<RenderComponent>(floorEntityId);
    floorRender->vao = squareVertexArray;
    floorRender->shader = baseShader;
    
    Transform* cameraTransform = systemContainer.initializeDependency<CameraSystem>().lock()->getCameraTransform(registry);
    if (cameraTransform != nullptr)
    {
        cameraTransform->position = glm::vec3(12.67f, 3.04f, -9.13f);
        cameraTransform->rotation = glm::normalize(glm::quat(0.87f, 0.09f, -0.46f, 0.04f));
    }

    //{
    //    // light
    //    EntityId lightEntityId = registry.create();
    //    registry.addComponent<LightComponent>(lightEntityId);
    //    SpotlightComponent* spotlight = registry.addComponent<SpotlightComponent>(lightEntityId);
    //    spotlight->ambient = glm::vec3(.001f, .001f, .001f);
    //    spotlight->diffuse = glm::vec3(.85f, .85f, .85f);
    //    Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
    //    if (cameraTransform != nullptr)
    //    {
    //        lightTransform->position = cameraTransform->position;
    //    }
    //}
}

void SandboxSystem::onDeinitialize(EntityRegistry& registry)
{
}

void SandboxSystem::tick(float deltaTime, EntityRegistry& registry)
{
    glEnable(GL_DEPTH_TEST);

    // setting color state.
    glClearColor(.1f, .1f, .1f, 1.f);
    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 cameraPosition = glm::vec3();
    glm::vec3 cameraForward = glm::vec3();
    
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
            cameraForward = transform->forward();
        }
    }
    
    RegistryView<Transform, Cube> cubesView(registry);
    for (const EntityId& entityId : cubesView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        transform->rotation *= glm::angleAxis(glm::radians(25.f * deltaTime), glm::vec3(1.f, 1.f, 0.f));
    }
    
    // ECSE_LOG(Log, "{}", 1.f/ deltaTime);

    int index = 0;
    RegistryView<Transform, RenderComponent> registryView(registry);
    for (const auto& entityId : registryView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        RenderComponent* render = registry.getComponent<RenderComponent>(entityId);
        std::shared_ptr<OpenGLShader> shader = render->shader;
        
        // set the shader program to be used.
        if (shader == nullptr)
        {
            ECSE_LOG_ERROR(Log, "Shader was not loaded or compiled.");
            return;
        }
        
        shader->use();
        
        glm::mat4 modelMatrix = transform->calculate();
        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
        
        // set vertex uniforms
        shader->setFloatMatrix4("model", glm::value_ptr(modelMatrix));
        shader->setFloatMatrix3("normalMatrix", glm::value_ptr(normalMatrix));
        shader->setFloatMatrix4("view", glm::value_ptr(view));
        shader->setFloatMatrix4("projection", glm::value_ptr(projection));

        // set fragment uniforms
        shader->setFloat3("viewPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        
        // set material
        if (render->materialDiffuseMap != nullptr)
        {
            render->materialDiffuseMap->bind(1);
            shader->setTextureSlot("material.diffuseMap", 1);
        }

        if (render->materialSpecularMap != nullptr)
        {
            render->materialSpecularMap->bind(2);
            shader->setTextureSlot("material.specularMap", 2);
        }

        shader->setFloat("material.shininess", render->shininess);

        // set lights
        int directionalLightIndex = 0;
        int pointLightIndex = 0;
        int spotlightIndex = 0;
        RegistryView<Transform, LightComponent> lightSourcesView(registry);
        for (const EntityId& entityId : lightSourcesView)
        {
            if (registry.hasComponent<DirectionalLightComponent>(entityId))
            {
                DirectionalLightComponent* light = registry.getComponent<DirectionalLightComponent>(entityId);
                
                std::string directionalLightArray = std::format("directionalLights[{}]", directionalLightIndex);
                shader->setFloat3(std::format("{}.direction", directionalLightArray).c_str(), light->direction.x, light->direction.y, light->direction.z);
                shader->setFloat3(std::format("{}.ambient", directionalLightArray).c_str(), light->ambient.x, light->ambient.y, light->ambient.z);
                shader->setFloat3(std::format("{}.diffuse", directionalLightArray).c_str(), light->diffuse.x, light->diffuse.y, light->diffuse.z);
                shader->setFloat3(std::format("{}.specular", directionalLightArray).c_str(), light->specular.x, light->specular.x, light->specular.x);
                directionalLightIndex++;
            }

            if (registry.hasComponent<PointLightComponent>(entityId) && registry.hasComponent<Transform>(entityId))
            {
                Transform* transform = registry.getComponent<Transform>(entityId);
                PointLightComponent* light = registry.getComponent<PointLightComponent>(entityId);

                std::string pointLightArray = std::format("pointLights[{}]", pointLightIndex);
                shader->setFloat3(std::format("{}.position", pointLightArray).c_str(), transform->position.x, transform->position.y, transform->position.z);

                shader->setFloat3(std::format("{}.ambient", pointLightArray).c_str(), light->ambient.x, light->ambient.y, light->ambient.z);
                shader->setFloat3(std::format("{}.diffuse", pointLightArray).c_str(), light->diffuse.x, light->diffuse.y, light->diffuse.z);
                shader->setFloat3(std::format("{}.specular", pointLightArray).c_str(), light->specular.x, light->specular.x, light->specular.x);

                shader->setFloat(std::format("{}.constant", pointLightArray).c_str(), light->constant);
                shader->setFloat(std::format("{}.linear", pointLightArray).c_str(), light->linear);
                shader->setFloat(std::format("{}.quadratic", pointLightArray).c_str(), light->quadratic);

                pointLightIndex++;
            }

            if (registry.hasComponent<SpotlightComponent>(entityId) && registry.hasComponent<Transform>(entityId))
            {
                Transform* transform = registry.getComponent<Transform>(entityId);
                SpotlightComponent* light = registry.getComponent<SpotlightComponent>(entityId);

                std::string spotlightsArray = std::format("spotlights[{}]", spotlightIndex);
                shader->setFloat3(std::format("{}.position", spotlightsArray).c_str(), cameraPosition.x, cameraPosition.y, cameraPosition.z);
                shader->setFloat3(std::format("{}.direction", spotlightsArray).c_str(), cameraForward.x, cameraForward.y, cameraForward.z);
                shader->setFloat(std::format("{}.cutOff", spotlightsArray).c_str(), light->cutOff);
                shader->setFloat(std::format("{}.outterCutOff", spotlightsArray).c_str(), light->outterCutOff);

                shader->setFloat3(std::format("{}.ambient", spotlightsArray).c_str(), light->ambient.x, light->ambient.y, light->ambient.z);
                shader->setFloat3(std::format("{}.diffuse", spotlightsArray).c_str(), light->diffuse.x, light->diffuse.y, light->diffuse.z);
                shader->setFloat3(std::format("{}.specular", spotlightsArray).c_str(), light->specular.x, light->specular.x, light->specular.x);

                spotlightIndex++;
            }
        }

        shader->setInt("directionalLightsCount", directionalLightIndex);
        shader->setInt("pointLightsCount", pointLightIndex);
        shader->setInt("spotlightsCount", spotlightIndex);

        if (const LightComponent* lightComponent = registry.getComponent<LightComponent>(entityId))
        {
            shader->setFloat3("color", lightComponent->color.x, lightComponent->color.y, lightComponent->color.z);
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

        if (render->materialDiffuseMap != nullptr)
        {
            render->materialDiffuseMap->unbind();
        }

        if (render->materialSpecularMap != nullptr)
        {
            render->materialSpecularMap->unbind();
        }
    }
}
