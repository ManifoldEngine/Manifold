#include "UISystem.h"

#include <Camera/Camera.h>
#include <Camera/CameraSystem.h>

#include <Resources/Resources.h>
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
	world.initializeDependency<ResourceSystem>();
	world.initializeDependency<RenderContextSystem>();
	
	const auto configId = Resources::loadSync<UIConfig>(registry, Mani::UICONFIG_PATH);
	const UIConfig& config = registry.getPinned<Resource<UIConfig>>(configId).value;

	// create UICamera
	const auto cameraId = registry.create();

	registry.addMany<Position, Rotation, UICamera>(cameraId);
	Ref<Camera> uiCamera = registry.add<Camera>(cameraId);

	const EntityId mainCameraId = Cameras::getMainCameraId(registry);
	Ref<Camera> mainCamera = registry.get<Camera>(mainCameraId);
	uiCamera->width = mainCamera->width;
	uiCamera->height = mainCamera->height;
	uiCamera->near = -1'000.f;
	uiCamera->far = 1'000.f;
	
	uiCamera->mode = ECameraMode::Orthographic;
	uiCamera->pixelsPerUnit = config.ppu;
	uiCamera->useVirtualResolution = true;
	uiCamera->virtualWidth = config.virtualWidth;
	uiCamera->virtualHeight = config.virtualHeight;

	Ref<UI::Context> context = registry.addSingle<UI::Context>();
	context->configId = configId;
	context->cameraId = cameraId;
	world.createSystem<FillableBarSystem>();
}

void UISystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Ref<UI::Context> context = registry.getSingle<UI::Context>();
	registry.destroy(context->cameraId);
	registry.removeSingle<UI::Context>();
}

void UISystem::tick(ECS::Registry& registry)
{
	Ref<RenderContext> renderContext = registry.findSingle<RenderContext>();
	MANI_ASSERT(renderContext.isValid(), "Exptexted a render context here");
	Ref<UI::Context> uiContext = registry.getSingle<UI::Context>();
	
	Ref<Camera> uiCamera = registry.get<Camera>(uiContext->cameraId);
	renderContext->shaderParameters[ShaderNames::MANI_UI_PROJECTION] = uiCamera->projection;
}
