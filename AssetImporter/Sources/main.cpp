#include <Core/Log.h>
#include <Resources/ResourceSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/ShaderConfig.h>

#include <Sprite/Sprite.h>

#include <MeshImporter/MeshImporter.h>
#include <ShaderImporter/ShaderImporter.h>
#include <SpriteImporter/SpriteImporter.h>

#include <Core/Containers/List.h>

#include <memory>

using namespace Mani;

namespace fs = std::filesystem;

constexpr std::string_view GLSL_EXT = ".glsl";
constexpr std::string_view SHADER_EXT = ".shader";
constexpr std::string_view FBX_EXT = ".fbx";
constexpr std::string_view MESH_EXT = ".mesh";
constexpr std::string_view PNG_EXT = ".png";
constexpr std::string_view SPRITE_EXT = ".sprite";

struct AssetImporterConfig
{
	std::string enginePath = "Engine";
	std::string projectPath = "";
	uint32_t defaultTPU = 512;
};

AssetImporterConfig config;

void processShader(const fs::path& path)
{
	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	if (!ShaderImporter::importFromPath(path, shader))
	{
		MANI_LOG_ERROR(Log, "Could not import shader at {}", path.string());
		return;
	}

	fs::path output = path.parent_path();
	output.append(std::format("{}{}", shader->name, SHADER_EXT));
	MANI_LOG(Log, "Saving {}", output.string());
	if (!ShaderImporter::exportToPath(output, shader))
	{
		MANI_LOG_ERROR(Log, "Could not save shader at {}", output.string());
	}
}

void processMesh(const fs::path& path)
{
	Mani::List<std::shared_ptr<Mesh>> meshes;
	if (!MeshImporter::importFromPath(path, meshes))
	{
		MANI_LOG_ERROR(Log, "Could not import mesh at {}", path.string());
		return;
	}

	for (const std::shared_ptr<Mesh>& mesh : meshes)
	{
		fs::path output = path.parent_path();
		output.append(std::format("{}{}", mesh->name, MESH_EXT));
		MANI_LOG(Log, "Saving {}", output.string());
		if (!MeshImporter::exportToPath(output, mesh))
		{
			MANI_LOG_ERROR(Log, "Could not save mesh at {}", output.string());
		}
	}
}

void processSprite(const fs::path& path)
{
	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
	if (!SpriteImporter::importFromPath(path, sprite, config.defaultTPU))
	{
		return;
	}

	fs::path output = path.parent_path();
	const std::string filename = std::filesystem::path(path).stem().string();
	output.append(std::format("{}{}", filename, SPRITE_EXT));
	MANI_LOG(Log, "Saving {}", output.string());
	if (!SpriteImporter::exportToPath(output, sprite))
	{
		MANI_LOG_ERROR(Log, "Could not save sprite at {}", output.string());
	}
}

void processAsset(const fs::path& path, const fs::path& extension)
{
	if (extension == GLSL_EXT)
	{
		MANI_LOG(Log, "importing {}", path.string());
		processShader(path);
	}
	else if (extension == FBX_EXT)
	{
		MANI_LOG(Log, "importing {}", path.string());
		processMesh(path);
	}
	else if (extension == PNG_EXT)
	{
		MANI_LOG(Log, "importing {}", path.string());
		processSprite(path);
	}
}

void referenceAllShaders(const AssetImporterConfig& config, const ShaderConfig& shaderConfig)
{
	ShaderCollection shaders;

	const auto& rootPath = FileSystem::getRootPath();
	auto referenceShader = [&shaders, &rootPath](const fs::path& path, const fs::path& extension)
	{
		if (extension == SHADER_EXT)
		{
			const auto& relativePath = fs::relative(path, rootPath);
			MANI_LOG(Log, "referencing {}", relativePath.string());
			shaders.paths.add(relativePath.string());
		}
	};

	FileSystem::foreach(FileSystem::getAbsolutePath(config.enginePath), referenceShader);
	FileSystem::foreach(FileSystem::getAbsolutePath(config.projectPath), referenceShader);

	const std::string json = ManiZ::to::json(shaders);
	const fs::path path = FileSystem::getAbsolutePath(shaderConfig.shaderCollectionRelativePath).append(Mani::SHADERCOLLECTION_FILENAME);
	if (!FileSystem::writeFile(path, json))
	{
		MANI_LOG_ERROR(Log, "Could not write ShaderPaths to {}", path.string());
	}
}

int main(int argc, char** argv)
{
	Application app;
	World& world = app.getWorld();
	world.initializeDependency<ResourceSystem>();
	ECS::Registry& registry = world.getMutableRegistry();

	// load asset importer config
	const ECS::EntityId configId = ResourceSystem::loadResourceSync<AssetImporterConfig>(registry, "Config/AssetImporter.json");
	const Resource<AssetImporterConfig>& configRes = *registry.get<Resource<AssetImporterConfig>>(configId);
	MANI_ASSERT(configRes.isReady, "Could not find Config/AssetImporter.json");

	config = configRes.value;
	MANI_ASSERT(!config.enginePath.empty(), "Config engine asset path is empty");
	MANI_ASSERT(!config.projectPath.empty(), "Config project asset path is empty");

	// process assets
	FileSystem::foreach(FileSystem::getAbsolutePath(config.enginePath), &processAsset);
	FileSystem::foreach(FileSystem::getAbsolutePath(config.projectPath), &processAsset);

	// load shader config
	const std::string shaderConfigRelPath = std::format("Config/{}", Mani::SHADERCONFIG_FILENAME);
	const ECS::EntityId shaderConfigId = ResourceSystem::loadResourceSync<ShaderConfig>(registry, shaderConfigRelPath);
	const Resource<ShaderConfig>& shaderConfigRes = *registry.get<Resource<ShaderConfig>>(shaderConfigId);
	MANI_ASSERT(shaderConfigRes.isReady, "Could not find shader config at configured path");

	referenceAllShaders(config, shaderConfigRes.value);

	return EXIT_SUCCESS;
}
