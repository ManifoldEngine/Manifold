#pragma once

#include <Core/CoreFwd.h>
#include <RenderAPI/Colors.h>

namespace Mani
{
	namespace UI
	{
		struct Context
		{
			ECS::EntityId cameraId = ECS::INVALID_ID;
			ECS::EntityId configId = ECS::INVALID_ID;
		};

		ECS::EntityId createSprite(ECS::Registry& registry, const std::string_view& spritePath, uint8_t tag, const Vec4f& color = Colors::WHITE);
		ECS::EntityId createFillableBar(ECS::Registry& registry, const std::string_view& spritePath, uint8_t tag, const Vec4f& backgroundColor = Colors::BLACK);

		// normal device coordinate to pixel position
		Vec3f ndcToUISpace(const ECS::Registry& registry, const Vec2f& ndc);
		Vec3f ndcToUISpace(const Vec2f& ndc, const Vec2f& dimensions);

		namespace Constants
		{
			constexpr int RENDERER_ID = 1;
		}

		namespace ShaderNames
		{
			const std::string MANI_UI_CAMERA = "mani_ui_projection";
			const std::string MANI_UI_FILLRATIO = "mani_ui_fillRatio";
		}

		namespace ShaderPaths
		{
			constexpr std::string_view UI_SPRITE = "Engine/Modules/UI/Assets/Shaders/ui_sprite.shader";
			constexpr std::string_view UI_FILLABLEBAR = "Engine/Modules/UI/Assets/Shaders/ui_fillableBar.shader";
		}
	}
}
