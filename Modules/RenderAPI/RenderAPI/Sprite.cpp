#include "Sprite.h"
#include <nlohmann/json.hpp>

using namespace Mani;
using namespace nlohmann;

void Mani::Sprite::parse(const std::string_view& content)
{
	json object = json::parse(content);

	name = object["name"];

	shaderPath = object["shaderPath"].get<std::string>();
	texturePath = object["texturePath"].get<std::string>();
}

std::string Mani::Sprite::toJson()
{
	json output;

	output["name"] = name;
	output["shaderPath"] = shaderPath;
	output["texturePath"] = texturePath;

	return output.dump();
}
