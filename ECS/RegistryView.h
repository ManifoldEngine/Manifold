#pragma once

#include "ECS.h"
#include "Entity.h"
#include "EntityRegistry.h"

// TODO: do not use STL for exposed classes.
// warning C4251: 'ECSEngine::EntityRegistry::ComponentStore': class 'std::vector<unsigned char,std::allocator<unsigned char>>' needs to have dll-interface to be used by clients of class 'ECSEngine::EntityRegistry'
#pragma warning(disable:4251)

namespace ECSEngine
{
    template<typename ...TComponents>
    class ECS_API RegistryView
    {
    public:
        RegistryView(EntityRegistry& registry)
            : m_registryPtr(&registry)
        {
            if (sizeof...(TComponents) == 0)
            {
                m_bisAll = true;
            }
            else
            {
                int componentIds[] = { 0, EntityRegistry::getComponentId<TComponents>() ... };
                for (size_t i = 1; i < (sizeof...(TComponents) + 1); ++i)
                {
                    m_componentMask.set(componentIds[i]);
                }
            }
        }

        struct Iterator
        {
            Iterator(
                EntityId InCurrentEntityId,
                EntityRegistry* InRegistryPtr,
                std::bitset<ECSEngine::MAX_COMPONENTS> InComponentMask,
                bool InIsAll
            );

            EntityId operator*() const;
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;
            Iterator& operator++();

        private:
            EntityId m_currentEntityId;
            EntityRegistry* m_registryPtr;
            std::bitset<ECSEngine::MAX_COMPONENTS> m_componentMask;
            bool m_bisAll = false;

            bool isValidIndex() const;
        };

        const Iterator begin() const
        {
            if (m_bisAll)
            {
                return Iterator(m_registryPtr, 0, m_componentMask, m_bisAll);
            }

            EntityId entityId = 0;
            for (; m_registryPtr->m_entities.size(); ++entityId)
            {
                if (!m_registryPtr->isValid(entityId))
                {
                    continue;
                }

                Entity& entity = m_registryPtr->m_entities[entityId];
                if (entity.HasComponents(m_componentMask))
                {
                    break;
                }
            }

            return Iterator(m_registryPtr, entityId, m_componentMask, m_bisAll);
        }

        const Iterator end() const
        {
            return Iterator(m_registryPtr, m_registryPtr->m_entities.size(), m_componentMask, m_bisAll);
        }

    private:
        EntityRegistry* m_registryPtr = nullptr;
        std::bitset<ECSEngine::MAX_COMPONENTS> m_componentMask;
        bool m_bisAll = false;
    };

    // ITERATOR BEGIN
    template<typename ...TComponents>
    inline RegistryView<TComponents...>::Iterator::Iterator(
        ECSEngine::EntityId InCurrentEntityId,
        EntityRegistry* InRegistryPtr,
        std::bitset<ECSEngine::MAX_COMPONENTS> InComponentMask,
        bool InIsAll
    ) :
        m_currentEntityId(InCurrentEntityId),
        m_registryPtr(InRegistryPtr),
        m_componentMask(InComponentMask),
        m_bisAll(InIsAll)
    {}

    template<typename ...TComponents>
    inline EntityId RegistryView<TComponents...>::Iterator::operator*() const
    {
        return m_registryPtr->m_entities[m_currentEntityId];
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
        } while (m_currentEntityId < m_registryPtr->m_entities.size() && !isValidIndex(m_currentEntityId));
        return *this;
    }

    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::isValidIndex() const
    {
        if (!m_registryPtr->isValid(m_currentEntityId))
        {
            return false;
        }

        const Entity& entity = m_registryPtr->m_entities[m_currentEntityId];
        return m_bisAll || entity.HasComponents(m_componentMask);
    }
    // ITERATOR END
}

#pragma warning(default:4251)