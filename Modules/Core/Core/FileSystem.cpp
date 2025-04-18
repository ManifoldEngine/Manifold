#include "FileSystem.h"
#include <ios>
#include <fstream>
#include <Log.h>

using namespace Mani;

bool FileSystem::readFile(const std::filesystem::path& filePath, std::string& outResult)
{
	std::ifstream in(filePath, std::ios::in | std::ios::binary);
	if (in.is_open())
	{
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size > 0)
		{
			outResult.clear();
			outResult.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&outResult[0], size);
		}
		return true;
	}
	return false;
}

bool FileSystem::writeFile(const std::filesystem::path& filePath, const std::string& content)
{
	std::ofstream out(filePath);
	if (out.is_open())
	{
		out << content;
		return true;
	}
	return false;
}

std::filesystem::path FileSystem::getRootPath()
{
#ifdef MANI_WEBGL
#elif MANI_DISTRIBUTION
	return std::filesystem::current_path();
	
#else
	return std::filesystem::path(MANI_PROJECTROOT_PATH);
#endif
}

std::filesystem::path FileSystem::getEnginePath()
{
	return getRootPath().append("Engine");
}

std::filesystem::path Mani::FileSystem::getConfigPath()
{
	return getRootPath().append("Config");
}

std::filesystem::path Mani::FileSystem::getAbsolutePath(const std::filesystem::path& relativePath)
{
	return getRootPath().append(relativePath.string());
}
