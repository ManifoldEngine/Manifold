#pragma once

#include <Core/Containers/Map.h>
#include <Core/Containers/List.h>
#include <Inputs/Data/InputAction.h>

#include <string>

namespace Mani
{
	struct InputUser
	{
		// user actions
		Map<std::string, InputAction> actions;
		// control to actions binding
		Map<std::string, List<std::string>> bindings;

		List<ECS::EntityId> inputDevices;
	};
}