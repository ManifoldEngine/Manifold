#pragma once
#include <iostream>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <OpenGL/OpenGLSystem.h>

using namespace ECSEngine;

int main(int argc, char** argv)
{
	Application app;
	app.getSystemContainer().createSystem<OpenGLSystem>();
	app.run();
	return 0;
}