#pragma once
#include <string>
#include <glm/glm.hpp>

namespace ECSEngine
{
	struct InputAction
	{
		std::string name = "NONE";
		
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		
		bool isPressed = false;
		bool isEnabled = true;
	};
}