#include "ManImGuiSystem.h"

#include <Core/ManiAssert.h>

#include <Inputs/Data/InputDevice.h>
#include <Inputs/Data/InputUser.h>
#include <Inputs/InputSystem.h>
#include <Inputs/Cursor.h>

#include <ManImGui/ManImGuiWindowContext.h>
#include <ManImGui/ManImGuiRenderSystem.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <OpenGL/OpenGLSystem.h>
#include <OpenGL/OpenGLWindowContext.h>

#include <GLFW/glfw3.h>

using namespace Mani;


const std::string TOGGLE_MANIMGUI = "TOGGLE_MANIMGUI";

struct ManImGuiUser
{
	Cursor::EMode modeCache = Cursor::EMode::Disabled;
};

void handleInputs(ECS::Registry& registry, ManImGuiWindowContext& context, ManImGuiUser& manImguiUser, const InputAction& toggleManImGui)
{
	if (!toggleManImGui.changed() || !toggleManImGui.isPressed)
	{
		return;
	}

	Cursor& cursor = *registry.getSingle<Cursor>();

	switch (context.mode)
	{
		case EManImGuiMode::Show:
		{
			context.mode = EManImGuiMode::Hidden;
			cursor.mode = manImguiUser.modeCache;
			break;
		}

		case EManImGuiMode::Hidden:
		{
			context.mode = EManImGuiMode::Show;
			manImguiUser.modeCache = cursor.mode;
			cursor.mode = Cursor::EMode::Normal;
			break;
		}
		default: break;
	}
}

void ManImGuiSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<OpenGLSystem>();
	OpenGLWindowContext* openglContext = registry.getSingle<OpenGLWindowContext>();
	MANI_ASSERT(openglContext != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");
	
	// we create a shared context for Imgui so it can create resources on the main thread and share them with the render thread.
	ManImGuiWindowContext& context = *registry.addSingle<ManImGuiWindowContext>();
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	GLFWwindow* previousContext = glfwGetCurrentContext();
	context.window = glfwCreateWindow(openglContext->width, openglContext->height, "imgui", NULL, openglContext->window);

	// setup imgui
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
	glfwMakeContextCurrent(previousContext);

	// create manimgui systems
	world.createSystem<ManImGuiRenderSystem>();

	{
		world.initializeDependency<InputSystem>();
		const ECS::EntityId entityId = registry.create();

		registry.add<ManImGuiUser>(entityId);

		// add 
		InputUser& inputUser = *registry.add<InputUser>(entityId);
		inputUser.setAction(TOGGLE_MANIMGUI);
		inputUser.addBinding("F7", TOGGLE_MANIMGUI);

		for (const auto deviceId : ECS::View<InputDevice>(registry))
		{
			inputUser.inputDevices.add(deviceId);
		}
	}
}

void ManImGuiSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	registry.removeSingle<ManImGuiWindowContext>();

	for (const auto entityId : ECS::View<ManImGuiUser, InputUser>(registry))
	{
		registry.deferDestroy(entityId);
	}
}

void ManImGuiSystem::tick(ECS::Registry& registry)
{
	ManImGuiWindowContext& context = *registry.getSingle<ManImGuiWindowContext>();
	for (const auto entityId : ECS::View<ManImGuiUser, InputUser>(registry))
	{
		auto [imguiUser, inputUser] = registry.getMany<ManImGuiUser, InputUser>(entityId);
		const InputAction& toggleManImGui = inputUser->actions.get(TOGGLE_MANIMGUI);
		handleInputs(registry, context, *imguiUser, toggleManImGui);
		break;
	}

	if (context.mode == EManImGuiMode::Hidden)
	{
		return;
	}

	GLFWwindow* previousContext = glfwGetCurrentContext();
	glfwMakeContextCurrent(context.window);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	glfwMakeContextCurrent(previousContext);
}
