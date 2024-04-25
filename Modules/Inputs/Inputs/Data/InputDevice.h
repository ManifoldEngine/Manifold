#pragma once

#include <vector>
#include <Inputs/Data/InputControl.h>

namespace Mani
{
	struct InputDevice
	{
		std::string deviceName;
		std::vector<ButtonControl> buttonBuffer;
		std::vector<AxisControl> axis;
	};
}