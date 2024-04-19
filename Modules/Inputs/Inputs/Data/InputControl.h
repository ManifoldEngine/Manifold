#pragma once

#include <string>

namespace Mani
{
	struct ButtonControl
	{
		std::string name = "NONE";
		bool isPressed = false;
	};

	struct AxisControl
	{
		std::string name = "NONE";
		double x = 0.f;
		double y = 0.f;
		double z = 0.f;
	};
}