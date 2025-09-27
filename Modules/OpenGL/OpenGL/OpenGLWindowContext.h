#pragma once

#include <string_view>

struct GLFWwindow;

namespace Mani
{
	struct OpenGLWindowContext
	{
		GLFWwindow* window = nullptr; // main window, render thread
	
		int width = 1280;
		int height = 720;
		std::string_view name = "Mani";
	};
}