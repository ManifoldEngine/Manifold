#include <Core/Log.h>
#include <MeshImporter/MeshImporter.h>
#include <vector>
#include <memory>

using namespace ECSEngine;
namespace fs = std::filesystem;

struct Arguments
{
	fs::path inputPath;
	fs::path outputPath;
};

bool validateArguments(int argc, char** argv, Arguments& argument)
{
	if (argc < 3)
	{
		ECSE_LOG_ERROR(Log, "Expected 2 arguments, arg1 input path, arg2 outputpath");
		return false;
	}

	argument.inputPath = fs::path(argv[1]);
	argument.outputPath = fs::path(argv[2]);
	return true;
}

int main(int argc, char** argv)
{
	Arguments arguments;
	if (!validateArguments(argc, argv, arguments))
	{
		return EXIT_FAILURE;
	}

	std::vector<std::shared_ptr<Mesh>> meshes;
	if (!MeshImporter::importFromPath(arguments.inputPath, meshes))
	{
		return EXIT_FAILURE;
	}

	for (const std::shared_ptr<Mesh>& mesh : meshes)
	{
		MeshImporter::exportToPath(arguments.outputPath, mesh);
	}

	return EXIT_SUCCESS;
}