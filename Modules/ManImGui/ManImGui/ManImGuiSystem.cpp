#include "ManImGuiSystem.h"

#include <Core/ManiAssert.h>

#include <ManImGui/ManImGuiWindowContext.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <OpenGL/OpenGLSystem.h>
#include <OpenGL/OpenGLWindowContext.h>

#include <GLFW/glfw3.h>

using namespace Mani;

void ManImGuiSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<OpenGLSystem>();
	OpenGLWindowContext* openglContext = registry.getSingle<OpenGLWindowContext>();
	MANI_ASSERT(openglContext != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");
	
	// we create a shared context for Imgui so it can create resources on the main thread and share them with the render thread.
	ManImGuiWindowContext& context = *registry.addSingle<ManImGuiWindowContext>();
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	context.window = glfwCreateWindow(openglContext->width, openglContext->height, "imgui", NULL, openglContext->window);

	glfwMakeContextCurrent(context.window);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends. We pass the main window's context here as this is the context we should render to.
	constexpr bool install_callback = true;
	ImGui_ImplGlfw_InitForOpenGL(openglContext->window, install_callback);
	ImGui_ImplOpenGL3_Init();
	glfwMakeContextCurrent(nullptr);
}

void ManImGuiSystem::onDeinitialize(ECS::Registry& registry)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	registry.removeSingle<ManImGuiWindowContext>();
}
