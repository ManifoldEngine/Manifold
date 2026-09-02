#pragma once

#include <Core/ECS/Entity.h>
#include <Core/Containers/Array.h>
#include <Core/Containers/List.h>
#include <ManiZ/Reflection.h>
#include <string>

namespace Mani
{
	namespace ECS
	{
		struct TypeInfo
		{
			std::string_view name = "";
			SizeT size = 0;
		};

		class TypeInfoContainer
		{
		public:
			TypeInfoContainer()
			{
				m_exists.fill(false);
			}

			template<typename T>
			void registerType(ECS::ComponentId componentId)
			{
				if (m_exists[componentId])
				{
					return;
				}

				constexpr bool withNamespace = true;
				m_typeInfos[componentId] = TypeInfo{ ManiZ::RFL::getTypeName<T>(withNamespace), sizeof(T) };
				m_exists[componentId] = true;
			}

			Mani::Optional<TypeInfo> getInfo(ECS::ComponentId componentId) const
			{
				if (m_exists[componentId])
				{
					return m_typeInfos[componentId];
				}
				return {};
			}

		private:
			Mani::Array<bool, ECS::MAX_COMPONENTS> m_exists;
			Mani::Array<TypeInfo, ECS::MAX_COMPONENTS> m_typeInfos;
		};
	}
}