#pragma once

#include <OpenGL/OpenGL.h>
#include <filesystem>

namespace ECSEngine
{
	/*
	 * Extracts a shader sources from a shader source file.
	 * The expected format is as follow:
	 *
	 * #type vertex\n
	 * [vertex shader sources]
	 * #type fragment\n
	 * [fragment shader sources]
	 *
	 * returns true if the parse was successful.
	 */
	OpenGL_API bool parseShaderSourceFileFromPath(const std::filesystem::path& path, std::string& outFileName, std::string& outVertexSource, std::string& outFragmentSource);
}