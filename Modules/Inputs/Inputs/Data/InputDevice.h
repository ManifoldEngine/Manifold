#pragma once

#include <vector>
#include <Inputs/Data/InputControl.h>

namespace Mani
{
	struct InputDevice
	{
		std::string deviceName;
		List<ButtonControl> buttonBuffer;
		List<AxisControl> axis;
	};
}