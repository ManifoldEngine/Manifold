#pragma once

#include <Core/Containers/List.h>
#include <Inputs/Data/InputControl.h>

namespace Mani
{
	struct InputDevice
	{
		std::string deviceName;

		Map<EInputHints, ControlId> buttonHints;
		List<ButtonControl> buttonBuffer;
		List<AxisControl> axis;

#if MANI_DEBUG
		Map<EInputHints, std::string_view> debug_hintTobuttonNames;
#endif
	};
}