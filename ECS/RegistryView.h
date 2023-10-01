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
        RegistryView(EntityRegistry& registry);

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
            EntityId CurrentEntityId;
            EntityRegistry* RegistryPtr;
            std::bitset<ECSEngine::MAX_COMPONENTS> ComponentMask;
            bool IsAll = false;

            bool IsValidIndex() const;
        };

        const Iterator begin() const;
        const Iterator end() const;

    private:
        EntityRegistry* RegistryPtr = nullptr;
        std::bitset<ECSEngine::MAX_COMPONENTS> ComponentMask;
        bool IsAll = false;
    };

    template<typename ...TComponents>
    inline RegistryView<TComponents...>::RegistryView(EntityRegistry& registry)
        : RegistryPtr(&registry)
    {
        if (sizeof...(TComponents) == 0)
        {
            IsAll = true;
        }
        else
        {
            int componentIds[] = { 0, EntityRegistry::GetComponentId<TComponents>() ... };
            for (size_t i = 1; i < (sizeof...(TComponents) + 1); ++i)
            {
                ComponentMask.set(componentIds[i]);
            }
        }
    }

    // ITERATOR BEGIN
    template<typename ...TComponents>
    inline RegistryView<TComponents...>::Iterator::Iterator(
        ECSEngine::EntityId InCurrentEntityId,
        EntityRegistry* InRegistryPtr,
        std::bitset<ECSEngine::MAX_COMPONENTS> InComponentMask,
        bool InIsAll
    ) :
        CurrentEntityId(InCurrentEntityId),
        RegistryPtr(InRegistryPtr),
        ComponentMask(InComponentMask),
        IsAll(InIsAll)
    {}

    template<typename ...TComponents>
    inline EntityId RegistryView<TComponents...>::Iterator::operator*() const
    {
        return RegistryPtr->Entities[CurrentEntityId];
    }
    
    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::operator==(const Iterator& other) const
    {
        return CurrentEntityId == other.CurrentEntityId;
    }
    
    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::operator!=(const Iterator& other) const
    {
        return CurrentEntityId != other.CurrentEntityId;
    }

    template<typename ...TComponents>
    inline RegistryView<TComponents...>::Iterator& RegistryView<TComponents...>::Iterator::operator++()
    {
        do
        {
            CurrentEntityId++;
        } while (CurrentEntityId < RegistryPtr->Entities.size() && !IsValidIndex(CurrentEntityId));
        return *this;
    }

    template<typename ...TComponents>
    inline bool RegistryView<TComponents...>::Iterator::IsValidIndex() const
    {
        if (!RegistryPtr->IsValid(CurrentEntityId))
        {
            return false;
        }

        const Entity& entity = RegistryPtr->Entities[CurrentEntityId];
        return IsAll || entity.HasComponents(ComponentMask);
    }
    // ITERATOR END

    template<typename ...TComponents>
    inline const RegistryView<TComponents...>::Iterator RegistryView<TComponents...>::begin() const
    {
        if (IsAll)
        {
            return Iterator(RegistryPtr, 0, ComponentMask, IsAll);
        }

        EntityId entityId = 0;
        for (; RegistryPtr->Entities.size(); ++entityId)
        {
            if (!RegistryPtr->IsValid(entityId))
            {
                continue;
            }

            Entity& entity = RegistryPtr->Entities[entityId];
            if (entity.HasComponents(ComponentMask))
            {
                break;
            }
        }
       
        return Iterator(RegistryPtr, entityId, ComponentMask, IsAll);
    }

    template<typename ...TComponents>
    inline const RegistryView<TComponents...>::Iterator RegistryView<TComponents...>::end() const
    {
        return Iterator(RegistryPtr, RegistryPtr->Entities.size(), ComponentMask, IsAll);
    }
}

#pragma warning(default:4251)