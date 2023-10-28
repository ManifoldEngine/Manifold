#include <iostream>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <OpenGL/OpenGLSystem.h>

int main(int argc, char** argv)
{
	ECSEngine::Application app;
	app.getSystemContainer().createSystem<ECSEngine::OpenGLSystem>();
	app.run();
	return 0;
}