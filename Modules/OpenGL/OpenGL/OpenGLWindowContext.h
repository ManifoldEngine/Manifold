#pragma once

#include <string_view>

struct GLFWwindow;

struct OpenGLWindowContext
{
	GLFWwindow* window = nullptr; // main window, render thread
	GLFWwindow* mainThreadContext = nullptr; // shared context, main thread

	int width = 800;
	int height = 800;
	std::string_view name = "Mani";
};