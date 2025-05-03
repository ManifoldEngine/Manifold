#include <ManiTests/ManiTests.h>
#include <Inputs/InputSystem.h>
#include <Inputs/Data/InputAction.h>
#include <Inputs/Data/InputControl.h>
#include <Inputs/Data/InputDevice.h>
#include <Inputs/Data/InputUser.h>
#include <Core/World.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	ManiTests::ManiTestsRunner::runTests();
}
#endif

using namespace Mani;

class VirtualControllerSystem : public ECS::System
{
public:
	virtual std::string_view getName() const override { return "VirtualControllerSystem"; }
	virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
	virtual ETickGroup getTickGroup() const override { return ETickGroup::Input; }

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
	virtual void onInitialize(ECS::Registry& registry, World& world) override
	{
		deviceId = registry.create();
		InputDevice* inputDevice = registry.add<InputDevice>(deviceId);
		inputDevice->deviceName = getDeviceName();
	}
	virtual void onDeinitialize(ECS::Registry& registry, World& world) override
	{
		registry.destroy(deviceId);
		deviceId = ECS::INVALID_ID;
		buffer.clear();

		m_aButton = { "AButton" };
		m_bButton = { "BButton" };

		m_leftStick = { "LeftStick" };
		m_rightStick = { "LeftStick" };
		m_rightBumper = { "RightBumper" };
		m_leftBumper = { "LeftBumper" };
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

	static ECS::EntityId getDeviceId() { return deviceId; }

	static void setLeftStick(float x, float y)
	{
		if (getDeviceId() == ECS::INVALID_ID)
		{
			return;
		}
		m_leftStick.x = x;
		m_leftStick.y = y;
	}

	static void setRightStick(float x, float y)
	{
		if (getDeviceId() == ECS::INVALID_ID)
		{
			return;
		}
		m_rightStick.x = x;
		m_rightStick.y = y;
	}

	static void setAButton(bool isPressed)
	{
		if (getDeviceId() == ECS::INVALID_ID)
		{
			return;
		}
		m_aButton.isPressed = isPressed;
		buffer.push_back(m_aButton);
	}

	static void setBButton(bool isPressed)
	{
		if (getDeviceId() == ECS::INVALID_ID)
		{
			return;
		}
		m_bButton.isPressed = isPressed;
		buffer.push_back(m_bButton);
	}

	static void setLeftBumper(float x)
	{
		if (getDeviceId() == ECS::INVALID_ID)
		{
			return;
		}
		m_leftBumper.x = x;
	}

	static void setRightBumper(float x)
	{
		if (getDeviceId() == ECS::INVALID_ID)
		{
			return;
		}
		m_rightBumper.x = x;
	}

	inline static std::vector<ButtonControl> buffer;
private:
	inline static ECS::EntityId deviceId = ECS::INVALID_ID;

	inline static ButtonControl m_aButton = { "AButton" };
	inline static ButtonControl m_bButton = { "BButton" };

	inline static AxisControl m_leftStick = { "LeftStick" };
	inline static AxisControl m_rightStick = { "LeftStick" };
	inline static AxisControl m_rightBumper = { "RightBumper" };
	inline static AxisControl m_leftBumper = { "LeftBumper" };
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

class InputUserMockSystem : public ECS::System
{
public:
	virtual std::string_view getName() const override { return "VirtualControllerSystem"; }
	virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
	virtual ETickGroup getTickGroup() const override { return ETickGroup::Update; }

	// never do this!
	inline static InputUser* inputUser = nullptr;
	inline static InputUser previousInputUser;
	inline static std::vector<InputAction> registeredActions;

	virtual void tick(float deltaTime, ECS::Registry& registry) override
	{
		for (const auto& [name, inputAction] : inputUser->actions)
		{
			
			bool hasChanged = false;
			hasChanged |= previousInputUser.actions[name].x != inputAction.x;
			hasChanged |= previousInputUser.actions[name].y != inputAction.y;
			hasChanged |= previousInputUser.actions[name].z != inputAction.z;
			hasChanged |= previousInputUser.actions[name].isPressed != inputAction.isPressed;
			hasChanged |= previousInputUser.actions[name].isEnabled != inputAction.isEnabled;
			if (hasChanged)
			{
				registeredActions.push_back(inputAction);
			}
		}

		previousInputUser = *inputUser;
	}

protected:
	virtual void onInitialize(ECS::Registry& registry, World& world) override
	{
		world.initializeDependency<VirtualControllerSystem>();

		ECS::EntityId entityId = registry.create();
		inputUser = registry.add<InputUser>(entityId);
		inputUser->inputDevices.push_back(VirtualControllerSystem::getDeviceId());
	}

	virtual void onDeinitialize(ECS::Registry& registry, World& world) override
	{
		inputUser = nullptr;
		previousInputUser = InputUser();
		registeredActions.clear();
	}
};

MANI_SECTION_BEGIN(Inputs, "Inputs")
{
	MANI_TEST(CreateInputUser, "Should create an input user and assign an input generator to it.")
	{
		World world;
		world.initialize();

		world.createSystem<VirtualControllerSystem>();
		world.createSystem<InputSystem>();
				
		// press A button before pluging in the virtual controller
		VirtualControllerSystem::setAButton(true);

		world.tick(.0f);

		// create a user and assign them the virtual controller.
		world.initializeDependency<InputUserMockSystem>();
		InputUserMockSystem::inputUser->actions = actionTemplate;
		InputUserMockSystem::inputUser->bindings = inputBindingsTemplate;
		std::vector<InputAction>& registeredActions = InputUserMockSystem::registeredActions;

		// press A button again
		VirtualControllerSystem::setAButton(true);
		// tick the system, consumes input buffers
		world.tick(.0f);

		MANI_TEST_ASSERT(registeredActions.size() == 1, "Should have registered an input because the button was already pressed.");

		// left stick diagonal right+up
		VirtualControllerSystem::setLeftStick(1.f, 1.f);
		// release A button
		VirtualControllerSystem::setAButton(false);
		// then press B button
		VirtualControllerSystem::setBButton(true);

		// tick the system, consumes input buffers
		world.tick(.0f);
		
		MANI_TEST_ASSERT(registeredActions.size() == 4, "Should have registered an input");
		
		InputAction thirdToLastAction = registeredActions[registeredActions.size() - 3];
		MANI_TEST_ASSERT(thirdToLastAction.name == "Jump" && thirdToLastAction.isPressed == false, "Last action should be Jump false");

		InputAction secondToLastAction = registeredActions[registeredActions.size() - 2];
		MANI_TEST_ASSERT(secondToLastAction.name == "Dodge" && secondToLastAction.isPressed == true, "Last action should be Jump false");

		InputAction lastAction = registeredActions.back();
		MANI_TEST_ASSERT(lastAction.name == "Move" && lastAction.x > 0 && lastAction.y > 0, "Last action should be Dodge true");

		world.deinitialize();
	}

	MANI_TEST(MultiAxisInteraction, "Two opposite axis on the same direction should result in zero")
	{
		World world;
		world.initialize();

		world.createSystem<VirtualControllerSystem>();
		world.createSystem<InputSystem>();
		world.createSystem<InputUserMockSystem>();

		// create a user and assign them the virtual controller.
		InputUserMockSystem::inputUser->actions =
		{
			{ "Move", { "Move" }}
		};
		
		InputUserMockSystem::inputUser->bindings =
		{
			{ "LeftStick", { "Move" }},
			{ "RightStick", { "Move" }}
		};

		// tick the system, consumes input buffers
		world.tick(.0f);

		// left stick full left
		VirtualControllerSystem::setLeftStick(-1.f, 0.f);
		// right stick full right
		VirtualControllerSystem::setRightStick(1.f, 0.f);

		// tick the system, consumes input buffers
		world.tick(.0f);

		const InputAction& moveAction = InputUserMockSystem::inputUser->actions["Move"];
		MANI_TEST_ASSERT(moveAction.x <= FLT_EPSILON, "Move X axis should be zero");

		world.deinitialize();
	}

	MANI_TEST(PressUnassignedButton, "Should press an unassigned button without side effects")
	{
		World world;
		world.initialize();

		world.createSystem<VirtualControllerSystem>();
		world.createSystem<InputSystem>();
		world.createSystem<InputUserMockSystem>();

		VirtualControllerSystem::setAButton(true);
		VirtualControllerSystem::setBButton(true);
		VirtualControllerSystem::setLeftStick(-1.f, 1.f);
		VirtualControllerSystem::setRightStick(1.f, -1.f);
		VirtualControllerSystem::setLeftBumper(1.f);
		VirtualControllerSystem::setRightBumper(1.f);

		// this should not crash.
		world.tick(0.f);
		
		MANI_TEST_ASSERT(InputUserMockSystem::registeredActions.empty(), "Action should not have been triggered.");
	
		world.deinitialize();
	}

	MANI_TEST(AssignControlToMultipleAction, "Should press receive multiple actions for a single control")
	{
		World world;
		world.initialize();

		world.createSystem<VirtualControllerSystem>();
		world.createSystem<InputSystem>();
		world.createSystem<InputUserMockSystem>();
		std::vector<InputAction>& actionEvents = InputUserMockSystem::registeredActions;
		
		// create a user and assign them the virtual controller.
		InputUserMockSystem::inputUser->actions =
		{
			{ "Jump",{ "Jump" } },
			{ "Dodge",{ "Dodge" } }
		};
		InputUserMockSystem::inputUser->bindings =
		{
			{
				"AButton",
				{
					"Jump",
					"Dodge",
				}
			}
		};

		VirtualControllerSystem::setAButton(true);
		
		// this should not crash.
		world.tick(0.f);

		MANI_TEST_ASSERT(actionEvents.size() == 2, "Should have registered 2 actions");
		MANI_TEST_ASSERT(actionEvents[0].name == "Jump", "First action should have been jump");
		MANI_TEST_ASSERT(actionEvents[1].name == "Dodge", "Second action should have been jump");

		world.deinitialize();
	}
}
MANI_SECTION_END(Inputs)