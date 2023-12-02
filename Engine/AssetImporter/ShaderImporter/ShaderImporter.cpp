#include "ShaderImporter.h"

#include <Core/Log.h>
#include <Core/CoreAssert.h>
#include <Core/FileSystem.h>

#include <RenderAPI/Shader.h>

#include <Utils/StringUtils.h>

#include <filesystem>
#include <nlohmann/json.hpp>

using namespace ECSEngine;
using namespace nlohmann;

bool ShaderImporter::importFromPath(const std::filesystem::path& path, std::shared_ptr<Shader>& outShader)
{
	ECSE_ASSERT(outShader != nullptr, "provided shader cannot be null");
	if (parseShaderSourceFileFromPath(path, outShader->name, outShader->vertexSource, outShader->fragmentSource))
	{
		ECSE_LOG(LogShaderImporter, "Imported {} from path {}", outShader->name, path.string());
		return true;
	}
	return false;
}

bool ShaderImporter::exportToPath(const std::filesystem::path& path, const std::shared_ptr<Shader>& shader)
{
	ECSE_ASSERT(shader != nullptr, "provided shader cannot be null");

	json output;
	output["name"] = shader->name;
	output["vertexSource"] = shader->vertexSource;
	output["fragmentSource"] = shader->fragmentSource;

	return FileSystem::tryWriteFile(path, output.dump());
}

bool ShaderImporter::parseShaderSourceFileFromPath(const std::filesystem::path& path, std::string& outFileName, std::string& outVertexSource, std::string& outFragmentSource)
{
	std::string source;
	if (!FileSystem::tryReadFile(path, source))
	{
		ECSE_LOG_ERROR(LogShaderImporter, "Could not read source from {}", path.string());
		return false;
	}

	// token that marks the type of shader
	std::string_view typeToken = "#type ";

	// find first occurence of type token.
	size_t typeTokenIndex = source.find(typeToken);
	while (typeTokenIndex != std::string::npos)
	{
		// find the eol of the token's line
		size_t endOfLineIndex = source.find_first_of("\r\n", typeTokenIndex);
		if (endOfLineIndex == std::string::npos)
		{
			ECSE_LOG_ERROR(LogShaderImporter, "Could not parse shader type token");
			return false;
		}

		// token type starts after index + tokenType's length.
		const size_t typeNameBeginIndex = typeTokenIndex + typeToken.size();

		// extract the type name
		const std::string typeName = StringUtils::removeWhiteSpace(
			StringUtils::toLower(
				source.substr(typeNameBeginIndex, endOfLineIndex - typeNameBeginIndex)
			));

		// find following occurence of type token.
		typeTokenIndex = source.find(typeToken, endOfLineIndex);

		std::string content;
		// the shader source starts on the next index after the token's eol
		const size_t contentBeginIndex = endOfLineIndex + 1;
		if (typeTokenIndex == std::string::npos)
		{
			// if no other token are left, source is the rest of the file.
			content = source.substr(contentBeginIndex);
		}
		else
		{
			// otherwise extract the source until the next token index.
			// substr requires the length of the source. since we have the index of the follow up token, we can
			// just substract the two indices.
			content = source.substr(contentBeginIndex, typeTokenIndex - contentBeginIndex);
		}

		if (typeName == "vertex")
		{
			outVertexSource = content;
		}
		else if (typeName == "fragment")
		{
			outFragmentSource = content;
		}
		else
		{
			ECSE_LOG_ERROR(LogShaderImporter, "Incorrect shader type token format for file [{}]", path.string());
			return false;
		}
	}

	outFileName = std::filesystem::path(path).stem().string();
	return true;
}
