#include "OpenGLInputs.h"

#include <glfw3.h>

using namespace Mani;

int OpenGLInputsStatics::toOpenGLCode(EOpenGLKeyCode code)
{
	switch (code)
	{
		case EOpenGLKeyCode::Space:				return GLFW_KEY_SPACE;
		case EOpenGLKeyCode::Apostrophe:		return GLFW_KEY_APOSTROPHE;
		case EOpenGLKeyCode::Comma:				return GLFW_KEY_COMMA;
		case EOpenGLKeyCode::Minus:				return GLFW_KEY_MINUS;
		case EOpenGLKeyCode::Period:			return GLFW_KEY_PERIOD;
		case EOpenGLKeyCode::Slash:				return GLFW_KEY_SLASH;

		case EOpenGLKeyCode::Alpha0:			return GLFW_KEY_0;
		case EOpenGLKeyCode::Alpha1:			return GLFW_KEY_1;
		case EOpenGLKeyCode::Alpha2:			return GLFW_KEY_2;
		case EOpenGLKeyCode::Alpha3:			return GLFW_KEY_3;
		case EOpenGLKeyCode::Alpha4:			return GLFW_KEY_4;
		case EOpenGLKeyCode::Alpha5:			return GLFW_KEY_5;
		case EOpenGLKeyCode::Alpha6:			return GLFW_KEY_6;
		case EOpenGLKeyCode::Alpha7:			return GLFW_KEY_7;
		case EOpenGLKeyCode::Alpha8:			return GLFW_KEY_8;
		case EOpenGLKeyCode::Alpha9:			return GLFW_KEY_9;
		case EOpenGLKeyCode::Semicolon:			return GLFW_KEY_SEMICOLON;
		case EOpenGLKeyCode::Equal:				return GLFW_KEY_EQUAL;

		case EOpenGLKeyCode::A:					return GLFW_KEY_A;
		case EOpenGLKeyCode::B:					return GLFW_KEY_B;
		case EOpenGLKeyCode::C:					return GLFW_KEY_C;
		case EOpenGLKeyCode::D:					return GLFW_KEY_D;
		case EOpenGLKeyCode::E:					return GLFW_KEY_E;
		case EOpenGLKeyCode::F:					return GLFW_KEY_F;
		case EOpenGLKeyCode::G:					return GLFW_KEY_G;
		case EOpenGLKeyCode::H:					return GLFW_KEY_H;
		case EOpenGLKeyCode::I:					return GLFW_KEY_I;
		case EOpenGLKeyCode::J:					return GLFW_KEY_J;
		case EOpenGLKeyCode::K:					return GLFW_KEY_K;
		case EOpenGLKeyCode::L:					return GLFW_KEY_L;
		case EOpenGLKeyCode::M:					return GLFW_KEY_M;
		case EOpenGLKeyCode::N:					return GLFW_KEY_N;
		case EOpenGLKeyCode::O:					return GLFW_KEY_O;
		case EOpenGLKeyCode::P:					return GLFW_KEY_P;
		case EOpenGLKeyCode::Q:					return GLFW_KEY_Q;
		case EOpenGLKeyCode::R:					return GLFW_KEY_R;
		case EOpenGLKeyCode::S:					return GLFW_KEY_S;
		case EOpenGLKeyCode::T:					return GLFW_KEY_T;
		case EOpenGLKeyCode::U:					return GLFW_KEY_U;
		case EOpenGLKeyCode::V:					return GLFW_KEY_V;
		case EOpenGLKeyCode::W:					return GLFW_KEY_W;
		case EOpenGLKeyCode::X:					return GLFW_KEY_X;
		case EOpenGLKeyCode::Y:					return GLFW_KEY_Y;
		case EOpenGLKeyCode::Z:					return GLFW_KEY_Z;

		case EOpenGLKeyCode::LeftBracket:		return GLFW_KEY_LEFT_BRACKET;
		case EOpenGLKeyCode::Backslash:			return GLFW_KEY_BACKSLASH;
		case EOpenGLKeyCode::RightBracket:		return GLFW_KEY_RIGHT_BRACKET;
		case EOpenGLKeyCode::GraveAccent:		return GLFW_KEY_GRAVE_ACCENT;
		case EOpenGLKeyCode::Escape:			return GLFW_KEY_ESCAPE;
		case EOpenGLKeyCode::Enter:				return GLFW_KEY_ENTER;
		case EOpenGLKeyCode::Tab:				return GLFW_KEY_TAB;
		case EOpenGLKeyCode::Backspace:			return GLFW_KEY_BACKSPACE;
		case EOpenGLKeyCode::Insert:			return GLFW_KEY_INSERT;
		case EOpenGLKeyCode::Delete:			return GLFW_KEY_DELETE;
		case EOpenGLKeyCode::Right:				return GLFW_KEY_RIGHT;
		case EOpenGLKeyCode::Left:				return GLFW_KEY_LEFT;
		case EOpenGLKeyCode::Down:				return GLFW_KEY_DOWN;
		case EOpenGLKeyCode::Up:				return GLFW_KEY_UP;
		case EOpenGLKeyCode::PageUp:			return GLFW_KEY_PAGE_UP;
		case EOpenGLKeyCode::PageDown:			return GLFW_KEY_PAGE_DOWN;
		case EOpenGLKeyCode::Home:				return GLFW_KEY_HOME;
		case EOpenGLKeyCode::End:				return GLFW_KEY_END;

		case EOpenGLKeyCode::CapsLock:			return GLFW_KEY_CAPS_LOCK;
		case EOpenGLKeyCode::ScrollLock:		return GLFW_KEY_SCROLL_LOCK;
		case EOpenGLKeyCode::NumLock:			return GLFW_KEY_NUM_LOCK;
		case EOpenGLKeyCode::PrintScreen:		return GLFW_KEY_PRINT_SCREEN;
		case EOpenGLKeyCode::Pause:				return GLFW_KEY_PAUSE;

		case EOpenGLKeyCode::F1:				return GLFW_KEY_F1;
		case EOpenGLKeyCode::F2:				return GLFW_KEY_F2;
		case EOpenGLKeyCode::F3:				return GLFW_KEY_F3;
		case EOpenGLKeyCode::F4:				return GLFW_KEY_F4;
		case EOpenGLKeyCode::F5:				return GLFW_KEY_F5;
		case EOpenGLKeyCode::F6:				return GLFW_KEY_F6;
		case EOpenGLKeyCode::F7:				return GLFW_KEY_F7;
		case EOpenGLKeyCode::F8:				return GLFW_KEY_F8;
		case EOpenGLKeyCode::F9:				return GLFW_KEY_F9;
		case EOpenGLKeyCode::F10:				return GLFW_KEY_F10;
		case EOpenGLKeyCode::F11:				return GLFW_KEY_F11;
		case EOpenGLKeyCode::F12:				return GLFW_KEY_F12;
		case EOpenGLKeyCode::F13:				return GLFW_KEY_F13;
		case EOpenGLKeyCode::F14:				return GLFW_KEY_F14;
		case EOpenGLKeyCode::F15:				return GLFW_KEY_F15;
		case EOpenGLKeyCode::F16:				return GLFW_KEY_F16;
		case EOpenGLKeyCode::F17:				return GLFW_KEY_F17;
		case EOpenGLKeyCode::F18:				return GLFW_KEY_F18;
		case EOpenGLKeyCode::F19:				return GLFW_KEY_F19;
		case EOpenGLKeyCode::F20:				return GLFW_KEY_F20;
		case EOpenGLKeyCode::F21:				return GLFW_KEY_F21;
		case EOpenGLKeyCode::F22:				return GLFW_KEY_F22;
		case EOpenGLKeyCode::F23:				return GLFW_KEY_F23;
		case EOpenGLKeyCode::F24:				return GLFW_KEY_F24;
		case EOpenGLKeyCode::F25:				return GLFW_KEY_F25;

		case EOpenGLKeyCode::NumPad0:			return GLFW_KEY_KP_0;
		case EOpenGLKeyCode::NumPad1:			return GLFW_KEY_KP_1;
		case EOpenGLKeyCode::NumPad2:			return GLFW_KEY_KP_2;
		case EOpenGLKeyCode::NumPad3:			return GLFW_KEY_KP_3;
		case EOpenGLKeyCode::NumPad4:			return GLFW_KEY_KP_4;
		case EOpenGLKeyCode::NumPad5:			return GLFW_KEY_KP_5;
		case EOpenGLKeyCode::NumPad6:			return GLFW_KEY_KP_6;
		case EOpenGLKeyCode::NumPad7:			return GLFW_KEY_KP_7;
		case EOpenGLKeyCode::NumPad8:			return GLFW_KEY_KP_8;
		case EOpenGLKeyCode::NumPad9:			return GLFW_KEY_KP_9;
		case EOpenGLKeyCode::NumPadDecimal:		return GLFW_KEY_KP_DECIMAL;
		case EOpenGLKeyCode::NumPadDivide:		return GLFW_KEY_KP_DIVIDE;
		case EOpenGLKeyCode::NumPadMultiply:	return GLFW_KEY_KP_MULTIPLY;
		case EOpenGLKeyCode::NumPadSubtract:	return GLFW_KEY_KP_SUBTRACT;
		case EOpenGLKeyCode::NumPadAdd:			return GLFW_KEY_KP_ADD;
		case EOpenGLKeyCode::NumPadEnter:		return GLFW_KEY_KP_ENTER;
		case EOpenGLKeyCode::NumPadEqual:		return GLFW_KEY_KP_EQUAL;

		case EOpenGLKeyCode::LeftShift:			return GLFW_KEY_LEFT_SHIFT;
		case EOpenGLKeyCode::LeftControl:		return GLFW_KEY_LEFT_CONTROL;
		case EOpenGLKeyCode::LeftAlt:			return GLFW_KEY_LEFT_ALT;
		case EOpenGLKeyCode::LeftSuper:			return GLFW_KEY_LEFT_SUPER;
		case EOpenGLKeyCode::RightShift:		return GLFW_KEY_RIGHT_SHIFT;
		case EOpenGLKeyCode::RightControl:		return GLFW_KEY_RIGHT_CONTROL;
		case EOpenGLKeyCode::RightAlt:			return GLFW_KEY_RIGHT_ALT;
		case EOpenGLKeyCode::RightSuper:		return GLFW_KEY_RIGHT_SUPER;
		case EOpenGLKeyCode::Menu:				return GLFW_KEY_MENU;

		case EOpenGLKeyCode::Count:
		default:
		{
			MANI_ASSERT(false, "unhandled open gl key codes");
			return 0;
		}
	}
}

