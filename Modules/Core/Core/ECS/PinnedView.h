#pragma once

#include <Core/ECS/Entity.h>
#include <Core/ECS/Registry.h>
#include <Core/ManiTraits.h>
#include <tuple>

namespace Mani
{
	namespace ECS
	{
		template<class TRegistry, typename ...Ts>
		requires(DerivedFrom<TRegistry, Registry>)
		class BasePinnedView
		{
		public:
			BasePinnedView() = default;
			BasePinnedView(TRegistry& registry) :
				m_registry(&registry)
			{
				m_mask = registry.getMask<Ts...>();
				(tryAdd<Ts>(m_pools, registry), ...);

				m_pools.sort([](const Registry::IComponentPool* lhs, const Registry::IComponentPool* rhs)
				{
					return lhs->count() < rhs->count();
				});
			};

			struct Iterator
			{
				Iterator() = default;
				Iterator(const BasePinnedView& view, SizeT index) :
					m_registry(&view.getRegistry()),
					m_indices(nullptr),
					m_mask(view.m_mask),
					m_index(index)
				{
					if (!view.m_pools.isEmpty())
					{
						const Registry::IComponentPool* pool = view.m_pools.first();
						m_indices = &pool->getDenseIndices();
						m_count = pool->count();
					}
				}

				[[nodiscard]] bool operator==	(const Iterator& other) const { MANI_ASSERT(m_registry == other.m_registry && m_mask == other.m_mask, MATCHING_VIEWS_MESSAGE); return m_index == other.m_index; }
				[[nodiscard]] bool operator!=	(const Iterator& other) const { MANI_ASSERT(m_registry == other.m_registry && m_mask == other.m_mask, MATCHING_VIEWS_MESSAGE); return m_index != other.m_index; }
				[[nodiscard]] bool operator>	(const Iterator& other) const { MANI_ASSERT(m_registry == other.m_registry && m_mask == other.m_mask, MATCHING_VIEWS_MESSAGE); return m_index > other.m_index; }
				[[nodiscard]] bool operator<	(const Iterator& other) const { MANI_ASSERT(m_registry == other.m_registry && m_mask == other.m_mask, MATCHING_VIEWS_MESSAGE); return m_index < other.m_index; }
				[[nodiscard]] bool operator>=	(const Iterator& other) const { MANI_ASSERT(m_registry == other.m_registry && m_mask == other.m_mask, MATCHING_VIEWS_MESSAGE); return m_index >= other.m_index; }
				[[nodiscard]] bool operator<=	(const Iterator& other) const { MANI_ASSERT(m_registry == other.m_registry && m_mask == other.m_mask, MATCHING_VIEWS_MESSAGE); return m_index <= other.m_index; }


				[[nodiscard]] std::tuple<ECS::EntityId, Ts&...> operator*() const
				{
					MANI_ASSERT(m_registry != nullptr, "Null registry in view");
					MANI_ASSERT(m_indices->isValid(m_index), "Out of bounds");
					const ECS::Index entityIdx = static_cast<ECS::Index>(m_indices->at(m_index));
					const ECS::Entity* entity = m_registry->getEntityAt(entityIdx);
					MANI_ASSERT(entity != nullptr, "Null entity in view");
					const ECS::EntityId entityId = entity->getId();
					return std::tuple<ECS::EntityId, Ts&...>(
						m_indices->at(m_index),
						m_registry->getPinned<Ts>(entityId)...
					);
				}

				Iterator& operator++()
				{
					for (m_index += 1; m_index < m_count; m_index++)
					{
						const ECS::Index index = static_cast<ECS::Index>(m_indices->at(m_index));
						if (const ECS::Entity* entity = m_registry->getEntityAt(index))
						{
							if (entity->pinned.contains(m_mask))
							{
								return *this;
							}
						}
					}
					return *this;
				}

				ECS::EntityId getEntityId() const
				{
					MANI_ASSERT(isValidIndex(), "out of bouds");
					return m_registry->getEntityAt(static_cast<ECS::Index>(m_indices->at(m_index)))->getId();
				}

				bool isValidIndex() const
				{
					return m_index < m_count 
						&& m_registry
						   ->getEntityAt(static_cast<ECS::Index>(m_indices->at(m_index)))
						   ->pinned.contains(m_mask);
				}

			private:
				TRegistry* m_registry = nullptr;
				const Mani::Array<SizeT, ECS::PINNED_COMPONENTS_CAPACITY>* m_indices = nullptr;
				ECS::ComponentMask m_mask;
				SizeT m_count = 0;
				SizeT m_index = INDEX_NONE;

				static inline constexpr std::string_view MATCHING_VIEWS_MESSAGE = "Comparing iterators from different views";
			};

			Iterator begin() const
			{
				Iterator it(*this, 0);
				const SizeT n = count();
				for (SizeT i = 0; i < n; i++)
				{
					if (it.isValidIndex())
					{
						break;
					}
					++it;
				}
				
				return it;
			}

			Iterator end() const
			{
				return Iterator(*this, count());
			}

			TRegistry& getRegistry() const
			{
				MANI_ASSERT(m_registry != nullptr, "View with a null registry");
				return *m_registry;
			}

			SizeT count() const { return !m_pools.isEmpty() ? m_pools.first()->count() : 0; }
			ECS::ComponentMask getMask() const { return m_mask; }
		private:

			template<typename T>
			static void tryAdd(Mani::List<const Registry::IComponentPool*>& pools, TRegistry& registry)
			{
				if (auto* pool = registry.getPinnedComponentPoolPtr<T>())
				{
					pools.add(pool);
				}
			}

			TRegistry* m_registry = nullptr;
			ECS::ComponentMask m_mask;
			Mani::List<const Registry::IComponentPool*> m_pools;
		};

		template<typename ...Ts>
		class PinnedView : public BasePinnedView<Registry, Ts...>
		{
		public:
			PinnedView() : BasePinnedView<Registry, Ts...>() {};
			PinnedView(Registry& registry) : BasePinnedView<Registry, Ts...>(registry) {};
		};

		template<typename ...Ts>
		class ConstPinnedView : public BasePinnedView<const Registry, const Ts...>
		{
		public:
			ConstPinnedView() : BasePinnedView<const Registry, const Ts...>() {};
			ConstPinnedView(const Registry& registry) : BasePinnedView<const Registry, const Ts...>(registry) {};
		};
	}
}