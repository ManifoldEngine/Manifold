#pragma once

#include <string>

namespace ECSEngine
{
	struct ButtonControl
	{
		std::string name = "NONE";
		bool isPressed = false;
	};

	struct AxisControl
	{
		std::string name = "NONE";
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
	};
}