int OpenGLInputsStatics::toOpenGLCode(EOpenGLMouseCode code)
{
	switch (code)
	{
		case EOpenGLMouseCode::Left:			return GLFW_MOUSE_BUTTON_LEFT;
		case EOpenGLMouseCode::Right:			return GLFW_MOUSE_BUTTON_RIGHT;
		case EOpenGLMouseCode::Middle:			return GLFW_MOUSE_BUTTON_MIDDLE;
		case EOpenGLMouseCode::Button4:			return GLFW_MOUSE_BUTTON_4;
		case EOpenGLMouseCode::Button5:			return GLFW_MOUSE_BUTTON_5;
		case EOpenGLMouseCode::Button6:			return GLFW_MOUSE_BUTTON_6;
		case EOpenGLMouseCode::Button7:			return GLFW_MOUSE_BUTTON_7;
		case EOpenGLMouseCode::Button8:			return GLFW_MOUSE_BUTTON_8;

		case EOpenGLMouseCode::Count:
		default:
		{
			MANI_ASSERT(false, "unhandled open gl mouse code");
			return 0;
		}
	}
}

std::string_view OpenGLInputsStatics::toString(EOpenGLKeyCode code)
{
	switch (code)
	{
		case EOpenGLKeyCode::Space:				return "Space";
		case EOpenGLKeyCode::Apostrophe:		return "Apostrophe";
		case EOpenGLKeyCode::Comma:				return "Comma";
		case EOpenGLKeyCode::Minus:				return "Minus";
		case EOpenGLKeyCode::Period:			return "Period";
		case EOpenGLKeyCode::Slash:				return "Slash";
		case EOpenGLKeyCode::Alpha0:			return "Alpha0";
		case EOpenGLKeyCode::Alpha1:			return "Alpha1";
		case EOpenGLKeyCode::Alpha2:			return "Alpha2";
		case EOpenGLKeyCode::Alpha3:			return "Alpha3";
		case EOpenGLKeyCode::Alpha4:			return "Alpha4";
		case EOpenGLKeyCode::Alpha5:			return "Alpha5";
		case EOpenGLKeyCode::Alpha6:			return "Alpha6";
		case EOpenGLKeyCode::Alpha7:			return "Alpha7";
		case EOpenGLKeyCode::Alpha8:			return "Alpha8";
		case EOpenGLKeyCode::Alpha9:			return "Alpha9";
		case EOpenGLKeyCode::Semicolon:			return "Semicolon";
		case EOpenGLKeyCode::Equal:				return "Equal";
		case EOpenGLKeyCode::A:					return "A";
		case EOpenGLKeyCode::B:					return "B";
		case EOpenGLKeyCode::C:					return "C";
		case EOpenGLKeyCode::D:					return "D";
		case EOpenGLKeyCode::E:					return "E";
		case EOpenGLKeyCode::F:					return "F";
		case EOpenGLKeyCode::G:					return "G";
		case EOpenGLKeyCode::H:					return "H";
		case EOpenGLKeyCode::I:					return "I";
		case EOpenGLKeyCode::J:					return "J";
		case EOpenGLKeyCode::K:					return "K";
		case EOpenGLKeyCode::L:					return "L";
		case EOpenGLKeyCode::M:					return "M";
		case EOpenGLKeyCode::N:					return "N";
		case EOpenGLKeyCode::O:					return "O";
		case EOpenGLKeyCode::P:					return "P";
		case EOpenGLKeyCode::Q:					return "Q";
		case EOpenGLKeyCode::R:					return "R";
		case EOpenGLKeyCode::S:					return "S";
		case EOpenGLKeyCode::T:					return "T";
		case EOpenGLKeyCode::U:					return "U";
		case EOpenGLKeyCode::V:					return "V";
		case EOpenGLKeyCode::W:					return "W";
		case EOpenGLKeyCode::X:					return "X";
		case EOpenGLKeyCode::Y:					return "Y";
		case EOpenGLKeyCode::Z:					return "Z";
		case EOpenGLKeyCode::LeftBracket:		return "LeftBracket";
		case EOpenGLKeyCode::Backslash:			return "Backslash";
		case EOpenGLKeyCode::RightBracket:		return "RightBracket";
		case EOpenGLKeyCode::GraveAccent:		return "GraveAccent";
		case EOpenGLKeyCode::Escape:			return "Escape";
		case EOpenGLKeyCode::Enter:				return "Enter";
		case EOpenGLKeyCode::Tab:				return "Tab";
		case EOpenGLKeyCode::Backspace:			return "Backspace";
		case EOpenGLKeyCode::Insert:			return "Insert";
		case EOpenGLKeyCode::Delete:			return "Delete";
		case EOpenGLKeyCode::Right:				return "Right";
		case EOpenGLKeyCode::Left:				return "Left";
		case EOpenGLKeyCode::Down:				return "Down";
		case EOpenGLKeyCode::Up:				return "Up";
		case EOpenGLKeyCode::PageUp:			return "PageUp";
		case EOpenGLKeyCode::PageDown:			return "PageDown";
		case EOpenGLKeyCode::Home:				return "Home";
		case EOpenGLKeyCode::End:				return "End";
		case EOpenGLKeyCode::CapsLock:			return "CapsLock";
		case EOpenGLKeyCode::ScrollLock:		return "ScrollLock";
		case EOpenGLKeyCode::NumLock:			return "NumLock";
		case EOpenGLKeyCode::PrintScreen:		return "PrintScreen";
		case EOpenGLKeyCode::Pause:				return "Pause";
		case EOpenGLKeyCode::F1:				return "F1";
		case EOpenGLKeyCode::F2:				return "F2";
		case EOpenGLKeyCode::F3:				return "F3";
		case EOpenGLKeyCode::F4:				return "F4";
		case EOpenGLKeyCode::F5:				return "F5";
		case EOpenGLKeyCode::F6:				return "F6";
		case EOpenGLKeyCode::F7:				return "F7";
		case EOpenGLKeyCode::F8:				return "F8";
		case EOpenGLKeyCode::F9:				return "F9";
		case EOpenGLKeyCode::F10:				return "F10";
		case EOpenGLKeyCode::F11:				return "F11";
		case EOpenGLKeyCode::F12:				return "F12";
		case EOpenGLKeyCode::F13:				return "F13";
		case EOpenGLKeyCode::F14:				return "F14";
		case EOpenGLKeyCode::F15:				return "F15";
		case EOpenGLKeyCode::F16:				return "F16";
		case EOpenGLKeyCode::F17:				return "F17";
		case EOpenGLKeyCode::F18:				return "F18";
		case EOpenGLKeyCode::F19:				return "F19";
		case EOpenGLKeyCode::F20:				return "F20";
		case EOpenGLKeyCode::F21:				return "F21";
		case EOpenGLKeyCode::F22:				return "F22";
		case EOpenGLKeyCode::F23:				return "F23";
		case EOpenGLKeyCode::F24:				return "F24";
		case EOpenGLKeyCode::F25:				return "F25";
		case EOpenGLKeyCode::NumPad0:			return "NumPad0";
		case EOpenGLKeyCode::NumPad1:			return "NumPad1";
		case EOpenGLKeyCode::NumPad2:			return "NumPad2";
		case EOpenGLKeyCode::NumPad3:			return "NumPad3";
		case EOpenGLKeyCode::NumPad4:			return "NumPad4";
		case EOpenGLKeyCode::NumPad5:			return "NumPad5";
		case EOpenGLKeyCode::NumPad6:			return "NumPad6";
		case EOpenGLKeyCode::NumPad7:			return "NumPad7";
		case EOpenGLKeyCode::NumPad8:			return "NumPad8";
		case EOpenGLKeyCode::NumPad9:			return "NumPad9";
		case EOpenGLKeyCode::NumPadDecimal:		return "NumPadDecimal";
		case EOpenGLKeyCode::NumPadDivide:		return "NumPadDivide";
		case EOpenGLKeyCode::NumPadMultiply:	return "NumPadMultiply";
		case EOpenGLKeyCode::NumPadSubtract:	return "NumPadSubtract";
		case EOpenGLKeyCode::NumPadAdd:			return "NumPadAdd";
		case EOpenGLKeyCode::NumPadEnter:		return "NumPadEnter";
		case EOpenGLKeyCode::NumPadEqual:		return "NumPadEqual";
		case EOpenGLKeyCode::LeftShift:			return "LeftShift";
		case EOpenGLKeyCode::LeftControl:		return "LeftControl";
		case EOpenGLKeyCode::LeftAlt:			return "LeftAlt";
		case EOpenGLKeyCode::LeftSuper:			return "LeftSuper";
		case EOpenGLKeyCode::RightShift:		return "RightShift";
		case EOpenGLKeyCode::RightControl:		return "RightControl";
		case EOpenGLKeyCode::RightAlt:			return "RightAlt";
		case EOpenGLKeyCode::RightSuper:		return "RightSuper";
		case EOpenGLKeyCode::Menu:				return "Menu";

		case EOpenGLKeyCode::Count:
		default:
		{
			MANI_ASSERT(false, "unhandled open gl key codes");
			return "";
		}
	}
}

