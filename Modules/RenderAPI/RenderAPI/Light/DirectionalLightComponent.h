#pragma once

#include <glm/glm.hpp>

namespace Mani
{
	struct DirectionalLightComponent
	{
		glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);

		glm::vec3 ambient = glm::vec3(0.2f);
		glm::vec3 diffuse = glm::vec3(0.5f);
		glm::vec3 specular = glm::vec3(1.0f);
	};
}