#pragma once

#include <Core/Core.h>
#include <Core/Containers/List.h>
#include <Core/ManiTypes.h>
#include <string>

namespace Mani
{
	namespace FileSystem
	{
		constexpr std::string_view CONFIG_FILENAME = "Config.json";

		// dumps a file to a string
		bool readFile(const Path& filePath, std::string& outResult);
		// dumps a file to list of bytes
		bool readBytes(const Path& filePath, Mani::List<unsigned char>& outResult);

		// dumps a string to a file
		bool writeFile(const Path& filePath, const std::string& content);
		
		// returns a path to the root of the project
		Path getRootPath();
		// returns a path to {root}/Engine
		Path getEnginePath();
		// returns a path to config file
		Path getConfigFilePath();
		// returns a path to {}/{projectName}
		// it is expexted that the project name is defined in the build files
		Path getProjectPath();

		// returns an absolute path from a root relative path
		Path getAbsolutePath(const Path& relativePath);

		void foreach(const Path& path, auto&& f)
		{
			namespace fs = std::filesystem;
			for (const auto& entry : fs::recursive_directory_iterator(path))
			{
				if (entry.is_directory() || !entry.is_regular_file())
				{
					continue;
				}
				const fs::path& entryPath = entry.path();
				const fs::path extension = entryPath.extension();
				f(entryPath, extension);
			}
		}
	};
}