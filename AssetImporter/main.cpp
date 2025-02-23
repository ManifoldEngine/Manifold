#include <Core/Log.h>
#include <Assets/AssetSystem.h>

#include <Scene/Scene.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Shader.h>

#include <MeshImporter/MeshImporter.h>
#include <SceneImporter/SceneImporter.h>
#include <ShaderImporter/ShaderImporter.h>

#include <vector>
#include <memory>

using namespace Mani;

namespace fs = std::filesystem;

struct AssetImporterConfig
{
	std::string engineAssetPath = "Engine/Assets";
	std::string projectAssetPath = "";
};

class AssetImporterSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "AssetImporterSystem"; }
	virtual bool shouldTick(ECS::Registry& registry) const override { return false; }

protected:
	virtual void onInitialize(ECS::Registry& registry, Mani::SystemContainer& systemContainer) override
	{
		std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();
		std::weak_ptr<AssetImporterConfig> weakConfig = AssetSystem::loadAsset<AssetImporterConfig>(registry, "Config/AssetImporter.json");
		MANI_ASSERT(!weakConfig.expired(), "Could not find Config/AssetImporter.json");

		std::shared_ptr<AssetImporterConfig> config = weakConfig.lock();
		fs::path rootPath;
		MANI_ASSERT(FileSystem::tryGetRootPath(rootPath), "Could not get root path");

		if (!config->engineAssetPath.empty())
		{
			fs::path engineAssetPath = fs::path(rootPath).append(config->engineAssetPath);
			exploreDirectory(engineAssetPath);
		}

		if (!config->projectAssetPath.empty())
		{
			fs::path projectAssetPath = fs::path(rootPath).append(config->projectAssetPath);
			exploreDirectory(projectAssetPath);
		}
	}

private:
	void exploreDirectory(const fs::path& path)
	{
		for (const auto& entry : fs::recursive_directory_iterator(path))
		{
			if (entry.is_directory() || !entry.is_regular_file())
			{
				continue;
			}

			fs::path extension = entry.path().extension();

			if (extension == ".glsl")
			{
				MANI_LOG(Log, "importing {}", entry.path().string());
				importShader(entry.path());
			}
			else if (extension == ".fbx")
			{
				MANI_LOG(Log, "importing {}", entry.path().string());
				importModel(entry.path());
			}
		}
	}

	void importModel(const fs::path& path)
	{
		std::vector<std::shared_ptr<Mesh>> meshes;
		if (!MeshImporter::importFromPath(path, meshes))
		{
			MANI_LOG_ERROR(Log, "Could not import mesh at {}", path.string());
			return;
		}

		for (const std::shared_ptr<Mesh>& mesh : meshes)
		{
			fs::path output = path.parent_path();
			output.append(std::format("{}{}", mesh->name, ".mesh"));
			MANI_LOG(Log, "Saving {}", output.string());
			if (!MeshImporter::exportToPath(output, mesh))
			{
				MANI_LOG_ERROR(Log, "Could not save mesh at {}", output.string());
			}
		}

		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		if (!SceneImporter::importFromPath(path, scene))
		{
			MANI_LOG_ERROR(Log, "Could not import scene from {}", path.string());
			return;
		}

		{
			fs::path output = path.parent_path();
			output.append(std::format("{}{}", path.stem().string(), ".scene"));
			MANI_LOG(Log, "Saving {}", output.string());
			if (!SceneImporter::exportToPath(output, scene))
			{
				MANI_LOG_ERROR(Log, "Could not save scene at {}", path.string());
			}
		}
	}

	void importShader(const fs::path& path)
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();
		if (!ShaderImporter::importFromPath(path, shader))
		{
			MANI_LOG_ERROR(Log, "Could not import shader at {}", path.string());
			return;
		}

		fs::path output = path.parent_path();
		output.append(std::format("{}{}", shader->name, ".shader"));
		MANI_LOG(Log, "Saving {}", output.string());
		if (!ShaderImporter::exportToPath(output, shader))
		{
			MANI_LOG_ERROR(Log, "Could not save shader at {}", output.string());
		}
	}
};

int main(int argc, char** argv)
{
	Application app;
	app.getSystemContainer().createSystem<AssetImporterSystem>();

	return EXIT_SUCCESS;
}


