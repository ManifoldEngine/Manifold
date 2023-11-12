#include "InputSystem.h"

#include <Log.h>
#include <Inputs/Interfaces/InputGenerator.h>

using namespace ECSEngine;

std::string_view InputSystem::getName() const
{
	return "InputSystem";
}

bool InputSystem::shouldTick(EntityRegistry& registry) const
{
	return true;
}

void InputSystem::tick(float deltaTime, EntityRegistry& registry)
{
	for (const auto& user : m_users)
	{
		if (user->generator == nullptr)
		{
			continue;
		}

		std::vector<ButtonControl> buffer;
		if (user->generator->consumeInputBuffer(buffer))
		{
			for (const ButtonControl& buttonEvent : buffer)
			{
				const std::string boundActionName = user->inputBindings[buttonEvent.name];
				InputAction& action = user->actions[boundActionName];

				if (action.isPressed != buttonEvent.isPressed)
				{
					action.isPressed = buttonEvent.isPressed;
					onActionChanged.broadcast(user->userId, action);
				}
			}
		}
		else
		{
			ECSE_LOG_WARNING(LogInputs, "Could not get input buffer from generator {}", user->generator->getName());
		}
		
		std::vector<AxisControl> axis;
		if (user->generator->getAxis(axis))
		{
			// axis are reset each tick.
			for (auto& [name, action] : user->actions)
			{
				action.x = 0.f;
				action.y = 0.f;
				action.z = 0.f;
			}

			for (const AxisControl& axisControl : axis)
			{
				const std::string boundActionName = user->inputBindings[axisControl.name];
				InputAction& action = user->actions[boundActionName];

				action.x += axisControl.x;
				action.y += axisControl.y;
				action.z += axisControl.z;
			}
		}
		else
		{
			ECSE_LOG_WARNING(LogInputs, "Could not get axis from generator {}", user->generator->getName());
		}
	}
}

uint32_t InputSystem::createInputUser()
{
	std::shared_ptr<InputUser> user = std::make_shared<InputUser>();
	user->userId = userCounter++;
	m_users.push_back(user);
	return 0;
}

std::shared_ptr<InputUser> InputSystem::getInputUser(uint32_t userId)
{
	const auto it = std::find_if(m_users.begin(), m_users.end(), [&userId](const std::shared_ptr<InputUser>& inputUser) {
		return userId == inputUser->userId;
	});

	if (it == m_users.end())
	{
		return nullptr;
	}
	
	return *it;
}

void InputSystem::destroyInputUser(uint32_t userId)
{
	const auto it = std::find_if(m_users.begin(), m_users.end(), [&userId](const std::shared_ptr<InputUser>& inputUser) {
		return userId == inputUser->userId;
	});

	if (it != m_users.end())
	{
		m_users.erase(it);
	}
}

void InputSystem::assignInputGenerator(uint32_t userId, std::shared_ptr<IInputGenerator> generator)
{
	std::shared_ptr<InputUser> inputUser = getInputUser(userId);
	if (inputUser == nullptr)
	{
		return;
	}

	inputUser->generator = generator;
}
