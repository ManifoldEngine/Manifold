#include <ManiTests/ManiTests.h>
#include <Inputs/InputSystem.h>
#include <Inputs/Data/InputAction.h>
#include <Inputs/Data/InputControl.h>
#include <Inputs/Data/InputDevice.h>
#include <Inputs/Data/InputUser.h>
#include <Core/System/SystemContainer.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	ManiTests::ManiTestsRunner::runTests();
}
#endif

using namespace Mani;

class VirtualControllerSystem : public SystemBase
{
public:
	virtual std::string_view getName() const override { return "VirtualControllerSystem"; }
	virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
	virtual ETickGroup getTickGroup() const override { return ETickGroup::PreTick; }

	virtual void tick(float deltaTime, ECS::Registry& registry)
	{
		ECS::View<InputDevice> deviceView(registry);
		for (const ECS::EntityId entityId : deviceView)
		{
			InputDevice* device = registry.get<InputDevice>(entityId);
			consumeInputBuffer(device->buttonBuffer);
			getAxis(device->axis);
		}
	}

protected:
	virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override
	{
		deviceId = registry.create();
		InputDevice* inputDevice = registry.add<InputDevice>(deviceId);
		inputDevice->deviceName = getDeviceName();
	}
	virtual void onDeinitialize(ECS::Registry& registry) override
	{
		registry.destroy(deviceId);
	}

public:
	std::string getDeviceName() const
	{
		return "My Cool Joystick";
	}

	bool consumeInputBuffer(std::vector<ButtonControl>& outBuffer)
	{
		outBuffer = buffer;
		buffer.clear();
		return true;
	}

	bool getAxis(std::vector<AxisControl>& outAxis)
	{
		outAxis.clear();
		outAxis.push_back(m_leftStick);
		outAxis.push_back(m_rightStick);
		outAxis.push_back(m_leftBumper);
		outAxis.push_back(m_rightBumper);
		return true;
	}

	ECS::EntityId getDeviceId() const { return deviceId; }

	void setLeftStick(float x, float y)
	{
		m_leftStick.x = x;
		m_leftStick.y = y;
	}

	void setRightStick(float x, float y)
	{
		m_rightStick.x = x;
		m_rightStick.y = y;
	}

	void setAButton(bool isPressed)
	{
		m_aButton.isPressed = isPressed;
		buffer.push_back(m_aButton);
	}

	void setBButton(bool isPressed)
	{
		m_bButton.isPressed = isPressed;
		buffer.push_back(m_bButton);
	}

	void setLeftBumper(float x)
	{
		m_leftBumper.x = x;
	}

	void setRightBumper(float x)
	{
		m_rightBumper.x = x;
	}

	std::vector<ButtonControl> buffer;
private:
	ECS::EntityId deviceId;

	ButtonControl m_aButton = { "AButton" };
	ButtonControl m_bButton = { "BButton" };

	AxisControl m_leftStick = { "LeftStick" };
	AxisControl m_rightStick = { "LeftStick" };
	AxisControl m_rightBumper = { "RightBumper" };
	AxisControl m_leftBumper = { "LeftBumper" };
};

std::unordered_map<std::string, InputAction> actionTemplate =
{
	{ "Jump", { "Jump" }},
	{ "Dodge", { "Dodge" }},
	{ "Move", { "Move" }},
	{ "Shoot", { "Shoot" }},
};

std::unordered_map<std::string, std::unordered_set<std::string>> inputBindingsTemplate =
{
	{ "LeftStick", { "Move" }},
	{ "RightStick", { "Move" }},
	{ "AButton", { "Jump" }},
	{ "BButton", { "Dodge" }},
	{ "RightBumper", { "Shoot" }},
};

class InputUserMockSystem : public SystemBase
{
public:
	virtual std::string_view getName() const override { return "VirtualControllerSystem"; }
	virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
	virtual ETickGroup getTickGroup() const override { return ETickGroup::PreTick; }

	// never do this!
	InputUser* inputUser = nullptr;

protected:
	virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override
	{
		std::shared_ptr<VirtualControllerSystem> controller = systemContainer.initializeDependency<VirtualControllerSystem>().lock();

		ECS::EntityId entityId = registry.create();
		inputUser = registry.add<InputUser>(entityId);
		inputUser->inputDevices.push_back(controller->getDeviceId());
	}
};

