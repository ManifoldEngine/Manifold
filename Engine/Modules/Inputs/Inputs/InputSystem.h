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

	struct InputUser
	{
		uint32_t userId = 0;

		std::shared_ptr<IInputGenerator> generator = nullptr;
		std::unordered_map<std::string, InputAction> actions;
		std::unordered_map<std::string, std::string> inputBindings;
	};

	class InputSystem: public SystemBase
	{
	public:
		std::string_view getName() const override;
		bool shouldTick(EntityRegistry& registry) const override;

		void tick(float deltaTime, EntityRegistry& registry) override;

		uint32_t createInputUser();
		std::shared_ptr<InputUser> getInputUser(uint32_t userId);
		void destroyInputUser(uint32_t userId);

		void assignInputGenerator(uint32_t userId, std::shared_ptr<IInputGenerator> generator);

		ActionEvent onActionChanged;
	private:

		std::vector<std::shared_ptr<InputUser>> m_users;
		uint32_t userCounter = 0;
	};
}