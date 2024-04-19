#pragma once

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <memory>
#include <vector>

namespace Mani
{
	struct MeshComponent
	{
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};
}