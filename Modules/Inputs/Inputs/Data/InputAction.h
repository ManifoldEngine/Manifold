#pragma once

#include <string>

namespace Mani
{
	using ActionId = Mani::SizeT;
	constexpr ActionId INVALID_INPUT_ACTION_ID = (std::numeric_limits<ActionId>::max)();

	struct InputAction
	{
		// /!\ Attention: Action ids are only unique within their input user space
		ActionId id = INVALID_INPUT_ACTION_ID;
		std::string name = "";

		float x = 0.f;
		float y = 0.f;
		float z = 0.f;

		bool wasPressed = false;
		bool isPressed = false;
		bool isEnabled = true;

		[[nodiscard]] bool changed() const { return isPressed != wasPressed; }
		[[nodiscard]] bool pressed() const { return isPressed && !wasPressed; }
		[[nodiscard]] bool held() const { return isPressed; }
		[[nodiscard]] bool released() const { return !isPressed && wasPressed; }

		[[nodiscard]] Vec3f xyz() const { return { x, y, z }; }
		[[nodiscard]] Vec2f xy() const { return { x, y }; }
		[[nodiscard]] Vec2f xz() const { return { x, z }; }
	};
}