std::string_view OpenGLInputsStatics::toString(EOpenGLMouseCode code)
{
	switch (code)
	{
		case EOpenGLMouseCode::Left:			return "Left";
		case EOpenGLMouseCode::Right:			return "Right";
		case EOpenGLMouseCode::Middle:			return "Middle";
		case EOpenGLMouseCode::Button4:			return "Button4";
		case EOpenGLMouseCode::Button5:			return "Button5";
		case EOpenGLMouseCode::Button6:			return "Button6";
		case EOpenGLMouseCode::Button7:			return "Button7";
		case EOpenGLMouseCode::Button8:			return "Button8";

		case EOpenGLMouseCode::Count:
		default:
		{
			MANI_ASSERT(false, "unhandled open gl mouse code");
			return "";
		}
	}
}

EInputHints OpenGLInputsStatics::toHint(EOpenGLKeyCode code)
{
	switch (code)
	{
		case EOpenGLKeyCode::Space:				return EInputHints::Keyboard_Space;
		case EOpenGLKeyCode::Apostrophe:		return EInputHints::Keyboard_Apostrophe;
		case EOpenGLKeyCode::Comma:				return EInputHints::Keyboard_Comma;
		case EOpenGLKeyCode::Minus:				return EInputHints::Keyboard_Minus;
		case EOpenGLKeyCode::Period:			return EInputHints::Keyboard_Period;
		case EOpenGLKeyCode::Slash:				return EInputHints::Keyboard_Slash;
		case EOpenGLKeyCode::Alpha0:			return EInputHints::Keyboard_Alpha0;
		case EOpenGLKeyCode::Alpha1:			return EInputHints::Keyboard_Alpha1;
		case EOpenGLKeyCode::Alpha2:			return EInputHints::Keyboard_Alpha2;
		case EOpenGLKeyCode::Alpha3:			return EInputHints::Keyboard_Alpha3;
		case EOpenGLKeyCode::Alpha4:			return EInputHints::Keyboard_Alpha4;
		case EOpenGLKeyCode::Alpha5:			return EInputHints::Keyboard_Alpha5;
		case EOpenGLKeyCode::Alpha6:			return EInputHints::Keyboard_Alpha6;
		case EOpenGLKeyCode::Alpha7:			return EInputHints::Keyboard_Alpha7;
		case EOpenGLKeyCode::Alpha8:			return EInputHints::Keyboard_Alpha8;
		case EOpenGLKeyCode::Alpha9:			return EInputHints::Keyboard_Alpha9;
		case EOpenGLKeyCode::Semicolon:			return EInputHints::Keyboard_Semicolon;
		case EOpenGLKeyCode::Equal:				return EInputHints::Keyboard_Equal;
		case EOpenGLKeyCode::A:					return EInputHints::Keyboard_A;
		case EOpenGLKeyCode::B:					return EInputHints::Keyboard_B;
		case EOpenGLKeyCode::C:					return EInputHints::Keyboard_C;
		case EOpenGLKeyCode::D:					return EInputHints::Keyboard_D;
		case EOpenGLKeyCode::E:					return EInputHints::Keyboard_E;
		case EOpenGLKeyCode::F:					return EInputHints::Keyboard_F;
		case EOpenGLKeyCode::G:					return EInputHints::Keyboard_G;
		case EOpenGLKeyCode::H:					return EInputHints::Keyboard_H;
		case EOpenGLKeyCode::I:					return EInputHints::Keyboard_I;
		case EOpenGLKeyCode::J:					return EInputHints::Keyboard_J;
		case EOpenGLKeyCode::K:					return EInputHints::Keyboard_K;
		case EOpenGLKeyCode::L:					return EInputHints::Keyboard_L;
		case EOpenGLKeyCode::M:					return EInputHints::Keyboard_M;
		case EOpenGLKeyCode::N:					return EInputHints::Keyboard_N;
		case EOpenGLKeyCode::O:					return EInputHints::Keyboard_O;
		case EOpenGLKeyCode::P:					return EInputHints::Keyboard_P;
		case EOpenGLKeyCode::Q:					return EInputHints::Keyboard_Q;
		case EOpenGLKeyCode::R:					return EInputHints::Keyboard_R;
		case EOpenGLKeyCode::S:					return EInputHints::Keyboard_S;
		case EOpenGLKeyCode::T:					return EInputHints::Keyboard_T;
		case EOpenGLKeyCode::U:					return EInputHints::Keyboard_U;
		case EOpenGLKeyCode::V:					return EInputHints::Keyboard_V;
		case EOpenGLKeyCode::W:					return EInputHints::Keyboard_W;
		case EOpenGLKeyCode::X:					return EInputHints::Keyboard_X;
		case EOpenGLKeyCode::Y:					return EInputHints::Keyboard_Y;
		case EOpenGLKeyCode::Z:					return EInputHints::Keyboard_Z;
		case EOpenGLKeyCode::LeftBracket:		return EInputHints::Keyboard_LeftBracket;
		case EOpenGLKeyCode::Backslash:			return EInputHints::Keyboard_Backslash;
		case EOpenGLKeyCode::RightBracket:		return EInputHints::Keyboard_RightBracket;
		case EOpenGLKeyCode::GraveAccent:		return EInputHints::Keyboard_GraveAccent;
		case EOpenGLKeyCode::Escape:			return EInputHints::Keyboard_Escape;
		case EOpenGLKeyCode::Enter:				return EInputHints::Keyboard_Enter;
		case EOpenGLKeyCode::Tab:				return EInputHints::Keyboard_Tab;
		case EOpenGLKeyCode::Backspace:			return EInputHints::Keyboard_Backspace;
		case EOpenGLKeyCode::Insert:			return EInputHints::Keyboard_Insert;
		case EOpenGLKeyCode::Delete:			return EInputHints::Keyboard_Delete;
		case EOpenGLKeyCode::Right:				return EInputHints::Keyboard_Right;
		case EOpenGLKeyCode::Left:				return EInputHints::Keyboard_Left;
		case EOpenGLKeyCode::Down:				return EInputHints::Keyboard_Down;
		case EOpenGLKeyCode::Up:				return EInputHints::Keyboard_Up;
		case EOpenGLKeyCode::PageUp:			return EInputHints::Keyboard_PageUp;
		case EOpenGLKeyCode::PageDown:			return EInputHints::Keyboard_PageDown;
		case EOpenGLKeyCode::Home:				return EInputHints::Keyboard_Home;
		case EOpenGLKeyCode::End:				return EInputHints::Keyboard_End;
		case EOpenGLKeyCode::CapsLock:			return EInputHints::Keyboard_CapsLock;
		case EOpenGLKeyCode::ScrollLock:		return EInputHints::Keyboard_ScrollLock;
		case EOpenGLKeyCode::NumLock:			return EInputHints::Keyboard_NumLock;
		case EOpenGLKeyCode::PrintScreen:		return EInputHints::Keyboard_PrintScreen;
		case EOpenGLKeyCode::Pause:				return EInputHints::Keyboard_Pause;
		case EOpenGLKeyCode::F1:				return EInputHints::Keyboard_F1;
		case EOpenGLKeyCode::F2:				return EInputHints::Keyboard_F2;
		case EOpenGLKeyCode::F3:				return EInputHints::Keyboard_F3;
		case EOpenGLKeyCode::F4:				return EInputHints::Keyboard_F4;
		case EOpenGLKeyCode::F5:				return EInputHints::Keyboard_F5;
		case EOpenGLKeyCode::F6:				return EInputHints::Keyboard_F6;
		case EOpenGLKeyCode::F7:				return EInputHints::Keyboard_F7;
		case EOpenGLKeyCode::F8:				return EInputHints::Keyboard_F8;
		case EOpenGLKeyCode::F9:				return EInputHints::Keyboard_F9;
		case EOpenGLKeyCode::F10:				return EInputHints::Keyboard_F10;
		case EOpenGLKeyCode::F11:				return EInputHints::Keyboard_F11;
		case EOpenGLKeyCode::F12:				return EInputHints::Keyboard_F12;
		case EOpenGLKeyCode::F13:				return EInputHints::Keyboard_F13;
		case EOpenGLKeyCode::F14:				return EInputHints::Keyboard_F14;
		case EOpenGLKeyCode::F15:				return EInputHints::Keyboard_F15;
		case EOpenGLKeyCode::F16:				return EInputHints::Keyboard_F16;
		case EOpenGLKeyCode::F17:				return EInputHints::Keyboard_F17;
		case EOpenGLKeyCode::F18:				return EInputHints::Keyboard_F18;
		case EOpenGLKeyCode::F19:				return EInputHints::Keyboard_F19;
		case EOpenGLKeyCode::F20:				return EInputHints::Keyboard_F20;
		case EOpenGLKeyCode::F21:				return EInputHints::Keyboard_F21;
		case EOpenGLKeyCode::F22:				return EInputHints::Keyboard_F22;
		case EOpenGLKeyCode::F23:				return EInputHints::Keyboard_F23;
		case EOpenGLKeyCode::F24:				return EInputHints::Keyboard_F24;
		case EOpenGLKeyCode::F25:				return EInputHints::Keyboard_F25;
		case EOpenGLKeyCode::NumPad0:			return EInputHints::Keyboard_NumPad0;
		case EOpenGLKeyCode::NumPad1:			return EInputHints::Keyboard_NumPad1;
		case EOpenGLKeyCode::NumPad2:			return EInputHints::Keyboard_NumPad2;
		case EOpenGLKeyCode::NumPad3:			return EInputHints::Keyboard_NumPad3;
		case EOpenGLKeyCode::NumPad4:			return EInputHints::Keyboard_NumPad4;
		case EOpenGLKeyCode::NumPad5:			return EInputHints::Keyboard_NumPad5;
		case EOpenGLKeyCode::NumPad6:			return EInputHints::Keyboard_NumPad6;
		case EOpenGLKeyCode::NumPad7:			return EInputHints::Keyboard_NumPad7;
		case EOpenGLKeyCode::NumPad8:			return EInputHints::Keyboard_NumPad8;
		case EOpenGLKeyCode::NumPad9:			return EInputHints::Keyboard_NumPad9;
		case EOpenGLKeyCode::NumPadDecimal:		return EInputHints::Keyboard_NumPadDecimal;
		case EOpenGLKeyCode::NumPadDivide:		return EInputHints::Keyboard_NumPadDivide;
		case EOpenGLKeyCode::NumPadMultiply:	return EInputHints::Keyboard_NumPadMultiply;
		case EOpenGLKeyCode::NumPadSubtract:	return EInputHints::Keyboard_NumPadSubtract;
		case EOpenGLKeyCode::NumPadAdd:			return EInputHints::Keyboard_NumPadAdd;
		case EOpenGLKeyCode::NumPadEnter:		return EInputHints::Keyboard_NumPadEnter;
		case EOpenGLKeyCode::NumPadEqual:		return EInputHints::Keyboard_NumPadEqual;
		case EOpenGLKeyCode::LeftShift:			return EInputHints::Keyboard_LeftShift;
		case EOpenGLKeyCode::LeftControl:		return EInputHints::Keyboard_LeftControl;
		case EOpenGLKeyCode::LeftAlt:			return EInputHints::Keyboard_LeftAlt;
		case EOpenGLKeyCode::LeftSuper:			return EInputHints::Keyboard_LeftSuper;
		case EOpenGLKeyCode::RightShift:		return EInputHints::Keyboard_RightShift;
		case EOpenGLKeyCode::RightControl:		return EInputHints::Keyboard_RightControl;
		case EOpenGLKeyCode::RightAlt:			return EInputHints::Keyboard_RightAlt;
		case EOpenGLKeyCode::RightSuper:		return EInputHints::Keyboard_RightSuper;
		case EOpenGLKeyCode::Menu:				return EInputHints::Keyboard_Menu;

		case EOpenGLKeyCode::Count:
		default:
		{
			return EInputHints::None;
		}
	}
}

EInputHints OpenGLInputsStatics::toHint(EOpenGLMouseCode code)
{
	switch (code)
	{
		case EOpenGLMouseCode::Left:			return EInputHints::Mouse_Left_Click;
		case EOpenGLMouseCode::Right:			return EInputHints::Mouse_Right_Click;
		case EOpenGLMouseCode::Middle:			return EInputHints::Mouse_Middle;
		case EOpenGLMouseCode::Button4:			return EInputHints::Mouse_Button4;
		case EOpenGLMouseCode::Button5:			return EInputHints::Mouse_Button5;
		case EOpenGLMouseCode::Button6:			return EInputHints::Mouse_Button6;
		case EOpenGLMouseCode::Button7:			return EInputHints::Mouse_Button7;
		case EOpenGLMouseCode::Button8:			return EInputHints::Mouse_Button8;

		case EOpenGLMouseCode::Count:
		default:
		{
			return EInputHints::None;
		}
	}
}
