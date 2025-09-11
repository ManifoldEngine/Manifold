#pragma once

#include "Position.h"
#include "Rotation.h"
#include "Scale.h"

#include <Core/ECS/Registry.h>

#include <ManiMaths/Fwd.h>

namespace Mani
{
	namespace Transform
	{
		Mat4f model(const Position& p, const Rotation& q, const Scale& s);
		Vec3f forward(const Rotation& q);
		Vec3f up(const Rotation& q);
		Vec3f right(const Rotation& q);

		Mat4f model(const Vec3f& p, const Quatf& q, const Vec3f& s);
		Vec3f forward(const Quatf& q);
		Vec3f up(const Quatf& q);
		Vec3f right(const Quatf& q);

		std::tuple<Position&, Rotation&, Scale&> addTransform(ECS::Registry& registry, ECS::EntityId entityId);
		std::tuple<Position*, Rotation*, Scale*> getTransform(ECS::Registry& registry, ECS::EntityId entityId);
	};
}