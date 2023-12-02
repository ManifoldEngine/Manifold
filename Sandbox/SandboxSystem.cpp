#include "SandboxSystem.h"
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Components/Transform.h>
#include <Core/Log.h>
#include <Core/FileSystem.h>

#include <Camera/CameraSystem.h>

#include <ECS/EntityRegistry.h>
#include <ECS/RegistryView.h>

#include <Assets/AssetSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/Light/PointLightComponent.h>
#include <RenderAPI/Light/DirectionalLightComponent.h>
#include <RenderAPI/Light/SpotlightComponent.h>

#include <OpenGL/Render/OpenGLResourceSystem.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <Inputs/InputSystem.h>

#include <glm/glm.hpp>

// srand
#include <stdlib.h>
#include <time.h>
#include <glm/gtx/rotate_vector.hpp>

using namespace ECSEngine;

struct PhysicsComponent
{
    glm::vec3 velocity;
    float timeAcc = 0.0f;
    float changeDirectionInterval = 0.0f;
};

struct CactusLight 
{
    EntityId ownerId;
};

void SandboxSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();

    std::shared_ptr<Mesh> cubeMesh = hardcodeCubeMesh(systemContainer);
    std::shared_ptr<Mesh> cactusMesh = assetSystem->loadJsonAsset<Mesh>("Sandbox/Assets/Meshes/Cylinder.004.mesh");
    std::shared_ptr<Material> cactusMaterial = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/cactus.material");
    std::shared_ptr<Material> lightMaterial = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/light.material");
    std::shared_ptr<Material> floorMaterial = assetSystem->loadJsonAsset<Material>("Sandbox/Assets/Materials/floor.material");

    // initialize entities.
    {
        for (int i = 0; i < 10; ++i)
        {
            // cube
            EntityId cactusEntityId = registry.create();
            Transform* cactusTransform = registry.addComponent<Transform>(cactusEntityId);

            const glm::vec3 position = glm::vec3(i * 5.f, 0.0f, 0.0f);

            cactusTransform->position = position;
            cactusTransform->rotation = glm::angleAxis(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));

            MeshComponent* cactusMeshComponent = registry.addComponent<MeshComponent>(cactusEntityId);
            cactusMeshComponent->mesh = cactusMesh;
            cactusMeshComponent->material = cactusMaterial;

            PhysicsComponent* physics = registry.addComponent<PhysicsComponent>(cactusEntityId);
            physics->velocity = glm::vec3(0.0f, 0.0f, -1.0f);
            physics->changeDirectionInterval = (50 + (std::rand() / ((RAND_MAX + 1u) / 100))) / 100.f;

            {
                // light
                EntityId lightEntityId = registry.create();
                CactusLight* cactusLight = registry.addComponent<CactusLight>(lightEntityId);
                cactusLight->ownerId = cactusEntityId;

                SpotlightComponent* spotlight = registry.addComponent<SpotlightComponent>(lightEntityId);
                spotlight->ambient = glm::vec3(.01f, .01f, .01f);
                spotlight->diffuse = glm::vec3(.75f, .75f, .85f);

                Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
                lightTransform->position = position + glm::vec3(0.0f, 5.f, 0.0f);
                lightTransform->rotation = glm::quatLookAt(physics->velocity, glm::vec3(0.0f, 1.0f, 0.0f));

                MeshComponent* meshComponent = registry.addComponent<MeshComponent>(lightEntityId);
                meshComponent->mesh = cubeMesh;
                meshComponent->material = lightMaterial;
            }
        }
    }

    {
        // directional light
        EntityId lightEntityId = registry.create();
        DirectionalLightComponent* directionalLigh = registry.addComponent<DirectionalLightComponent>(lightEntityId);
        directionalLigh->ambient = glm::vec3(.16f, .16f, .16f);
        directionalLigh->diffuse = glm::vec3(.75f, .75f, .75f);
        directionalLigh->direction = glm::vec3(-0.2f, -1.0f, -0.2f);

        Transform* lightTransform = registry.addComponent<Transform>(lightEntityId);
        lightTransform->position = glm::vec3(-1.f, 4.f, -1.f);
        lightTransform->scale = glm::vec3(.1f, .1f, .1f);
    }

    {
        // floor
        EntityId floorEntityId = registry.create();
        
        Transform* floorTransform = registry.addComponent<Transform>(floorEntityId);
        floorTransform->position = glm::vec3(0.f, 0.0f, 0.f);
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

std::shared_ptr<Mesh> SandboxSystem::hardcodeCubeMesh(ECSEngine::SystemContainer& systemContainer)
{
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
    cubeMesh->name = "CubeMesh";

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

    systemContainer.initializeDependency<OpenGLRenderSystem>();
    std::shared_ptr<OpenGLResourceSystem> resourceSystem = systemContainer.initializeDependency<OpenGLResourceSystem>().lock();
    resourceSystem->onMeshLoaded(cubeMesh);

    return cubeMesh;
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

    RegistryView<Transform, PhysicsComponent> cactusView(registry);
    for (const EntityId entityId : cactusView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        PhysicsComponent* physics = registry.getComponent<PhysicsComponent>(entityId);

        physics->timeAcc += deltaTime;

        transform->position += physics->velocity * 5.f * deltaTime;

        if (physics->timeAcc >= physics->changeDirectionInterval)
        {
            const bool isLeft = 1 + std::rand() / ((RAND_MAX + 1u) / 100) >= 50;

            const float angle = (isLeft ? -1.f : 1.f) * glm::radians(45.f);

            physics->velocity = glm::rotate(physics->velocity, angle, glm::vec3(0.0f, 1.0f, 0.0f));


            glm::quat quatYaw = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat quatPitch(1.0, 0.0, 0.0, 0.0);

            transform->rotation = quatYaw * transform->rotation * quatPitch;

            physics->timeAcc = 0.0f;
        }
    }

    RegistryView<Transform, CactusLight> cactusLightView(registry);
    for (const EntityId entityId : cactusLightView)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        CactusLight* cactusLight = registry.getComponent<CactusLight>(entityId);

        for (const EntityId cactusEntityId : cactusView)
        {
            if (cactusEntityId != cactusLight->ownerId)
            {
                continue;
            }

            Transform* cactusTransform = registry.getComponent<Transform>(cactusEntityId);
            PhysicsComponent* physics = registry.getComponent<PhysicsComponent>(cactusEntityId);

            transform->position = cactusTransform->position + glm::vec3(0.0f, 5.f, 0.0f);
            glm::quat yaw = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat pitch = glm::angleAxis(glm::radians(45.f), glm::vec3(1.0f, 0.0f, 0.0f));
            transform->rotation =  yaw * glm::quatLookAt(physics->velocity, glm::vec3(0.0f, 1.0f, 0.0f)) * pitch;
        }
    }
}