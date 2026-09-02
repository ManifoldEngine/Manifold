#pragma once

#include <Core/ECS/Entity.h>
#include <Core/ECS/Registry.h>
#include <Core/Containers/List.h>
#include <Core/Containers/SparseSet.h>
#include <Core/ManiTypes.h>

#include <functional>
#include <memory>

#if MANI_ASSERT_ENABLED
#include <ManiZ/Reflection.h>
#endif

namespace Mani
{
	namespace ECS
	{
		class CommandBuffer
		{
			using Command = std::function<void(CommandBuffer&, Registry&)>;

		public:
			CommandBuffer(Registry& registry) : m_registry(&registry)
			{
				m_componentPools.reserve(m_registry->getComponentCount());
			}

			template<typename T, typename ...TArgs>
			T& add(EntityId entityId, TArgs&&... args)
			{
				MANI_ASSERT(!m_registry->has<T>(entityId), "Entity {} already has a component of type {}", entityId, ManiZ::RFL::getTypeName<T>());
				SparseSet<T, ECS::Index>& pool = getOrAddComponentPool<T>();
				const ECS::Index index = ECS::toIndex(entityId);
				pool.insert(index, T(std::forward<TArgs>(args)...));
				m_commands.add([entityId](CommandBuffer& buffer, Registry& registry) mutable
				{
					const ECS::Index index = ECS::toIndex(entityId);
					T& component = buffer.getComponentPoolPtr<T>()->get(index);
					registry.add<T>(entityId, std::move(component));
				});
				return pool.get(index);
			}

			template<typename T>
			T& get(EntityId entityId)
			{
				SparseSet<T, ECS::Index>* pool = getComponentPoolPtr<T>();
				MANI_ASSERT(pool != nullptr, "No component of type {} has been added to this command buffer yet.", ManiZ::RFL::getTypeName<T>());
				const ECS::Index index = ECS::toIndex(entityId);
				pool->get(index);
			}

			template<typename T>
			void remove(EntityId entityId)
			{
				const ECS::Index index = ECS::toIndex(entityId);
				if (SparseSet<T, ECS::Index>* pool = getComponentPoolPtr<T>())
				{
					pool->removeSwap(index);
				}
				else
				{
					MANI_ASSERT(m_registry->has<T>(entityId), "Entity {} does not have component {}", entityId, ManiZ::RFL::getTypeName<T>());
					m_commands.add([entityId](CommandBuffer& buffer, Registry& registry) mutable
					{
						registry.remove<T>(entityId);
					});
				}
			}

			Mani::List<Command>& getCommands() { return m_commands; }

			void execute()
			{
				for (const Command& command : m_commands)
				{
					command(*this, *m_registry);
				}
				m_commands.clear();
			}

		private:
			template<typename T>
			[[nodiscard]] SparseSet<T, ECS::Index>& getOrAddComponentPool()
			{
				const ECS::ComponentId componentId = m_registry->getComponentId<T>();
				if (componentId >= m_componentPools.count())
				{
					m_componentPools.resize(componentId + 1);
				}
				if (m_componentPools[componentId] == nullptr)
				{
					m_componentPools[componentId] = std::make_shared<SparseSet<T, ECS::Index>>();
				}

				return *(static_cast<SparseSet<T, ECS::Index>*>(m_componentPools[componentId].get()));
			}

			template<typename T>
			[[nodiscard]] SparseSet<T, ECS::Index>* getComponentPoolPtr()
			{
				const ECS::ComponentId componentId = m_registry->getComponentId<T>();
				if (!m_componentPools.isValid(componentId))
				{
					return nullptr;
				}
				return static_cast<SparseSet<T, ECS::Index>*>(m_componentPools[componentId].get());
			}

			Mani::List<std::shared_ptr<ISparseSet<ECS::Index>>> m_componentPools;
			Mani::List<Command> m_commands;
			Registry* m_registry = nullptr;
		};

		class CommandScope : public CommandBuffer
		{
		public:
			~CommandScope()
			{
				execute();
			}
		};
	}
}