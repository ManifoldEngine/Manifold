#include "Shader.h"
#include <nlohmann/json.hpp>

using namespace Mani;
using namespace nlohmann;

void Shader::parse(const std::string_view& content)
{
	name = "";
	vertexSource = "";
	fragmentSource = "";

	json object = json::parse(content);

	name = object["name"];
	vertexSource = object["vertexSource"];
	fragmentSource = object["fragmentSource"];
}

std::string Shader::toJson()
{
	json output;
	output["name"] = name;
	output["vertexSource"] = vertexSource;
	output["fragmentSource"] = fragmentSource;
	return output.dump();
}
