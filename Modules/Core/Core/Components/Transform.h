#pragma once

#include <Core/Core.h>
#include <Core/Interfaces/IJsonAsset.h>
#include <ECS/Entity.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Mani
{
	struct Transform : public IJsonAsset
	{
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		glm::vec3 localPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 localScale = glm::vec3(1.0f, 1.0f, 1.0f);

		EntityId parentId = INVALID_ID;

		bool hasParent() const;

		glm::mat4 calculateModelMatrix() const;
	
		glm::vec3 forward() const;
		glm::vec3 up() const;
		glm::vec3 right() const;

		virtual void parse(const std::string_view& content) override;
		virtual std::string toJson() override;
	};
}