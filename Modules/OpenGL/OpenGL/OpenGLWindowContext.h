#pragma once

#include <string_view>

struct GLFWwindow;

namespace Mani
{
	struct OpenGLWindowContext
	{
		GLFWwindow* window = nullptr; // main window, render thread
	
		int width = 800;
		int height = 800;
		std::string_view name = "Mani";
	};
}