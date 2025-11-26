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
                : m_registry(&registry), m_count(registry.unadjustedCount())
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
                    ECS::Index inCurrentEntityId,
                    ECS::Index inCount,
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

                ECS::Index getIndex() const;

            private:
                ECS::Index m_index = 0;
                const Registry* m_registry = nullptr;
                ECS::Index m_count = 0;
                Bitset<Mani::ECS::MAX_COMPONENTS> m_componentMask;
                bool m_isAll = false;

                bool isValidIndex(const ECS::Index id) const;
            };

            const Iterator begin() const
            {
                if (m_bisAll)
                {
                    return Iterator(m_registry, 0, m_count, m_componentMask, m_bisAll);
                }

                ECS::Index index = 0;
                for (; index < m_count; ++index)
                {
                    if (!m_registry->isValidIndex(index))
                    {
                        continue;
                    }

                    if (m_registry->getEntityAt(index)->hasComponents(m_componentMask))
                    {
                        break;
                    }
                }

                return Iterator(m_registry, index, m_count, m_componentMask, m_bisAll);
            }

            const Iterator end() const
            {
                return Iterator(m_registry, m_count, m_count, m_componentMask, m_bisAll);
            }

            const Iterator at(ECS::Index index) const
            {
                assert(m_registry != nullptr);
                if (index >= m_count)
                {
                    return end();
                }

                Iterator it(m_registry, index, m_count, m_componentMask, m_bisAll);
                if (m_registry->isValidIndex(index) && m_registry->getEntityAt(index)->hasComponents(m_componentMask))
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

            ECS::Index count() const 
            {
                return m_count;
            }

            const Registry& getRegistry()
            {
                assert(m_registry != nullptr);
                return *m_registry;
            }

        private:
            const Registry* m_registry = nullptr;
            ECS::Index m_count = 0;
            Bitset<ECS::MAX_COMPONENTS> m_componentMask;
            bool m_bisAll = false;
        };

        // ITERATOR BEGIN
        template<typename ...TComponents>
        inline View<TComponents...>::Iterator::Iterator(
            const Registry* inRegistry,
            ECS::Index inIndex,
            ECS::Index inCount,
            Bitset<ECS::MAX_COMPONENTS> inComponentMask,
            bool inIsAll
        ) :
            m_index(inIndex),
            m_registry(inRegistry),
            m_count(inCount),
            m_componentMask(inComponentMask),
            m_isAll(inIsAll)
        {
            MANI_ASSERT(m_registry != nullptr, "Can't view a null registry");
        }

        template<typename ...TComponents>
        inline ECS::EntityId View<TComponents...>::Iterator::operator*() const
        {
            const Entity* entity = m_registry->getEntityAt(m_index);
            MANI_ASSERT(entity != nullptr, "null entity");
            return entity->getId();
        }
    
        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator==(const Iterator& other) const
        {
            return m_registry == other.m_registry && 
                m_count == other.m_count &&
                m_index == other.m_index;
        }
    
        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator!=(const Iterator& other) const
        {
            return !(operator==(other));
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator>(const Iterator& other) const
        {
            MANI_ASSERT(m_registry == other.m_registry, "Trying to compare two different registries");
            return m_index > other.m_index;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator<(const Iterator& other) const
        {
            MANI_ASSERT(m_registry == other.m_registry, "Trying to compare two different registries");
            return m_index < other.m_index;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator>=(const Iterator& other) const
        {
            MANI_ASSERT(m_registry == other.m_registry, "Trying to compare two different registries");
            return m_index >= other.m_index;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::operator<=(const Iterator& other) const
        {
            MANI_ASSERT(m_registry == other.m_registry, "Trying to compare two different registries");
            return m_index <= other.m_index;
        }

        template<typename ...TComponents>
        inline View<TComponents...>::Iterator& View<TComponents...>::Iterator::operator++()
        {
            m_index++;
            for (; m_index < m_count; m_index++)
            {
                if (isValidIndex(m_index))
                {
                    break; 
                }
            }
            return *this;
        }

        template<typename ...TComponents>
        inline ECS::Index View<TComponents...>::Iterator::getIndex() const
        {
            return m_index;
        }

        template<typename ...TComponents>
        inline bool View<TComponents...>::Iterator::isValidIndex(const ECS::Index index) const
        {
            if (!m_registry->isValidIndex(index))
            {
                return false;
            }

            const Entity* entity = m_registry->getEntityAt(index);
            return m_isAll || entity->hasComponents(m_componentMask);
        }
        // ITERATOR END
    }
}