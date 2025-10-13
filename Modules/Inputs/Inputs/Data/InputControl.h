#pragma once

#include <string>
#include <limits>
#include <Inputs/Data/InputHints.h>

namespace Mani
{
	using ControlId = uint32_t;
	constexpr ControlId INVALID_CONTROL_ID = (std::numeric_limits<ControlId>::max)();

	struct ButtonControl
	{
		ControlId id = INVALID_CONTROL_ID;
		bool isPressed = false;

		EInputHints hint = EInputHints::None;
#if MANI_DEBUG
		std::string_view debug_name = "";
#endif
	};

	struct AxisControl
	{
		ControlId id = INVALID_CONTROL_ID;
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;

		EInputHints hint = EInputHints::None;
#if MANI_DEBUG
		std::string_view debug_name = "";
#endif
	};
}