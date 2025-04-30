#pragma once

#include "Registry.h"
#include "Entity.h"
#include "Bitset.h"
#include <ManiMaths/Maths.h>
#include <cassert>

namespace Mani
{
    namespace ECS
    {
        /*
         * Allows a client to iterate over a view of entities with a specified set of components
         */
        template<typename ...TComponents>
        class View
        {
        public:
            View() = default;

            View(const Registry& registry)
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
                    const Registry* inRegistry,
                    ECS::EntityId inCurrentEntityId,
                    Bitset<Mani::ECS::MAX_COMPONENTS> inComponentMask,
                    bool inIsAll
                );

                ECS::EntityId operator*() const;
                bool operator==(const Iterator& other) const;
                bool operator!=(const Iterator& other) const;
                bool operator>(const Iterator& other) const;
                bool operator<(const Iterator& other) const;
                bool operator>=(const Iterator& other) const;
                bool operator<=(const Iterator& other) const;
                Iterator& operator++();

            private:
                ECS::EntityId m_currentEntityId = ECS::INVALID_ID;
                const Registry* m_registry = nullptr;
                size_t m_size = 0;
                Bitset<Mani::ECS::MAX_COMPONENTS> m_componentMask;
                bool m_isAll = false;

                bool isValidIndex(const ECS::EntityId id) const;
            };

            const Iterator begin() const
            {
                if (m_bisAll)
                {
                    return Iterator(m_registry, 0, m_componentMask, m_bisAll);
                }

                ECS::EntityId entityId = 0;
                for (; entityId < m_registry->unadjustedSize(); ++entityId)
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
                return Iterator(m_registry, m_registry->unadjustedSize(), m_componentMask, m_bisAll);
            }

            const Iterator at(ECS::EntityId entityId) const
            {
                assert(m_registry != nullptr);
                if (entityId >= m_registry->unadjustedSize())
                {
                    return end();
                }

                Iterator it(m_registry, entityId, m_componentMask, m_bisAll);
                if (m_registry->isValid(entityId) && m_registry->getEntity(entityId)->hasComponents(m_componentMask))
                {
                    return it;
                }

                ++it;
                return it;
            }

            ECS::EntityId first() const
            {
                return *begin();
            }

            ECS::EntityId last() const
            {
                return *end();
            }

            size_t size() const 
            {
                assert(m_registry != nullptr);
                return m_registry->size();
            }

            size_t unadjustedSize() const
            {
                assert(m_registry != nullptr);
                return m_registry->unadjustedSize();
            }

            const Registry& getRegistry()
            {
                assert(m_registry != nullptr);
                return *m_registry;
            }

        private:
            const Registry* m_registry = nullptr;
            Bitset<ECS::MAX_COMPONENTS> m_componentMask;
            bool m_bisAll = false;
        };

        // ITERATOR BEGIN
        template<typename ...TComponents>
        inline View<TComponents...>::Iterator::Iterator(
            const Registry* inRegistry,
            ECS::EntityId inCurrentEntityId,
            Bitset<ECS::MAX_COMPONENTS> inComponentMask,
            bool inIsAll
        ) :
            m_currentEntityId(inCurrentEntityId),
            m_registry(inRegistry),
            m_size(0),
            m_componentMask(inComponentMask),
            m_isAll(inIsAll)
        {
            assert(m_registry != nullptr);
            m_size = m_registry->unadjustedSize();
        }

        template<typename ...TComponents>
        inline ECS::EntityId View<TComponents...>::Iterator::operator*() const
        {
            return m_currentEntityId;
        }
    
        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator==(const Iterator& other) const
        {
            return m_registry == other.m_registry && 
                m_size == other.m_size &&
                m_currentEntityId == other.m_currentEntityId;
        }
    
        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator!=(const Iterator& other) const
        {
            return m_registry != other.m_registry ||
                m_size != other.m_size ||
                m_currentEntityId != other.m_currentEntityId;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator>(const Iterator& other) const
        {
            assert(m_registry == other.m_registry);
            return m_currentEntityId > other.m_currentEntityId;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator<(const Iterator& other) const
        {
            assert(m_registry == other.m_registry);
            return m_currentEntityId < other.m_currentEntityId;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator>=(const Iterator& other) const
        {
            assert(m_registry == other.m_registry);
            return m_currentEntityId >= other.m_currentEntityId;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator<=(const Iterator& other) const
        {
            assert(m_registry == other.m_registry);
            return m_currentEntityId <= other.m_currentEntityId;
        }

        template<typename ...TComponents>
        inline View<TComponents...>::Iterator& View<TComponents...>::Iterator::operator++()
        {
            for (m_currentEntityId++; m_currentEntityId < m_size; m_currentEntityId++)
            {
                if (isValidIndex(m_currentEntityId))
                {
                    break; 
                }
            }
            return *this;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::isValidIndex(const ECS::EntityId id) const
        {
            if (!m_registry->isValid(id))
            {
                return false;
            }

            const Entity* entity = m_registry->getEntity(id);
            return m_isAll || entity->hasComponents(m_componentMask);
        }
        // ITERATOR END
    }
}