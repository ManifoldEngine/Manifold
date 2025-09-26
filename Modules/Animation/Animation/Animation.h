#pragma once

#include <Core/CoreFwd.h>
#include <RenderAPI/Colors.h>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	using FrameId = size_t;
	constexpr FrameId INVALID_FRAME_ID = UINT64_MAX;

	struct AnimationAsset
	{
		struct Key
		{
			Mani::Vec3f position = Mani::VEC3F::ZERO;
			Mani::Quatf rotation = Mani::QUATF::IDENTITY;
			Mani::Vec3f scale = Mani::VEC3F::ZERO;

			Mani::Vec4f color = Mani::Colors::WHITE;
		};

		struct Frame
		{
			std::vector<Key> keys;
		};

		std::string name = "";
		float duration = 0.f;		// in seconds
		float tickDuration = .16f;	// in seconds

		std::vector<Frame> frames;
	};

	struct Animation
	{
		Mani::ECS::EntityId animationId = Mani::ECS::INVALID_ID;
		FrameId frameId = INVALID_FRAME_ID;
	};

	namespace AnimationStatics
	{
		void play(Mani::ECS::Registry& registry, Mani::ECS::EntityId entityId);
	}
}