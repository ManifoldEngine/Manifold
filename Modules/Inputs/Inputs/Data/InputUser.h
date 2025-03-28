#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "InputAction.h"

namespace Mani
{
	struct InputUser
	{
		// user actions
		std::unordered_map<std::string, InputAction> actions;
		// control to actions binding
		std::unordered_map<std::string, std::unordered_set<std::string>> bindings;
		
		std::vector<ECS::EntityId> inputDevices;

		inline void setAction(const std::string& action)
		{
			actions[action] = { action };
		}

		inline void unsetAction(const std::string& action)
		{
			actions.erase(action);
		}

		inline void addBinding(const std::string& control, const std::string& action)
		{
			bindings[control].insert(action);
		}

		inline void removeBinding(const std::string& control, const std::string& action)
		{
			bindings[control].erase(action);
		}
	};
}