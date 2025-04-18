#pragma once

#include <Core/Core.h>
#include <string>
#include <filesystem>

namespace Mani
{
	namespace FileSystem
	{
		// dumps a file to a string
		bool readFile(const std::filesystem::path& filePath, std::string& outResult);

		// dumps a string to a file
		bool writeFile(const std::filesystem::path& filePath, const std::string& content);
		
		// returns a path to the root of the project
		std::filesystem::path getRootPath();
		// returns a path to {root}/Engine
		std::filesystem::path getEnginePath();
		// returns a path to {root}/Config
		std::filesystem::path getConfigPath();
		// returns an absolute path from a root relative path
		std::filesystem::path getAbsolutePath(const std::filesystem::path& relativePath);

		void foreach(const std::filesystem::path& path, auto&& f)
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