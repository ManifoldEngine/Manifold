#include "UISystem.h"

#include <Camera/Camera.h>
#include <Camera/CameraSystem.h>

#include <Resources/ResourceSystem.h>

#include <RenderAPI/Texture.h>
#include <RenderAPI/RenderContextSystem.h>

#include <UI/UI.h>
#include <UI/UIConfig.h>
#include <UI/UICamera.h>
#include <UI/FillableBarSystem.h>

using namespace Mani;

void UISystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<CameraSystem>();
	world.initializeDependency<RenderContextSystem>();

	UI::Context& context = *registry.addSingle<UI::Context>();
	context.configId = ResourceSystem::loadResourceSync<UIConfig>(registry, Mani::UICONFIG_PATH);
	const UIConfig& config = registry.getRef<Resource<UIConfig>>(context.configId).value;
	
	const ECS::EntityId mainCameraId = CameraStatics::getMainCameraId(registry);
	const Camera& mainCamera = registry.getRef<Camera>(mainCameraId);

	// create UICamera
	context.cameraId = registry.create();

	Camera& uiCamera = *registry.add<Camera>(context.cameraId);
	registry.add<Position>(context.cameraId);
	registry.add<Rotation>(context.cameraId);
	registry.add<UICamera>(context.cameraId);

	uiCamera.width = mainCamera.width;
	uiCamera.height = mainCamera.height;
	uiCamera.near = -1'000.f;
	uiCamera.far = 1'000.f;

	uiCamera.mode = ECameraMode::Orthographic;
	uiCamera.pixelsPerUnit = config.ppu;
	uiCamera.useVirtualResolution = true;
	uiCamera.virtualWidth = config.virtualWidth;
	uiCamera.virtualHeight = config.virtualHeight;

	world.createSystem<FillableBarSystem>();
}

void UISystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	const UI::Context& context = *registry.getSingle<UI::Context>();
	registry.destroy(context.cameraId);
	registry.removeSingle<UI::Context>();
}

void UISystem::tick(ECS::Registry& registry)
{
	RenderContext* renderContext = registry.getSingle<RenderContext>();
	MANI_ASSERT(renderContext != nullptr, "Exptexted a render context here");
	const UI::Context& uiContext = *registry.getSingle<UI::Context>();
	
	const Camera& uiCamera = registry.getRef<Camera>(uiContext.cameraId);
	renderContext->shaderParameters[UI::ShaderNames::MANI_UI_CAMERA] = uiCamera.projection;
}
