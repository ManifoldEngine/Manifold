#pragma once

#include "ECS.h"
#include "Internals/EntityContainer.h"
#include "EntityRegistry.h"
#include "Entity.h"
#include "Bitset.h"

namespace Mani
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
            : m_registry(&registry)
        {
            if (sizeof...(TComponents) == 0)
            {
                m_bisAll = true;
            }
            else
            {
                // The compiler complains if we allocate an array of size 0. So, we start the array with 0.
               const ComponentId componentIds[] = { 0, registry.getComponentId<TComponents>() ... };
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
                const EntityRegistry* inRegistry,
                EntityId inCurrentEntityId,
                Bitset<Mani::MAX_COMPONENTS> inComponentMask,
                bool inIsAll
            );

            EntityId operator*() const;
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;
            Iterator& operator++();

        private:
            EntityId m_currentEntityId;
            const EntityRegistry* m_registry;
            Bitset<Mani::MAX_COMPONENTS> m_componentMask;
            bool m_isAll = false;

            bool isValidIndex() const;
        };

        const Iterator begin() const
        {
            if (m_bisAll)
            {
                return Iterator(m_registry, 0, m_componentMask, m_bisAll);
            }

            EntityId entityId = 0;
            for (; entityId < m_registry->size(); ++entityId)
            {
                if (!m_registry->isValid(entityId))
                {
                    continue;
                }

                if (m_registry->getEntity(entityId)->hasComponents(m_componentMask))
                {
                    break;
                }
            }

            return Iterator(m_registry, entityId, m_componentMask, m_bisAll);
        }

        const Iterator end() const
        {
            return Iterator(m_registry, m_registry->size(), m_componentMask, m_bisAll);
        }

    private:
        const EntityRegistry* m_registry = nullptr;
        Bitset<Mani::MAX_COMPONENTS> m_componentMask;
        bool m_bisAll = false;
    };

    // ITERATOR BEGIN
    template<typename ...TComponents>
    inline RegistryView<TComponents...>::Iterator::Iterator(
        const EntityRegistry* inRegistry,
        Mani::EntityId inCurrentEntityId,
        Bitset<Mani::MAX_COMPONENTS> inComponentMask,
        bool inIsAll
    ) :
        m_currentEntityId(inCurrentEntityId),
        m_registry(inRegistry),
        m_componentMask(inComponentMask),
        m_isAll(inIsAll)
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
        } while (m_currentEntityId < m_registry->size() && !isValidIndex());
        return *this;
    }

    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::isValidIndex() const
    {
        if (!m_registry->isValid(m_currentEntityId))
        {
            return false;
        }

        const Entity* entity = m_registry->getEntity(m_currentEntityId);
        return m_isAll || entity->hasComponents(m_componentMask);
    }
    // ITERATOR END
}