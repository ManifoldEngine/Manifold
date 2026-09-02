#pragma once

#include <Core/Containers/Map.h>
#include <ECS/Entity.h>
#include <RenderAPI/Shader.h>

namespace Mani
{
	struct MeshRendering
	{
		EntityId meshResourceId;
		EntityId materialResourceId;
		int rendererId = 0;

		Mani::Map<std::string_view, ShaderType> shaderParameters;
		Mani::Map<std::string_view, EntityId> textureParameters;
	};
}