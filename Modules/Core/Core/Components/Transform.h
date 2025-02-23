#pragma once

#include <Core/Core.h>
#include <ECS/Entity.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Mani
{
	struct Transform
	{
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		glm::vec3 localPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 localScale = glm::vec3(1.0f, 1.0f, 1.0f);

		ECS::EntityId parentId = ECS::INVALID_ID;

		bool hasParent() const;

		glm::mat4 calculateModelMatrix() const;

		glm::vec3 forward() const;
		glm::vec3 up() const;
		glm::vec3 right() const;
	};
}