#pragma once

#include <Core/ECS/Entity.h>
#include <Core/ECS/Registry.h>
#include <Core/ECS/Archetype.h>
#include <Core/ECS/CommandBuffer.h>
#include <Core/ManiTraits.h>
#include <tuple>

namespace Mani
{
	namespace ECS
	{
		template<class TRegistry, class TArchetype, typename ...Ts>
		requires(DerivedFrom<TRegistry, Registry> && DerivedFrom<TArchetype, Archetype>)
		class BaseView
		{
		public:
			BaseView() = default;
			BaseView(TRegistry& registry) : 
				m_registry(&registry),
				m_mask(registry.getMask<Bare<Ts>...>()),
				m_locks(true)
			{
				for (auto& [mask, archetype] : registry.m_archetypes)
				{
					if (mask.contains(m_mask))
					{
						m_archetypes.add(&archetype);
					}
				}

				if (m_locks)
				{
					m_registry->lock();
				}
			}

			~BaseView()
			{
				if (m_locks && m_registry != nullptr)
				{
					m_registry->unlock();
				}
			}

			BaseView(const BaseView&) = delete;
			BaseView(BaseView&) = delete;
			BaseView& operator=(const BaseView&) = delete;

			struct Iterator
			{
				Iterator() = default;
				Iterator(const BaseView& view, SizeT archetypeIndex, SizeT localIndex) :
					m_view(&view),
					m_archetypeIndex(archetypeIndex),
					m_localIndex(localIndex)
				{
					m_totalIndex = 0;

					// Accumulate indices up to the archetype index
					for (SizeT index = 0; index < m_archetypeIndex; index++)
					{
						m_totalIndex += view.m_archetypes[index]->count();
					}

					// then add the current archetype's index
					m_totalIndex += m_localIndex;
				}

				[[nodiscard]] std::tuple<ECS::EntityId, Ts&...> operator*() const
				{
					MANI_ASSERT(m_view != nullptr, "Null view in iterator.");
					const Mani::List<TArchetype*>& archetypes = m_view->m_archetypes;
					MANI_ASSERT(archetypes.isValid(m_archetypeIndex) && m_localIndex < archetypes[m_archetypeIndex]->count(), "Out of bounds.");
					TArchetype& archetype = *(archetypes[m_archetypeIndex]);
					const Mani::List<ECS::EntityId>& entityIds = archetype.getEntityIds();
					MANI_ASSERT(entityIds.isValid(m_localIndex), "Out of bounds.");
					const TRegistry& registry = m_view->getRegistry();

					return std::tuple<ECS::EntityId, Ts&...>(
						entityIds[m_localIndex],
						archetype.getComponents<Bare<Ts>>(registry.getComponentId<Ts>())[m_localIndex]...
					);
				}

				template<typename TFunctor = void(EntityId, Ts&...), typename ...TArgs>
				void apply(TFunctor&& f, TArgs&& ...args)
				{
					MANI_ASSERT(m_view != nullptr, "Null view in iterator.");
					const Mani::List<TArchetype*>& archetypes = m_view->m_archetypes;
					MANI_ASSERT(archetypes.isValid(m_archetypeIndex) && m_localIndex < archetypes[m_archetypeIndex]->count(), "Out of bounds.");
					TArchetype& archetype = *(archetypes[m_archetypeIndex]);
					const Mani::List<ECS::EntityId>& entityIds = archetype.getEntityIds();
					MANI_ASSERT(entityIds.isValid(m_localIndex), "Out of bounds.");
					const TRegistry& registry = m_view->getRegistry();

					f(args..., entityIds[m_localIndex], archetype.getComponents<Bare<Ts>>(registry.getComponentId<Ts>())[m_localIndex]...);
				}

				ECS::EntityId getEntityId() const
				{
					MANI_ASSERT(m_view != nullptr, "Null view in iterator.");
					const Mani::List<TArchetype*>& archetypes = m_view->m_archetypes;
					if (!archetypes.isValid(m_archetypeIndex) || m_localIndex >= archetypes[m_archetypeIndex]->count())
					{
						return ECS::INVALID_ID;
					}

					TArchetype& archetype = *(archetypes[m_archetypeIndex]);
					const Mani::List<ECS::EntityId>& entityIds = archetype.getEntityIds();
					if (!entityIds.isValid(m_localIndex))
					{
						return ECS::INVALID_ID;
					}

					return entityIds[m_localIndex];
				}

				[[nodiscard]] bool operator==	(const Iterator& other) const { MANI_ASSERT(m_view == other.m_view, MATCHING_VIEWS_MESSAGE); return m_totalIndex ==	other.m_totalIndex; }
				[[nodiscard]] bool operator!=	(const Iterator& other) const { MANI_ASSERT(m_view == other.m_view, MATCHING_VIEWS_MESSAGE); return m_totalIndex !=	other.m_totalIndex; }
				[[nodiscard]] bool operator>	(const Iterator& other) const { MANI_ASSERT(m_view == other.m_view, MATCHING_VIEWS_MESSAGE); return m_totalIndex >	other.m_totalIndex; }
				[[nodiscard]] bool operator<	(const Iterator& other) const { MANI_ASSERT(m_view == other.m_view, MATCHING_VIEWS_MESSAGE); return m_totalIndex <	other.m_totalIndex; }
				[[nodiscard]] bool operator>=	(const Iterator& other) const { MANI_ASSERT(m_view == other.m_view, MATCHING_VIEWS_MESSAGE); return m_totalIndex >=	other.m_totalIndex; }
				[[nodiscard]] bool operator<=	(const Iterator& other) const { MANI_ASSERT(m_view == other.m_view, MATCHING_VIEWS_MESSAGE); return m_totalIndex <=	other.m_totalIndex; }

