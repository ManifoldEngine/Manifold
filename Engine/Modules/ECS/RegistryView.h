#pragma once

#include "ECS.h"
#include "Internals/EntityContainer.h"
#include "EntityRegistry.h"
#include "Entity.h"
#include <bitset>

namespace ECSEngine
{
    /*
     * Allows a client to iterate over a view of entities with a specified set of components
     */
    template<typename ...TComponents>
    class RegistryView
    {
    public:
        RegistryView() = default;

        RegistryView(const EntityRegistry& registry)
            : m_pRegistry(&registry)
        {
            if (sizeof...(TComponents) == 0)
            {
                m_bisAll = true;
            }
            else
            {
                // The compiler complains if we allocate an array of size 0. So, we start the array with 0.
               const ComponentId componentIds[] = { 0, EntityRegistry::getComponentId<TComponents>() ... };
                for (size_t i = 1; i < (sizeof...(TComponents) + 1); ++i)
                {
                    m_componentMask.set(componentIds[i]);
                }
            }
        }

        struct Iterator
        {
            Iterator() = default;

            Iterator(
                const EntityRegistry* pInRegistry,
                EntityId inCurrentEntityId,
                std::bitset<ECSEngine::MAX_COMPONENTS> inComponentMask,
                bool bInIsAll
            );

            EntityId operator*() const;
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;
            Iterator& operator++();

        private:
            EntityId m_currentEntityId;
            const EntityRegistry* m_pRegistry;
            std::bitset<ECSEngine::MAX_COMPONENTS> m_componentMask;
            bool m_bIsAll = false;

            bool isValidIndex() const;
        };

        const Iterator begin() const
        {
            if (m_bisAll)
            {
                return Iterator(m_pRegistry, 0, m_componentMask, m_bisAll);
            }

            EntityId entityId = 0;
            for (; entityId < m_pRegistry->size(); ++entityId)
            {
                if (!m_pRegistry->isValid(entityId))
                {
                    continue;
                }

                if (m_pRegistry->getEntity(entityId)->hasComponents(m_componentMask))
                {
                    break;
                }
            }

            return Iterator(m_pRegistry, entityId, m_componentMask, m_bisAll);
        }

        const Iterator end() const
        {
            return Iterator(m_pRegistry, m_pRegistry->size(), m_componentMask, m_bisAll);
        }

    private:
        const EntityRegistry* m_pRegistry = nullptr;
        std::bitset<ECSEngine::MAX_COMPONENTS> m_componentMask;
        bool m_bisAll = false;
    };

    // ITERATOR BEGIN
    template<typename ...TComponents>
    inline RegistryView<TComponents...>::Iterator::Iterator(
        const EntityRegistry* pInRegistry,
        ECSEngine::EntityId inCurrentEntityId,
        std::bitset<ECSEngine::MAX_COMPONENTS> inComponentMask,
        bool InIsAll
    ) :
        m_currentEntityId(inCurrentEntityId),
        m_pRegistry(pInRegistry),
        m_componentMask(inComponentMask),
        m_bIsAll(InIsAll)
    {}

    template<typename ...TComponents>
    inline EntityId RegistryView<TComponents...>::Iterator::operator*() const
    {
        return m_currentEntityId;
    }
    
    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::operator==(const Iterator& other) const
    {
        return m_currentEntityId == other.m_currentEntityId;
    }
    
    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::operator!=(const Iterator& other) const
    {
        return m_currentEntityId != other.m_currentEntityId;
    }

    template<typename ...TComponents>
    inline RegistryView<TComponents...>::Iterator& RegistryView<TComponents...>::Iterator::operator++()
    {
        do
        {
            m_currentEntityId++;
        } while (m_currentEntityId < m_pRegistry->size() && !isValidIndex());
        return *this;
    }

    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::isValidIndex() const
    {
        if (!m_pRegistry->isValid(m_currentEntityId))
        {
            return false;
        }

        const Entity* entity = m_pRegistry->getEntity(m_currentEntityId);
        return m_bIsAll || entity->hasComponents(m_componentMask);
    }
    // ITERATOR END
}