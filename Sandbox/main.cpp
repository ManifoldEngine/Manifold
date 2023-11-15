#pragma once
#include <iostream>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <OpenGL/OpenGLSystem.h>
#include <Core/Log.h>
#include <Core/World/WorldSystem.h>
#include <Camera/CameraSystem.h>
#include <Inputs/InputSystem.h>

#include "SandboxSystem.h"
#include "DebugCameraControllerSystem.h"

using namespace ECSEngine;

int main(int argc, char** argv)
{
	Application app;
	SystemContainer& systemContainer = app.getSystemContainer();
	std::weak_ptr<OpenGLSystem> weakOpenGLSystem = systemContainer.initializeDependency<OpenGLSystem>();

	std::shared_ptr<WorldSystem> worldSystem = systemContainer.getSystem<WorldSystem>().lock();
	std::shared_ptr<World> world = worldSystem->createWorld();

	std::weak_ptr<InputSystem> weakInputSystem = world->getSystemContainer().initializeDependency<InputSystem>();
	if (!weakOpenGLSystem.expired() && !weakInputSystem.expired())
	{
		std::shared_ptr<OpenGLSystem> openGLSystem = weakOpenGLSystem.lock();
		std::shared_ptr<InputSystem> inputSystem = weakInputSystem.lock();
		
		inputSystem->assignInputGenerator(LOCAL_USERID, openGLSystem->getInputGenerator());
	}

	world->getSystemContainer().createSystem<CameraSystem>();
	world->getSystemContainer().createSystem<DebugCameraControllerSystem>();
	world->getSystemContainer().createSystem<SandboxSystem>();

	app.run();
	
	world->getSystemContainer().destroySystem<SandboxSystem>();
	world->getSystemContainer().destroySystem<DebugCameraControllerSystem>();
	world->getSystemContainer().destroySystem<CameraSystem>();
	world->getSystemContainer().destroySystem<InputSystem>();

	systemContainer.destroySystem<OpenGLSystem>();
	return 0;
}