#pragma once

#include <Core/CoreFwd.h>
#include <Font.h>

namespace Mani
{
	namespace Text
	{
		void addRendering(ECS::Registry& registry, EntityId entityId, EntityId fontId, EntityId materialId, float height, std::string_view text);
	}
}