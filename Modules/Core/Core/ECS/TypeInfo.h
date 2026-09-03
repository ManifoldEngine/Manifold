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
				TypeInfo typeinfo = createInfo<T>();

				//Mani::List<std::string> names(ManiZ::RFL::getMemberNames<T>());

				//auto f = [&info]<typename T> (const Mani::List<std::string>&names, SizeT & index, T)
				//{
				//	TypeMember memberInfo;
				//	memberInfo.name = names[index];
				//	memberInfo.info = createInfo<T>();

				//	info.members.add(memberInfo);
				//	index++;
				//};

				//T object{};
				//ManiZ::RFL::visitMembers<T>(object, [&f, &info, &names](const auto& ...members)
				//{
				//	SizeT index = 0;
				//	(f(names, index, members), ...);
				//});

				m_typeInfos[componentId] = std::move(typeinfo);
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
			template<typename T>
			static TypeInfo createInfo()
			{
				TypeInfo info;
				info.name = ManiZ::RFL::getTypeName<T>();
				info.size = sizeof(T);
				return info;
			}

			Mani::Array<bool, ECS::MAX_COMPONENTS> m_exists;
			Mani::Array<TypeInfo, ECS::MAX_COMPONENTS> m_typeInfos;
		};
	}
}