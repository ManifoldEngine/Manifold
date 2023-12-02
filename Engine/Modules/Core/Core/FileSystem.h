#pragma once

#include <Core/Core.h>
#include <string>
#include <filesystem>

namespace Mani
{
	class FileSystem
	{
	public:
		// dumps a file to a string
		static bool tryReadFile(const std::filesystem::path& filePath, std::string& outResult);

		// dumps a string to a file
		static bool tryWriteFile(const std::filesystem::path& filePath, const std::string& content);
		
		// returns a path to the root of the project
		static bool tryGetRootPath(std::filesystem::path& outPath);
		// returns a path to {root}/Engine
		static bool tryGetEnginePath(std::filesystem::path& outPath);

		// finds all files with extension in a directory
		// returns their absolute path.
		static std::vector<std::filesystem::path> getAllFilesWithExtension(const std::filesystem::path& directoryPath, const std::string& extension);
	};
}