#pragma once

#include <string>

namespace Mani
{
	struct InputAction
	{
		InputAction() = default;
		InputAction(const std::string& inName) : name(inName) {};

		std::string name = "";

		double x = 0.f;
		double y = 0.f;
		double z = 0.f;

		bool wasPressed = false;
		bool isPressed = false;
		bool isEnabled = true;

		[[nodiscard]] bool changed() const { return isPressed != wasPressed; }
		[[nodiscard]] bool pressed() const { return isPressed && !wasPressed; }
		[[nodiscard]] bool released() const { return !isPressed && wasPressed; }

		[[nodiscard]] Vec3d xyz() const { return { x, y, z }; }
		[[nodiscard]] Vec2d xy() const { return { x, y }; }
		[[nodiscard]] Vec2d xz() const { return { x, z }; }
	};
}