#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	namespace STB
	{
		constexpr Mani::LogChannel Log{ "STB" };

		Mani::ECS::EntityId loadConfig(ECS::Registry& registry);
	}
}