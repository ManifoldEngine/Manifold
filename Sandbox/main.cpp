#pragma once
#include <iostream>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <OpenGL/OpenGLSystem.h>
#include <Core/Log.h>

using namespace ECSEngine;

int main(int argc, char** argv)
{
	Application app;
	SystemContainer& systemContainer = app.getSystemContainer();
	systemContainer.createSystem<OpenGLSystem>();
	app.run();
	systemContainer.destroySystem<OpenGLSystem>();
	return 0;
}