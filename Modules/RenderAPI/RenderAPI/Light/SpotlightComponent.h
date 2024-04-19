#pragma once

#include <glm/glm.hpp>

namespace Mani
{
	struct SpotlightComponent
	{
        float cutOff = glm::cos(glm::radians(12.5f));
        float outterCutOff = glm::cos(glm::radians(17.5f));

        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(0.5f);
        glm::vec3 specular = glm::vec3(1.0f);
	};
}