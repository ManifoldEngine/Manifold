#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Mani
{
	struct InputAction
	{
		InputAction() = default;
		InputAction(const std::string& inName) : name(inName) {};

		std::string name = "";
		
		double x = 0.f;
		double y = 0.f;
		double z = 0.f;
		
		bool isPressed = false;
		bool isEnabled = true;
	};
}