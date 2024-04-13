#include "TransformSystem.h"
#include <Core/ManiAssert.h>

using namespace Mani;

std::string_view TransformSystem::getName() const
{
    return "TransformSystem";
}

bool TransformSystem::shouldTick(EntityRegistry& registry) const
{
    return true;
}

ETickGroup TransformSystem::getTickGroup() const
{
    return ETickGroup::PostTick;
}

void TransformSystem::tick(float deltaTime, EntityRegistry& registry)
{
    m_processedTransforms.clear();
    m_childrenMap.clear();

    RegistryView<Transform> transformView(registry);
    for (const auto& entityId : transformView)
    {
        {
            Transform* transform = registry.getComponent<Transform>(entityId);
            if (!transform->hasParent())
            {
                // no parent, no worries.
                m_processedTransforms.insert(entityId);
                continue;
            }

            if (!m_processedTransforms.contains(transform->parentId))
            {
                // parent was not processed yet, keep it for later
                m_childrenMap[transform->parentId].insert(entityId);
                continue;
            }
        }

        updateTransformsAndTheirChildrenRecursively(registry, entityId);
    }
}

void TransformSystem::updateTransform(EntityRegistry& registry, EntityId entityId)
{
    Transform* transform = registry.getComponent<Transform>(entityId);
    MANI_ASSERT(transform != nullptr, "entity must have a transform component");

    Transform* parentTransform = registry.getComponent<Transform>(transform->parentId);
    MANI_ASSERT(parentTransform != nullptr, "Parent entity must have a transform component");

    transform->position = transform->localPosition;
    transform->rotation = transform->localRotation;
    transform->scale = transform->localScale;

    glm::mat4 parentMat4 = parentTransform->calculateModelMatrix();
    glm::mat4 mat4 = transform->calculateModelMatrix();
    glm::mat4 result = parentMat4 * mat4;

    transform->position = result[3];

    transform->rotation *= parentTransform->rotation;

    transform->scale.x = glm::length(glm::vec3(result[0]));
    transform->scale.y = glm::length(glm::vec3(result[1]));
    transform->scale.z = glm::length(glm::vec3(result[2]));
}

void TransformSystem::updateTransformsAndTheirChildrenRecursively(EntityRegistry& registry, EntityId entityId)
{
    if (!isValid(entityId))
    {
        return;
    }

    MANI_ASSERT(!m_processedTransforms.contains(entityId), "Transform cycle detected!");

    updateTransform(registry, entityId);
    m_processedTransforms.insert(entityId);

    if (m_childrenMap.contains(entityId))
    {
        for (const auto& childEntityId : m_childrenMap[entityId])
        {
            updateTransformsAndTheirChildrenRecursively(registry, childEntityId);
        }
    }

    m_childrenMap.erase(entityId);
}