				Iterator& operator++()
				{
					MANI_ASSERT(m_view != nullptr, "Null view in iterator.");

					m_localIndex++;
					m_totalIndex++;

					const Mani::List<TArchetype*>& archetypes = m_view->m_archetypes;
					for (; m_archetypeIndex < archetypes.count(); m_archetypeIndex++)
					{
						TArchetype& archetype = *archetypes[m_archetypeIndex];
						if (m_localIndex < archetype.count())
						{
							break;
						}

						m_localIndex = 0;
					}

					return *this;
				}

				[[nodiscard]] Iterator operator+(SizeT offset) const
				{
					Iterator result(*this);
					for (SizeT i = 0; i < offset; i++)
					{
						++result;
					}
					return result;
				}

			private:
				const BaseView* m_view = nullptr;
				SizeT m_archetypeIndex = INDEX_NONE;
				SizeT m_localIndex = INDEX_NONE;
				SizeT m_totalIndex = INDEX_NONE;

				static inline constexpr std::string_view MATCHING_VIEWS_MESSAGE = "Comparing iterators from different views";
			};

			Iterator begin() const
			{
				SizeT archetypeIndex = 0;
				// iterate up to the first archetypes that has data
				for (; archetypeIndex < m_archetypes.count(); archetypeIndex++)
				{
					if (m_archetypes[archetypeIndex]->count() > 0)
					{
						break;
					}
				}
				return Iterator(*this, archetypeIndex, 0);
			}

			Iterator end() const
			{
				if (m_archetypes.isEmpty())
				{
					return begin();
				}

				return Iterator(*this, m_archetypes.count(), 0);
			}

			// locks the view's registry
			void lock()
			{
				MANI_ASSERT(!m_locks, "trying to lock a view that already has the registry locked out.");
				MANI_ASSERT(m_registry != nullptr, "View with a null registry");
				m_registry->lock();
				m_locks = true;
			}

			// unlocks the view's registry.
			//	/!\ This allows modyfing entity archetypes during a view's lifetime. 
			//	/!\ unlocking the view and modifying it during iteration will lead to invalid memory acces,
			//	/!\ crashes, or memory corruption errors. I hope you know what you're doing king.
			void unlock()
			{
				MANI_ASSERT(m_locks, "trying to unlock a view that doesn't have the registry locked out.");
				MANI_ASSERT(m_registry != nullptr, "View with a null registry");
				m_registry->unlock();
				m_locks = false;
			}

			TRegistry& getRegistry() const
			{
				MANI_ASSERT(m_registry != nullptr, "View with a null registry");
				return *m_registry;
			}

			SizeT count() const 
			{
				SizeT count = 0;
				for (TArchetype* arch : m_archetypes)
				{
					MANI_ASSERT(arch != nullptr, "Null archetype in view");
					count += arch->count();
				}
				return count;
			}
			ECS::ComponentMask getMask() const { return m_mask; }

		private:
			TRegistry* m_registry = nullptr;
			Mani::List<TArchetype*> m_archetypes;
			ECS::ComponentMask m_mask;
			bool m_locks = true;
		};

		template<typename ...Ts>
		class View : public BaseView<Registry, Archetype, Ts...>
		{
		public:
			View() : BaseView<Registry, Archetype, Ts...>() {};
			View(Registry& registry) : BaseView<Registry, Archetype, Ts...>(registry) {};
		};

		template<typename ...Ts>
		class ConstView : public BaseView<const Registry, const Archetype, const Ts...>
		{
		public:
			ConstView() : BaseView<const Registry, const Archetype, const Ts...>() {};
			ConstView(const Registry& registry) : BaseView<const Registry, const Archetype, const Ts...>(registry) {};
		};
	}

	template<typename... Ts, typename TFunctor, typename TRegistry, typename TArchetype, typename ...TArgs>
	void foreach(ECS::BaseView<TRegistry, TArchetype, Ts...>& view, TFunctor&& f, TArgs&&... args)
	{
		for (auto it = view.begin(); it != view.end(); ++it)
		{
			it.apply(f, args...);
		}
	}

	template<typename... Ts, typename TFunctor, typename ...TArgs>
	void foreachWithCmd(ECS::View<Ts...>& view, TFunctor&& f, TArgs&&... args)
	{
		ECS::CommandBuffer cmd(view.getRegistry());
		for (auto it = view.begin(); it != view.end(); ++it)
		{
			it.apply(f, cmd, args...);
		}
		view.unlock();
		cmd.execute();
		view.lock();
	}
}