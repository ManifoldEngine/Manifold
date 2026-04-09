#pragma once

#include <Core/ECS/Entity.h>
#include <Core/Containers/List.h>
#include <Core/Containers/Map.h>
#include <Core/Containers/SparseSet.h>

#include <memory>
#include <limits>

namespace Mani
{
	namespace ECS
	{
		struct IComponentPool
		{
			virtual void* at(SizeT index) = 0;
			virtual void swap(SizeT i1, SizeT i2) = 0;
			virtual void moveTo(IComponentPool& dest, SizeT from, SizeT to) = 0;
			virtual void resize(SizeT newSize) = 0;
			virtual std::unique_ptr<IComponentPool> makeNew(SizeT capacity) const = 0;
		};

		template<typename T>
		struct ComponentPool : IComponentPool
		{
			ComponentPool(SizeT count) : data(count) {};

			
			void* at(SizeT index) override
			{
				return &data[index];
			}

			void swap(SizeT i1, SizeT i2) override
			{
				data.swap(i1, i2);
			}

			void moveTo(IComponentPool& other, SizeT from, SizeT to) override
			{
				auto& dest = static_cast<ComponentPool<T>&>(other);
				dest.data[to] = std::move(data[from]);
			}

			void resize(SizeT newSize) override
			{
				data.resize(newSize);
			}

			std::unique_ptr<IComponentPool> makeNew(SizeT capacity) const override
			{
				return std::make_unique<ComponentPool<T>>(capacity);
			}

			Mani::List<T> data;
		};

		class Archetype
		{
		public:			
			Archetype(SizeT capacity);

			void add(ECS::EntityId entityId);

			void* getRaw(ECS::EntityId entityId, ECS::ComponentId componentId);
			const void* getRaw(ECS::EntityId entityId, ECS::ComponentId componentId) const;

			template<typename T>
			T* get(ECS::EntityId entityId, ECS::ComponentId componentId)
			{
				return static_cast<T*>(getRaw(entityId, componentId));
			}

			template<typename T>
			const T* get(ECS::EntityId entityId, ECS::ComponentId componentId) const
			{
				return static_cast<const T*>(getRaw(entityId, componentId));
			}
			
			void removeSwap(ECS::EntityId entityId);

			const Mani::List<ECS::ComponentId>& getComponentIds() const { return m_componentIds; }
			const Mani::List<ECS::EntityId>& getEntityIds() const { return m_entities.getDenseIndices(); }
			
			Archetype makeNew(SizeT capacity);

			template<typename T>
			void addComponentPool(ECS::ComponentId componentId)
			{
				MANI_ASSERT(m_componentIndices[componentId] == INDEX_NONE, "Attempting to overwrite an existing component pool.");
				m_components.add(std::make_unique<ComponentPool<T>>(m_capacity));
				m_componentIds.add(componentId);
				m_componentIndices[componentId] = m_componentIds.count() - 1;
			}

			template<typename ...Ts, typename ...Cids>
			void addComponentPools(Cids... componentIds)
			{
				static_assert(sizeof...(Ts) == sizeof...(componentIds));
				(addComponentPool<Ts>(componentIds), ...);
			}

			template<typename T>
			void removeComponentPool(ECS::ComponentId componentId)
			{
				MANI_ASSERT(m_componentIndices[componentId] != INDEX_NONE, "Attempting to remove an non existing component pool.");
				const SizeT index = m_componentIndices[componentId];  
				m_components.removeSwapAt(index);
				m_componentIds.removeSwapAt(index);
				m_componentIndices[componentId] = INDEX_NONE;
				if (index < m_componentIds.count())
				{
					const ECS::ComponentId swapped = m_componentIds[index];
					m_componentIndices[swapped] = index;
				}
			}

			template<typename ...Ts, typename ...Cids>
			void removeComponentPools(Cids... componentIds)
			{
				static_assert(sizeof...(Ts) == sizeof...(componentIds));
				(removeComponentPool<Ts>(componentIds), ...);
			}

			template<typename T>
			T* getComponents(ECS::ComponentId componentId) 
			{
				MANI_ASSERT(m_componentIndices[componentId] != INDEX_NONE, "No valid component pool here");
				return static_cast<T*>(m_components[m_componentIndices[componentId]]->at(0)); 
			}

			template<typename T>
			const T* getComponents(ECS::ComponentId componentId) const 
			{
				MANI_ASSERT(m_componentIndices[componentId] != INDEX_NONE, "No valid component pool here");
				return static_cast<T*>(m_components[m_componentIndices[componentId]]->at(0));
			}
			
			static void move(ECS::EntityId entityId, Archetype& from, Archetype& to);

			SizeT count() const { return m_entities.count(); }
			SizeT getVersion() const { return m_version; }

		private:
			void incVersion();

			// components
			Mani::List<ComponentId> m_componentIds;
			Mani::List<std::unique_ptr<IComponentPool>> m_components;
			Mani::Array<SizeT, ECS::MAX_COMPONENTS> m_componentIndices;

			// entities
			Mani::SparseSet<SizeT, ECS::EntityId> m_entities;

			SizeT m_capacity;
			SizeT m_version = 0;
		};
	}
}