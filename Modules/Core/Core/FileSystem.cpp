#include "FileSystem.h"
#include <ios>
#include <fstream>
#include <Log.h>

using namespace Mani;

bool FileSystem::readFile(const Path& filePath, std::string& outResult)
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

bool FileSystem::readBytes(const Path& filePath, Mani::List<unsigned char>& outResult)
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
			in.read(reinterpret_cast<char*>(&outResult[0]), size);
		}
		return true;
	}
	return false;
}

bool FileSystem::writeFile(const Path& filePath, const std::string& content)
{
	std::ofstream out(filePath);
	if (out.is_open())
	{
		out << content;
		return true;
	}
	return false;
}

Path FileSystem::getRootPath()
{
#ifdef MANI_WEBGL
#elif MANI_DISTRIBUTION
	return std::filesystem::current_path();
	
#else
	return Path(MANI_PROJECTROOT_PATH);
#endif
}

Path FileSystem::getEnginePath()
{
	return getRootPath().append("Engine");
}

Path Mani::FileSystem::getConfigFilePath()
{
	return getRootPath().append(Mani::FileSystem::CONFIG_FILENAME);
}

Path Mani::FileSystem::getProjectPath()
{
	return getRootPath().append(MANI_PROJECT_NAME);
}

Path Mani::FileSystem::getAbsolutePath(const Path& relativePath)
{
	return getRootPath().append(relativePath.string());
}
