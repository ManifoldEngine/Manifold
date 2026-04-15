#include <Core/Log.h>
#include <Core/FileSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/ShaderConfig.h>

#include <MeshImporter/MeshImporter.h>
#include <ShaderImporter/ShaderImporter.h>

#include <Core/Containers/List.h>

#include <ManiZ/ManiZ.h>

using namespace Mani;

namespace fs = std::filesystem;

constexpr std::string_view GLSL_EXT = ".glsl";
constexpr std::string_view SHADER_EXT = ".shader";
constexpr std::string_view FBX_EXT = ".fbx";
constexpr std::string_view MESH_EXT = ".mesh";
constexpr std::string_view PNG_EXT = ".png";

constexpr uint32_t DEFAULT_TPU = 512;

void processShader(const fs::path& path)
{
	Shader shader;
	if (!ShaderImporter::importFromPath(path, shader))
	{
		MANI_LOG_ERROR(Log, "Could not import shader at {}", path.string());
		return;
	}

	fs::path output = path.parent_path();
	output.append(std::format("{}{}", shader.name, SHADER_EXT));
	MANI_LOG(Log, "Saving {}", output.string());
	if (!ShaderImporter::exportToPath(output, shader))
	{
		MANI_LOG_ERROR(Log, "Could not save shader at {}", output.string());
	}
}

void processMesh(const fs::path& path)
{
	Mani::List<Mesh> meshes;
	if (!MeshImporter::importFromPath(path, meshes))
	{
		MANI_LOG_ERROR(Log, "Could not import mesh at {}", path.string());
		return;
	}

	for (const Mesh& mesh : meshes)
	{
		fs::path output = path.parent_path();
		output.append(std::format("{}{}", mesh.name, MESH_EXT));
		MANI_LOG(Log, "Saving {}", output.string());
		if (!MeshImporter::exportToPath(output, mesh))
		{
			MANI_LOG_ERROR(Log, "Could not save mesh at {}", output.string());
		}
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
}

void referenceAllShaders()
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

	FileSystem::foreach(FileSystem::getEnginePath(), referenceShader);
	FileSystem::foreach(FileSystem::getProjectPath(), referenceShader);

	const std::string json = ManiZ::to::json(shaders);
	const fs::path path = FileSystem::getRootPath().append(Mani::SHADERCOLLECTION_FILENAME);
	if (!FileSystem::writeFile(path, json))
	{
		MANI_LOG_ERROR(Log, "Could not write ShaderPaths to {}", path.string());
	}
}

int main(int argc, char** argv)
{
	// process assets
	FileSystem::foreach(FileSystem::getEnginePath(), &processAsset);
	FileSystem::foreach(FileSystem::getProjectPath(), &processAsset);

	// reference all shaders in a single file for startup compilation
	referenceAllShaders();

	return EXIT_SUCCESS;
}
