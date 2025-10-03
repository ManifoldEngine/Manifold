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

		inline void setAction(const std::string& action)
		{
			actions[action] = { action };
		}

		inline void unsetAction(const std::string& action)
		{
			actions.remove(action);
		}

		inline void addBinding(const std::string& control, const std::string& action)
		{
			bindings[control].addUnique(action);
		}

		inline void removeBinding(const std::string& control, const std::string& action)
		{
			bindings[control].remove(action);
		}
	};
}