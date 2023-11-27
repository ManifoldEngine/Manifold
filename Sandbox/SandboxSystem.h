#pragma once

#include <Core/System/System.h>
#include <ECS/Entity.h>
#include <Events/Event.h>
#include <memory>

namespace ECSEngine
{
	class EntityRegistry;
	class CameraSystem;
	class InputSystem;
	struct InputAction;
}

class SandboxSystem : public ECSEngine::SystemBase
{
public:
	virtual void tick(float deltaTime, ECSEngine::EntityRegistry& registry) override;

protected:
	virtual bool shouldTick(ECSEngine::EntityRegistry& registry) const override { return true; }
	virtual std::string_view getName() const override { return "SandboxSystem"; }

	virtual void onInitialize(ECSEngine::EntityRegistry& registry, ECSEngine::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(ECSEngine::EntityRegistry& registry) override;

private:
	std::weak_ptr<ECSEngine::CameraSystem> m_cameraSystem;
	std::weak_ptr<ECSEngine::InputSystem> m_inputSystem;

	ECSEngine::EntityId m_spotlightEntityId;
	ECSEngine::EventHandle onActionEventHandle;

	bool m_isFlashlightOn = false;

	void onActionEvent(uint32_t userId, const ECSEngine::InputAction& action);


};

