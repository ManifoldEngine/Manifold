#pragma once

#include <Core/CoreFwd.h>
#include <RenderAPI/Colors.h>
#include <Camera/Camera.h>
#include <Resources/Resources.h>

namespace Mani
{
	namespace UI
	{
		struct Context
		{
			ECS::EntityId cameraId = ECS::INVALID_ID;
			ECS::EntityId configId = ECS::INVALID_ID;
		};

		Ref<const Camera> getUICamera(const ECS::Registry& registry);
		ECS::EntityId createSprite(ECS::Registry& registry, const Vec2f& size, ECS::EntityId textureId, ECS::EntityId materialId, uint32_t tag = Mani::GLOBAL_RESOURCE_TAG);
		ECS::EntityId createFillableBar(ECS::Registry& registry, const Vec2f& size, ECS::EntityId textureId, ECS::EntityId materialId, const Vec4f& backgroundColor = Colors::BLACK, uint32_t tag = Mani::GLOBAL_RESOURCE_TAG);

		// normal device coordinate to pixel position
		// use these to place ui elements in [-1, 1] coordinates
		Vec3f ndcToUISpace(const Vec2f& ndc, const Camera& uiCamera);
		Vec3f ndcToUISpace(const Vec2f& ndc, const Vec2f& dimensions);

		namespace ShaderNames
		{
			const std::string MANI_UI_FILLRATIO = "mani_ui_fillRatio";
		}

		namespace ShaderPaths
		{
			constexpr std::string_view UI_SPRITE = "Engine/Modules/UI/Assets/Shaders/ui_sprite.shader";
			constexpr std::string_view UI_FILLABLEBAR = "Engine/Modules/UI/Assets/Shaders/ui_fillableBar.shader";
		}
	}
}
