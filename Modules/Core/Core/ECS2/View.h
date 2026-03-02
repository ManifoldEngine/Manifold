#pragma once

#include <Core/ECS/Entity.h>
#include <Core/ECS2/Registry.h>
#include <Core/Containers/SparseSet.h>
#include <tuple>

namespace Mani
{
	namespace ECS
	{
		template<typename ...Ts>
		class View
		{
		public:
			View() = default;
			View(const Registry& registry) : m_registry(&registry)
			{
				m_mask = registry.getMask<Ts...>();
				(m_pools.add(registry.getComponentPoolPtr<Ts>()), ...);
				
				SizeT maxEntityCount = ECS::MAX_ENTITY_COUNT;
				for (SizeT index = 0; index < m_pools.count(); index++)
				{
					const ISparseSet* sparseSet = m_pools[index];
					MANI_ASSERT(sparseSet != nullptr, "Found null component pool in view");
					const SizeT count = sparseSet->count();
					if (count < maxEntityCount)
					{
						m_poolIndex = index;
						maxEntityCount = count;
					}
				}

#if MANI_DEBUG
				if (!m_pools.isEmpty())
				{
					MANI_ASSERT(m_poolIndex != INDEX_NONE, "Couldn't find a component pool to iterate on");
				}
#endif
			}

			struct Iterator
			{
				Iterator() = default;
				Iterator(const View& view, SizeT index) :
					m_registry(&view.getRegistry()),
					m_mask(view.getMask()),
					m_index(index)
				{
					if (const ISparseSet* pool = view.getSmallesPool())
					{
						m_indices = pool->getDenseIndices();
					}
				}

				[[nodiscard]] std::tuple<Ts...> operator*() const
				{
					MANI_ASSERT(m_registry != nullptr, "Undefined registry");
					const Entity* entity = m_registry->getEntityAt(toEntityIndex(m_index));
					MANI_ASSERT(entity != nullptr, "Trying to resolve a null entity");
					return std::make_tuple(m_registry->get<Ts>(entity->getId())...);
				}

				[[nodiscard]] bool operator==(const Iterator& other) const { return isSameView(other) && m_index == other.m_index; }
				[[nodiscard]] bool operator!=(const Iterator& other) const { return !operator==(other); }
				[[nodiscard]] bool operator>(const Iterator& other) const { return isSameView(other) && m_index > other.m_index; }
				[[nodiscard]] bool operator<(const Iterator& other) const { return isSameView(other) && m_index < other.m_index; }
				[[nodiscard]] bool operator>=(const Iterator& other) const { return isSameView(other) && m_index >= other.m_index; }
				[[nodiscard]] bool operator<=(const Iterator& other) const { return isSameView(other) && m_index <= other.m_index; }

				// TODO: when iterating on the smallest component pool, entities of that pool are not guaranteed to match the mask
				// we still need to iterate on the pool until we find an entity that matches the mask
				Iterator& operator++() 
				{
					for (m_index += 1; m_index < m_indices.count(); m_index++)
					{
						const ECS::Index entityIndex = toEntityIndex(m_index);
						if (const Entity* entity = m_registry->getEntityAt(entityIndex))
						{
							if (entity->hasComponents(m_mask))
							{
								return *this;
							}
						}
					}
					return *this; 
				}

				[[nodiscard]] bool isSameView(const Iterator& other) const { return m_registry == other.m_registry && m_mask == other.m_mask; }

				[[nodiscard]] ECS::Index toEntityIndex(SizeT index) const
				{
					if (m_indices.isValid(index))
					{
						return static_cast<ECS::Index>(m_indices[index]);
					}
					return MAX_INDEX;
				}

			private:
				const Registry* m_registry = nullptr;
				Mani::List<SizeT> m_indices;
				const ComponentMask m_mask;
				SizeT m_index = INDEX_NONE;
			};

			const Iterator begin() const { return Iterator(*this, 0); }
			const Iterator end() const { return Iterator(*this, getSmallesPool()->count() - 1); }

			const Registry& getRegistry() const
			{
				MANI_ASSERT(m_registry != nullptr, "View with a null registry");
				return *m_registry;
			}

			const SizeT count() const { return !m_pools.isEmpty() ? m_pools[m_poolIndex]->count() : 0; }
			ComponentMask getMask() const { return m_mask; }
		private:

			const ISparseSet* getSmallesPool() const
			{
				if (m_pools.isValid(m_poolIndex))
				{
					return m_pools[m_poolIndex];
				}
				return nullptr;
			}

			const Registry* m_registry = nullptr;
			ComponentMask m_mask;
			Mani::List<const ISparseSet*> m_pools;
			SizeT m_poolIndex = INDEX_NONE;
		};
	}
}