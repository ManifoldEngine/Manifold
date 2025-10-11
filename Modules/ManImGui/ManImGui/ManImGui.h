#pragma once

#include <Core/CoreFwd.h>
#include <string_view>

namespace Mani
{
	const std::string_view LogManImGui = "LogManImGui";

	namespace ManImGuiStatics
	{
		bool isShowing(const ECS::Registry& registry);

		namespace ManifoldMenu
		{
			ECS::EntityId getEntityId(const ECS::Registry& registry);
		}

		namespace Menu
		{
			void addItem(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name);
			void removeItem(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name);
			bool isOpened(const ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name);
			void open(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name);
			void close(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name);
		}
	}
}