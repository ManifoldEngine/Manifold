#pragma once
#include <iostream>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <OpenGL/OpenGLSystem.h>
#include <Core/Log.h>
#include <Core/World/WorldSystem.h>
#include "SandboxSystem.h"

using namespace ECSEngine;

int main(int argc, char** argv)
{
	Application app;
	SystemContainer& systemContainer = app.getSystemContainer();
	systemContainer.createSystem<OpenGLSystem>();

	std::shared_ptr<WorldSystem> pWorldSystem = systemContainer.getSystem<WorldSystem>().lock();
	std::shared_ptr<World> pWorld = pWorldSystem->createWorld();
	pWorld->getSystemContainer().createSystem<SandboxSystem>();
	
	app.run();

	systemContainer.destroySystem<OpenGLSystem>();
	return 0;
}