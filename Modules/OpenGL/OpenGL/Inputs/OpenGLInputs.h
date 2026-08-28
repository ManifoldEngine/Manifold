#pragma once

#include <Core/CoreFwd.h>
#include <Inputs/Data/InputControl.h>

#include <cstdint>

struct GLFWwindow;

namespace Mani
{
	enum class EOpenGLKeyCode : uint8_t
	{
		Space = 0,
		Apostrophe,
		Comma,
		Minus,
		Period,
		Slash,
		Alpha0,
		Alpha1,
		Alpha2,
		Alpha3,
		Alpha4,
		Alpha5,
		Alpha6,
		Alpha7,
		Alpha8,
		Alpha9,
		Semicolon,
		Equal,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		LeftBracket,
		Backslash,
		RightBracket,
		GraveAccent,
		Escape,
		Enter,
		Tab,
		Backspace,
		Insert,
		Delete,
		Right,
		Left,
		Down,
		Up,
		PageUp,
		PageDown,
		Home,
		End,
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,
		NumPad0,
		NumPad1,
		NumPad2,
		NumPad3,
		NumPad4,
		NumPad5,
		NumPad6,
		NumPad7,
		NumPad8,
		NumPad9,
		NumPadDecimal,
		NumPadDivide,
		NumPadMultiply,
		NumPadSubtract,
		NumPadAdd,
		NumPadEnter,
		NumPadEqual,
		LeftShift,
		LeftControl,
		LeftAlt,
		LeftSuper,
		RightShift,
		RightControl,
		RightAlt,
		RightSuper,
		Menu,

		Count,

		// meta
		First = Space,
	};

	enum class EOpenGLMouseCode : uint8_t
	{
		Left = 0,
		Right,
		Middle,
		Button4,
		Button5,
		Button6,
		Button7,
		Button8,

		Count,

		// axis
		Mouse_Axis,

		// meta
		First = Left,
	};

	namespace OpenGLInputs
	{
		int toOpenGLCode(EOpenGLKeyCode code);
		int toOpenGLCode(EOpenGLMouseCode code);

		std::string_view toString(EOpenGLKeyCode code);
		std::string_view toString(EOpenGLMouseCode code);

		EInputHints toHint(EOpenGLKeyCode code);
		EInputHints toHint(EOpenGLMouseCode code);
	}
}