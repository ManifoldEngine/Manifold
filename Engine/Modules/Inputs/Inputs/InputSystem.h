#pragma once

#include "Core/System/System.h"
#include <Inputs/Inputs.h>
#include <Inputs/Data/InputAction.h>
#include <Events/Event.h>

#include <unordered_map>
#include <memory>

namespace ECSEngine
{
	DECLARE_EVENT(ActionEvent, uint32_t /*userId*/, const InputAction& /*inputAction*/)

	class IInputGenerator;

	constexpr uint32_t LOCAL_USERID = 0;

	struct InputUser
	{
		friend class InputSystem;

		// user actions
		std::unordered_map<std::string, InputAction> actions;
		// control to actions binding
		std::unordered_map<std::string, std::vector<std::string>> bindings;
	
	private:
		uint32_t userId = 0;
		std::weak_ptr<IInputGenerator> generator;
	};

	/*
	 * Usage: 
	 * - create InputUser, each user has an associated actions map and input bindings
	 *   - actions map represents all the actions available to the user
	 *   - input bindings defines what control trigger what actions
	 *   - an action can have multiple controls
	 * - assign a IInputGenerator to an input user. Generators encapsulate controllers, like keyboard and mouse
	 *   or gamepads. 
	 * - subscribe to onActionEvent to receive button presses or use the getAction getter to get an action's state
	 */
	class InputSystem: public SystemBase
	{
	public:
		std::string_view getName() const override;
		bool shouldTick(EntityRegistry& registry) const override;

		void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;
		
		void tick(float deltaTime, EntityRegistry& registry) override;

		uint32_t createInputUser(const std::vector<InputAction>& actions, const std::unordered_map<std::string, std::vector<std::string>>& bindings);		void destroyInputUser(uint32_t userId);
		void assignInputGenerator(uint32_t userId, const std::weak_ptr<IInputGenerator>& generator);

		const InputAction* getAction(uint32_t userId, const std::string& name) const;

		bool addAction(uint32_t userId, const std::string& name);
		bool removeAction(uint32_t userId, const std::string& name);

		bool addBinding(uint32_t userId, const std::string& action, const std::string control);
		bool removeBinding(uint32_t userId, const std::string& action, const std::string control);

		ActionEvent onActionEvent;
	private:
		std::shared_ptr<InputUser> getInputUser(uint32_t userId) const;
		std::vector<std::shared_ptr<InputUser>> m_users;
		uint32_t userCounter = 0;
	};
}