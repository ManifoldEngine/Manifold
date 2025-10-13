#pragma once

#include <Core/Containers/Map.h>
#include <Core/Containers/List.h>
#include <Core/ECS/Entity.h>
#include <ManiZ/Reflection.h>
#include <ManiZ/Traits.h>

#include <mutex>
#include <string>

namespace Mani
{
	namespace ECS
	{
		using TypeId = unsigned long long;

		struct TypeInfo
		{
			std::string_view name = "";
			size_t size = 0;
		};

		class TypeInfoContainer
		{
		public:
			template<typename T>
			void registerComponent(ComponentId componentId)
			{
				std::scoped_lock<std::mutex> lock(m_mutex);

				if (m_typeInfo.has(componentId))
				{
					return;
				}

				TypeInfo info = {
					.name = ManiZ::RFL::getTypeName<T>(),
					.size = sizeof(T),
				};

				m_typeInfo[componentId] = info;
				m_reflectInfo[info.name] = componentId;
			}

			const TypeInfo& getTypeInfo(ComponentId componentId) const
			{
				std::scoped_lock<std::mutex> lock(m_mutex);
				return m_typeInfo.get(componentId);
			}

			List<TypeInfo> getTypeInfo(const Entity& entity) const
			{
				std::scoped_lock<std::mutex> lock(m_mutex);
				List<TypeInfo> result;
				for (const auto& [componentId, typeInfo] : m_typeInfo)
				{
					if (entity.hasComponent(componentId))
					{
						result.add(typeInfo);
					}
				}
				return result;
			}

			const ComponentId reflect(const std::string_view& name) const
			{
				std::scoped_lock<std::mutex> lock(m_mutex);
				if (const ComponentId* componentId = m_reflectInfo.find(name))
				{
					return *componentId;
				}
				return ECS::MAX_COMPONENTS;
			}

		private:
			mutable std::mutex m_mutex;
			Map<ComponentId, TypeInfo> m_typeInfo;
			Map<std::string_view, ComponentId> m_reflectInfo;
		};
	}
}