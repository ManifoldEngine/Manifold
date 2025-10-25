#pragma once

#include <Core/Containers/Map.h>
#include <ECS/Entity.h>
#include <RenderAPI/Shader.h>

namespace Mani
{
	struct MeshRendering
	{
		ECS::EntityId meshResourceId;
		ECS::EntityId materialResourceId;
		int rendererId = 0;

		Mani::Map<std::string, ShaderType> shaderParameters;
		Mani::Map<std::string, ECS::EntityId> textureParameters;
	};
}