#pragma once

#include <Core/CoreFwd.h>
#include <Inputs/Data/InputControl.h>
#include <OpenGL/Inputs/OpenGLInputs.h>

namespace Mani
{
	struct OpenGLKeyboard
	{
		List<ControlId> keys = List<ControlId>(static_cast<SizeT>(EOpenGLKeyCode::Count), INVALID_CONTROL_ID);
	};

	struct OpenGLMouse
	{
		List<ControlId> buttons = List<ControlId>(static_cast<SizeT>(EOpenGLMouseCode::Count), INVALID_CONTROL_ID);
	};
}