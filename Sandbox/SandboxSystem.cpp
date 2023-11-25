#include "SandboxSystem.h"
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Components/Transform.h>
#include <Core/Log.h>
#include <Core/FileSystem.h>

#include <Camera/CameraSystem.h>

#include <ECS/EntityRegistry.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/Light/PointLightComponent.h>
#include <RenderAPI/Light/SpotlightComponent.h>

#include <OpenGL/Render/OpenGLResourceSystem.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <Inputs/InputSystem.h>

#include "ShaderUtils.h"

#include <glm/glm.hpp>
#include <random>

using namespace ECSEngine;

struct Cube {};

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    size_t assetCount = 0;

    // load assets.
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
    std::vector<uint32_t> indices;
    for (int i = 0; i < verticesSize; ++i)
    {
        indices.push_back(i);
    }

    std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>();
    cubeMesh->id = assetCount++;
    for (int i = 0; i < verticesSize; i += 8)
    {
        Vertex vertex;
        vertex.position.x = vertices[i];
        vertex.position.y = vertices[i + 1];
        vertex.position.z = vertices[i + 2];

        vertex.normal.x = vertices[i + 3];
        vertex.normal.y = vertices[i + 4];
        vertex.normal.z = vertices[i + 5];

        vertex.textureCoordinate.x = vertices[i + 6];
        vertex.textureCoordinate.y = vertices[i + 7];
        cubeMesh->vertices.push_back(vertex);
    }

    cubeMesh->indices = indices;

    std::filesystem::path rootPath;
    if (!FileSystem::tryGetRootPath(rootPath))
    {
        ECSE_LOG_ERROR(Log, "Could not get root path.");
        return;
    }

    std::filesystem::path enginePath;
    if (!FileSystem::tryGetEnginePath(enginePath))
    {
        ECSE_LOG_ERROR(Log, "Could not get root path.");
        return;
    }

    std::shared_ptr<Texture> boxTexture = std::make_shared<Texture>();
    boxTexture->id = assetCount++;
    boxTexture->path = std::filesystem::path(rootPath).append("Sandbox/Assets/Images/container2.png").string();

    std::shared_ptr<Texture> boxFrameTexture = std::make_shared<Texture>();
    boxFrameTexture->id = assetCount++;
    boxFrameTexture->path = std::filesystem::path(rootPath).append("Sandbox/Assets/Images/container2_specular.png").string();

    std::shared_ptr<Texture> floorTexture = std::make_shared<Texture>();
    floorTexture->id = assetCount++;
    floorTexture->path = std::filesystem::path(rootPath).append("Sandbox/Assets/Images/floor.png").string();

    std::shared_ptr<Shader> baseLitShader = std::make_shared<Shader>();
    baseLitShader->id = assetCount++;
    parseShaderSourceFileFromPath(std::filesystem::path(enginePath).append("Assets/Shaders/baseLit.glsl"), baseLitShader->name, baseLitShader->vertexSource, baseLitShader->fragmentSource);

    std::shared_ptr<Shader> flatColorShader = std::make_shared<Shader>();
    flatColorShader->id = assetCount++;
    parseShaderSourceFileFromPath(std::filesystem::path(enginePath).append("Assets/Shaders/flatColor.glsl"), flatColorShader->name, flatColorShader->vertexSource, flatColorShader->fragmentSource);

    std::shared_ptr<Material> boxMaterial = std::make_shared<Material>();
    boxMaterial->id = assetCount++;
    boxMaterial->shader = baseLitShader;
    boxMaterial->diffuse = boxTexture;
    boxMaterial->specular = boxFrameTexture;

    std::shared_ptr<Material> lightMaterial = std::make_shared<Material>();
    lightMaterial->id = assetCount++;
    lightMaterial->shader = flatColorShader;
    lightMaterial->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // white

    std::shared_ptr<Material> floorMaterial = std::make_shared<Material>();
    floorMaterial->id = assetCount++;
    floorMaterial->shader = baseLitShader;
    floorMaterial->diffuse = floorTexture;
    floorMaterial->specular = floorTexture;
    
    // initialize OpenGL
    std::shared_ptr<OpenGLResourceSystem> resourceSystem = systemContainer.initializeDependency<OpenGLResourceSystem>().lock();
    resourceSystem->onMeshLoaded(cubeMesh);
    resourceSystem->onTextureLoaded(boxTexture);
    resourceSystem->onTextureLoaded(boxFrameTexture);
    resourceSystem->onTextureLoaded(floorTexture);
    resourceSystem->onShaderLoaded(baseLitShader);
    resourceSystem->onShaderLoaded(flatColorShader);

    systemContainer.initializeDependency<OpenGLRenderSystem>();

    // initialize entities.
    {
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

            MeshComponent* cubeMeshComponent = registry.addComponent<MeshComponent>(cubeEntityId);
            cubeMeshComponent->mesh = cubeMesh;
            cubeMeshComponent->material = boxMaterial;
        }
    }

    {
        // light
        EntityId lightEntityId = registry.create();
        PointLightComponent* pointLigh = registry.addComponent<PointLightComponent>(lightEntityId);
        pointLigh->ambient = glm::vec3(.01f, .01f, .01f);
        pointLigh->diffuse = glm::vec3(.75f, .75f, .85f);

        Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
        lightTransform->position = glm::vec3(-1.f, 1.f, -1.f);
        lightTransform->scale = glm::vec3(.1f, .1f, .1f);

        MeshComponent* lightMeshComponent = registry.addComponent<MeshComponent>(lightEntityId);
        lightMeshComponent->mesh = cubeMesh;
        lightMeshComponent->material = lightMaterial;
    }

    {
        // floor
        EntityId floorEntityId = registry.create();
        
        Transform* floorTransform = registry.addComponent<Transform>(floorEntityId);
        floorTransform->position = glm::vec3(0.f, -1.0f, 0.f);
        floorTransform->scale = glm::vec3(200.f, 0.1f, 200.f);

        MeshComponent* floorMeshComponent = registry.addComponent<MeshComponent>(floorEntityId);
        floorMeshComponent->mesh = cubeMesh;
        floorMeshComponent->material = floorMaterial;

    }

    m_cameraSystem = systemContainer.initializeDependency<CameraSystem>();
    Transform* cameraTransform = m_cameraSystem.lock()->getCameraTransform(registry);
    if (cameraTransform != nullptr)
    {
        cameraTransform->position = glm::vec3(12.67f, 3.04f, -9.13f);
        cameraTransform->rotation = glm::normalize(glm::quat(0.87f, 0.09f, -0.46f, 0.04f));
    }

    m_inputSystem = systemContainer.initializeDependency<InputSystem>();
    if (!m_inputSystem.expired())
    {
        std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
        inputSystem->addAction(LOCAL_USERID, "ToggleFlashlight");
        inputSystem->addBinding(LOCAL_USERID, "ToggleFlashlight", "F");
        onActionEventHandle = inputSystem->onActionEvent.subscribe(std::bind(&SandboxSystem::onActionEvent, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void SandboxSystem::onDeinitialize(EntityRegistry& registry)
{
    if (!m_inputSystem.expired())
    {
        std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
        inputSystem->onActionEvent.unsubscribe(onActionEventHandle);
    }
}

void SandboxSystem::onActionEvent(uint32_t userId, const InputAction& action)
{
    if (action.isPressed)
    {
        m_isFlashlightOn = !m_isFlashlightOn;
    }
}

void SandboxSystem::tick(float deltaTime, EntityRegistry& registry)
{
    if (m_cameraSystem.expired())
    {
        return;
    }

    std::shared_ptr<CameraSystem> cameraSystem = m_cameraSystem.lock();
    Transform* cameraTransform = cameraSystem->getCameraTransform(registry);
    if (cameraTransform == nullptr)
    {
        return;
    }

    Transform* spotlightTransform = registry.getComponent<Transform>(m_spotlightEntityId);
    if (spotlightTransform == nullptr)
    {
        return;
    }

    spotlightTransform->position = cameraTransform->position;
    spotlightTransform->rotation = cameraTransform->rotation;

    const bool hasSpotlight = registry.hasComponent<SpotlightComponent>(m_spotlightEntityId);
    if (m_isFlashlightOn && !hasSpotlight)
    {
        // create spotlight
        SpotlightComponent* spotlight = registry.addComponent<SpotlightComponent>(m_spotlightEntityId);
        spotlight->ambient = glm::vec3(.01f, .01f, .01f);
        spotlight->diffuse = glm::vec3(.75f, .75f, .85f);
    }
    else if (!m_isFlashlightOn && hasSpotlight)
    {
        // remove spotlight
        registry.removeComponent<SpotlightComponent>(m_spotlightEntityId);
    }
}
