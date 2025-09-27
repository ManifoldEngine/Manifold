#pragma once

#include <Core/CoreFwd.h>
#include <RenderAPI/Colors.h>
#include <ManiMaths/Fwd.h>
#include <string>

namespace Mani
{
	using FrameId = size_t;
	constexpr FrameId INVALID_FRAME_ID = UINT64_MAX;

	struct Animation
	{
		struct Frame
		{
			std::string texturePath = "";
			ECS::EntityId textureId = ECS::INVALID_ID;
			Mani::Vec4f color = Mani::Colors::WHITE;
		};

		std::string name = "";
		float duration = 0.f;	// in seconds

		std::vector<Frame> frames;
	};

	struct Animator
	{
		enum class EPlayMode : uint8_t
		{
			OneShot,
			Loop,
		};

		Mani::ECS::EntityId resourceId = Mani::ECS::INVALID_ID;
		FrameId frameId = INVALID_FRAME_ID;
		float elapsed = 0.f;			// in seconds
		float playRate = 1.f / 24.f;	// in seconds
		EPlayMode mode = EPlayMode::OneShot;

		bool shouldCull = true;
	};

	namespace AnimationStatics
	{
		void play(ECS::Registry& registry, Mani::ECS::EntityId entityId, Mani::ECS::EntityId resourceId, Animator::EPlayMode playMode = Animator::EPlayMode::OneShot);
	}
}