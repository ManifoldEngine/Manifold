#pragma once

#include <ECS/Entity.h>
#include <RenderAPI/Shader.h>
#include <unordered_map>

namespace Mani
{
	struct MeshComponent
	{
		ECS::EntityId meshHandle;
		ECS::EntityId materialHandle;
		int rendererId = 0;

		std::unordered_map<std::string, ShaderType> shaderParameters;
		std::unordered_map<std::string, ECS::EntityId> textureParameters;
	};
}