#pragma once

#include <Scene/Scene.h>
#include <filesystem>
#include <memory>
#include <assimp/matrix4x4.h>

struct aiNode;
struct aiScene;

namespace Mani
{
	const std::string_view LogSceneImporter = "SceneImporter";

	struct Mesh;

	class SceneImporter
	{
	public:
		static bool importFromPath(const std::filesystem::path& path, const std::shared_ptr<Scene>& outScene);
		static bool exportToPath(const std::filesystem::path& path, const std::shared_ptr<Scene>& scene);

	private:
		static void processNode(aiNode* aiNode, const aiScene* aiScene, const std::filesystem::path path, const std::shared_ptr<Scene>& outScene);
	};
}