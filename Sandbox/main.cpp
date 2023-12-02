#include <Core/Application.h>
#include <Core/Log.h>

#include <Core/System/SystemContainer.h>
#include <Core/World/WorldSystem.h>

#include <OpenGL/OpenGLSystem.h>

#include <Camera/CameraSystem.h>

#include <FloatingCamera/FloatingCameraControllerSystem.h>

#include <Inputs/InputSystem.h>

#include "SandboxSystem.h"

using namespace ECSEngine;

int main(int argc, char** argv)
{
	Application app;

	SystemContainer& systemContainer = app.getSystemContainer();
	systemContainer.createSystem<OpenGLSystem>();

	std::shared_ptr<WorldSystem> worldSystem = systemContainer.getSystem<WorldSystem>().lock();
	std::shared_ptr<World> world = worldSystem->createWorld();
	SystemContainer& worldSystemContainer = world->getSystemContainer();

	worldSystemContainer.createSystem<CameraSystem>()
		.createSystem<InputSystem>()
		.createSystem<FloatingCameraControllerSystem>()
		.createSystem<SandboxSystem>();

	std::weak_ptr<OpenGLSystem> weakOpenGLSystem = systemContainer.getSystem<OpenGLSystem>();
	std::weak_ptr<InputSystem> weakInputSystem = worldSystemContainer.getSystem<InputSystem>();
	if (!weakOpenGLSystem.expired() && !weakInputSystem.expired())
	{
		std::shared_ptr<OpenGLSystem> openGLSystem = weakOpenGLSystem.lock();
		std::shared_ptr<InputSystem> inputSystem = weakInputSystem.lock();

		inputSystem->assignInputGenerator(LOCAL_USERID, openGLSystem->getInputGenerator());
	}

	app.run();

	return 0;
}