MANI_SECTION_BEGIN(Inputs, "Inputs")
{
	MANI_TEST(CreateInputUser, "Should create an input user and assign an input generator to it.")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::vector<InputAction> registeredActions;
		std::shared_ptr<VirtualControllerSystem> controller = systemContainer.initializeDependency<VirtualControllerSystem>().lock();
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();

		EventHandle handle = inputSystem->onActionEvent.subscribe([&registeredActions](ECS::EntityId entityId, const InputAction& action) 
		{
			registeredActions.push_back(action);
		});
				
		// press A button before pluging in the virtual controller
		controller->setAButton(true);

		systemContainer.tick(.0f);

		MANI_TEST_ASSERT(registeredActions.size() == 0, "Should not have registered an input.");

		// create a user and assign them the virtual controller.
		std::shared_ptr<InputUserMockSystem> inputUser = systemContainer.initializeDependency<InputUserMockSystem>().lock();
		inputUser->inputUser->actions = actionTemplate;
		inputUser->inputUser->bindings = inputBindingsTemplate;

		// press A button again
		controller->setAButton(true);
		// tick the system, consumes input buffers
		systemContainer.tick(.0f);

		MANI_TEST_ASSERT(registeredActions.size() == 1, "Should have registered an input because the button was already pressed.");

		// left stick diagonal right+up
		controller->setLeftStick(1.f, 1.f);
		// release A button
		controller->setAButton(false);
		// then press B button
		controller->setBButton(true);

		// tick the system, consumes input buffers
		systemContainer.tick(.0f);
		
		MANI_TEST_ASSERT(registeredActions.size() == 3, "Should have registered an input");
		
		InputAction secondToLastAction = registeredActions[registeredActions.size() - 2];
		MANI_TEST_ASSERT(secondToLastAction.name == "Jump" && secondToLastAction.isPressed == false, "Last action should be Jump false");

		InputAction lastAction = registeredActions.back();
		MANI_TEST_ASSERT(lastAction.name == "Dodge" && lastAction.isPressed == true, "Last action should be Dodge true");

		inputSystem->onActionEvent.unsubscribe(handle);
		systemContainer.deinitialize();
	}

	MANI_TEST(MultiAxisInteraction, "Two opposite axis on the same direction should result in zero")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::shared_ptr<VirtualControllerSystem> controller = systemContainer.initializeDependency<VirtualControllerSystem>().lock();
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
		std::shared_ptr<InputUserMockSystem> inputUser = systemContainer.initializeDependency<InputUserMockSystem>().lock();

		// create a user and assign them the virtual controller.
		inputUser->inputUser->actions =
		{
			{ "Move", { "Move" }}
		};
		
		inputUser->inputUser->bindings =
		{
			{ "LeftStick", { "Move" }},
			{ "RightStick", { "Move" }}
		};

		// tick the system, consumes input buffers
		systemContainer.tick(.0f);

		// left stick full left
		controller->setLeftStick(-1.f, 0.f);
		// right stick full right
		controller->setRightStick(1.f, 0.f);

		// tick the system, consumes input buffers
		systemContainer.tick(.0f);

		const InputAction& moveAction = inputUser->inputUser->actions["Move"];
		MANI_TEST_ASSERT(moveAction.x <= FLT_EPSILON, "Move X axis should be zero");

		systemContainer.deinitialize();
	}

	MANI_TEST(PressUnassignedButton, "Should press an unassigned button without side effects")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		bool bHasActionBeenTriggered = false;

		std::shared_ptr<VirtualControllerSystem> controller = systemContainer.initializeDependency<VirtualControllerSystem>().lock();
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
		std::shared_ptr<InputUserMockSystem> inputUser = systemContainer.initializeDependency<InputUserMockSystem>().lock();

		EventHandle handle = inputSystem->onActionEvent.subscribe([&bHasActionBeenTriggered](ECS::EntityId entityId, const InputAction& action)
		{
			bHasActionBeenTriggered = true;
		});

		controller->setAButton(true);
		controller->setBButton(true);
		controller->setLeftStick(-1.f, 1.f);
		controller->setRightStick(1.f, -1.f);
		controller->setLeftBumper(1.f);
		controller->setRightBumper(1.f);

		// this should not crash.
		systemContainer.tick(0.f);
		
		MANI_TEST_ASSERT(bHasActionBeenTriggered == false, "Action should not have been triggered.");

		inputSystem->onActionEvent.unsubscribe(handle);
		
		systemContainer.deinitialize();
	}

	MANI_TEST(AssignControlToMultipleAction, "Should press receive multiple actions for a single control")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::vector<InputAction> actionEvents;

		std::shared_ptr<VirtualControllerSystem> controller = systemContainer.initializeDependency<VirtualControllerSystem>().lock();
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
		std::shared_ptr<InputUserMockSystem> inputUser = systemContainer.initializeDependency<InputUserMockSystem>().lock();
		EventHandle handle = inputSystem->onActionEvent.subscribe([&actionEvents](ECS::EntityId entityId, const InputAction& inputAction)
			{
				actionEvents.push_back(inputAction);
			});

		// create a user and assign them the virtual controller.
		inputUser->inputUser->actions =
		{
			{ "Jump",{ "Jump" } },
			{ "Dodge",{ "Dodge" } }
		};
		inputUser->inputUser->bindings =
		{
			{
				"AButton",
				{
					"Jump",
					"Dodge",
				}
			}
		};

		controller->setAButton(true);
		
		// this should not crash.
		systemContainer.tick(0.f);

		MANI_TEST_ASSERT(actionEvents.size() == 2, "Should have registered 2 actions");
		MANI_TEST_ASSERT(actionEvents[0].name == "Jump", "First action should have been jump");
		MANI_TEST_ASSERT(actionEvents[1].name == "Dodge", "Second action should have been jump");

		inputSystem->onActionEvent.unsubscribe(handle);

		systemContainer.deinitialize();
	}
}
MANI_SECTION_END(Inputs)