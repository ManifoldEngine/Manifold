#include "Archetype.h"

namespace Mani
{
	namespace ECS
	{
		Archetype::Archetype(SizeT capacity) : m_capacity(capacity)
		{
			m_componentIndices.fill(INDEX_NONE);
		}
		
		void Archetype::add(ECS::EntityId entityId)
		{
			m_entities.insert(entityId, m_entities.count());

			if (m_entities.count() <= m_capacity)
			{
				return;
			}

			incVersion();
			m_capacity *= 2;

			// resize if needed
			for (auto& components : m_components)
			{
				components->resize(m_capacity);
			}
		}

		void* Archetype::getRaw(ECS::EntityId entityId, ECS::ComponentId componentId)
		{
			MANI_ASSERT(m_componentIndices[componentId] != INDEX_NONE, "No valid component pool here");
			const SizeT index = m_componentIndices[componentId];
			const SizeT entity = m_entities.get(entityId);
			return m_components[index]->at(entity);
		}

		const void* Archetype::getRaw(ECS::EntityId entityId, ECS::ComponentId componentId) const
		{
			MANI_ASSERT(m_componentIndices[componentId] != INDEX_NONE, "No valid component pool here");
			const SizeT index = m_componentIndices[componentId];
			const SizeT entity = m_entities.get(entityId);
			return m_components[index]->at(entity);
		}

		void Archetype::removeSwap(ECS::EntityId entityId)
		{
			// get the entity's index and dense index
			const SizeT index = m_entities.get(entityId);
			// remove entity and get the new resident
			const ECS::EntityId newEntityId = m_entities.removeSwap(entityId);
			
			incVersion();
			
			if (m_entities.isEmpty() || newEntityId == INDEX_NONE)
			{
				// if we received index none, then entityId doesn't need to be swapped
				return;
			}

			const SizeT newIndex = m_entities.get(newEntityId);
			
			for (auto& components : m_components)
			{
				components->swap(index, newIndex);
			}
		}

		std::unique_ptr<Archetype> Archetype::makeNew(SizeT capacity)
		{
			std::unique_ptr<Archetype> arch = std::make_unique<Archetype>(capacity);
			for (SizeT index = 0; index < m_componentIds.count(); index++)
			{
				const ECS::ComponentId componentId = m_componentIds[index];
				arch->m_componentIds.add(componentId);
				arch->m_components.add(m_components[index]->makeNew(capacity));
				arch->m_componentIndices[componentId] = index;
			}
			return arch;
		}

		void Archetype::move(ECS::EntityId entityId, Archetype& lhs, Archetype& rhs)
		{
			const Mani::List<ECS::ComponentId>& componentIds = lhs.m_componentIds.count() <= rhs.m_componentIds.count() ? lhs.m_componentIds : rhs.m_componentIds;

			const SizeT fromIndex = lhs.m_entities.get(entityId);
			const SizeT toIndex = rhs.m_entities.get(entityId);

			for (const auto componentId : componentIds)
			{
				const SizeT lhsIndex = lhs.m_componentIndices[componentId];
				const SizeT rhsIndex = rhs.m_componentIndices[componentId];

				MANI_ASSERT(lhsIndex != INDEX_NONE && rhsIndex != INDEX_NONE, "Incompatible move detected");
				lhs.m_components[lhsIndex]->moveTo(*rhs.m_components[rhsIndex].get(), fromIndex, toIndex);
			}
		}

		void Archetype::incVersion()
		{
#ifdef MANI_DEBUG
			constexpr SizeT maxVersion = std::numeric_limits<SizeT>::max();
			if (m_version == maxVersion) { MANI_LOG_WARNING(Mani::LogCore, "Archetype version loopback"); }
#endif
			m_version++;
		}
	}
}
