#pragma once

#include <GLFW/glfw3.h>

namespace Mani
{
	enum class EManImGuiMode : uint8_t
	{
		Hidden = 0,
		Show
	};

	struct ManImGuiWindowContext
	{
		GLFWwindow* window = nullptr;
		EManImGuiMode mode = EManImGuiMode::Hidden;
	};
}