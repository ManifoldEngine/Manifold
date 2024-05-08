#pragma once

#include <Camera/Camera.h>
#include <Core/System/System.h>
#include <ECS/Entity.h>
#include <glm/glm.hpp>
#include <memory>

namespace Mani
{
	class EntityRegistry;
	class SystemContainer;
	struct Transform;

	struct CameraConfig
	{
		float fov = 45.f;
		float nearClipPlane = .1f;
		float farClipPlane = 10'000.f;
		float width = 800.f;
		float height = 800.f;

		float getAspectRatio() const;
	};

	struct CameraComponent
	{
		CameraConfig config;

		glm::mat4 projection;
		glm::mat4 view;
	};

	class CameraSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(ECS::Registry& registry) const override;

		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

		virtual void tick(float deltaTime, ECS::Registry& registry) override;
	
		const CameraComponent* getCameraComponent(const ECS::Registry& registry) const;
		Transform* getCameraTransform(ECS::Registry& registry) const;

		void setCameraConfig(ECS::Registry& registry, const CameraConfig& config);

		glm::vec2 worldToScreenSpace(glm::vec3 position, const ECS::Registry& registry) const;
		
	private:
		ECS::EntityId m_cameraId;
	};
}