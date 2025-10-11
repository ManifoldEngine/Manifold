#pragma once

#include <Core/Containers/Map.h>
#include <Core/Containers/List.h>
#include <Core/ECS/Entity.h>
#include <ManiZ/Reflection.h>
#include <ManiZ/Traits.h>

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
				return m_typeInfo.get(componentId);
			}

			List<TypeInfo> getTypeInfo(const Entity& entity) const
			{
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
				if (const ComponentId* componentId = m_reflectInfo.find(name))
				{
					return *componentId;
				}
				return ECS::MAX_COMPONENTS;
			}

		private:
			Map<ComponentId, TypeInfo> m_typeInfo;
			Map<std::string_view, ComponentId> m_reflectInfo;
		};
	}
}