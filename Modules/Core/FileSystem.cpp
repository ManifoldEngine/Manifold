#include "FileSystem.h"
#include <ios>
#include <fstream>
#include <Log.h>

using namespace ECSEngine;

bool FileSystem::tryReadFile(const std::string& filePath, std::string& outResult)
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
			return true;
		}
		else
		{
			ECSE_LOG_ERROR(LogCore, "{} is empty", filePath);
		}
	}
	else
	{
		ECSE_LOG_ERROR(LogCore, "Could not open file {}", filePath);
	}

	return false;
}