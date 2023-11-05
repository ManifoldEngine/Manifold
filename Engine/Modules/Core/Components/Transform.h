#pragma once

#include <Core/Core.h>
#include <glm/glm.hpp>

namespace ECSEngine
{
	struct Core_API Transform
	{
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		glm::mat4 calculate();
		glm::mat4 getTransform() const;

	private:
		glm::mat4 transform;
	};
}