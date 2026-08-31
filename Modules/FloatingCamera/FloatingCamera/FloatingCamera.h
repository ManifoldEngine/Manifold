#pragma once

#include <string_view>

namespace Mani
{
    constexpr LogChannel LogFloatingCamera("FloatingCamera");

	struct FloatingCamera
	{
		float cameraSpeed = 10.f;
		float previousCameraX = 0.f;
		float previousCameraY = 0.f;
		float sensitivity = 0.1f;
	};
}