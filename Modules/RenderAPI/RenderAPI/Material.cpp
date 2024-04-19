#include "Material.h"
#include <nlohmann/json.hpp>

using namespace Mani;
using namespace nlohmann;

void Material::parse(const std::string_view& content)
{
	json object = json::parse(content);

	name = object["name"];
	
	color.x = object["color"]["x"];
	color.y = object["color"]["y"];
	color.z = object["color"]["z"];
	color.w = object["color"]["w"];

	shaderPath = object["shaderPath"].get<std::string>();
	diffusePath = object["diffusePath"].get<std::string>();
	specularPath = object["specularPath"].get<std::string>();

	shininess = object["shininess"];
}

std::string Material::toJson()
{
	json output;
	
	output["name"] = name;
	
	output["color"]["x"] = color.x;
	output["color"]["y"] = color.y;
	output["color"]["z"] = color.z;
	output["color"]["w"] = color.w;
	
	output["shaderPath"] = shaderPath.string();
	output["diffusePath"] = diffusePath.string();
	output["specularPath"] = specularPath.string();
	output["shininess"] = shininess;

	return output.dump();
}
