#include "Scene.h"

#include <nlohmann/json.hpp>

using namespace Mani;
using namespace nlohmann;

void Scene::parse(const std::string_view& content) 
{
	nodes.clear();

	if (content.empty())
	{
		return;
	}

	json object = json::parse(content);
	std::vector<json> rawNodes = object["nodes"];
	for (const auto& rawNode : rawNodes)
	{
		Node node;
		node.id = rawNode["id"];
		node.meshAsset = std::filesystem::path(rawNode["meshAsset"].get<std::string>());
		
		node.localPosition.x = rawNode["localPosition"]["x"];
		node.localPosition.y = rawNode["localPosition"]["y"];
		node.localPosition.z = rawNode["localPosition"]["z"];

		node.localRotation.x = rawNode["localRotation"]["x"];
		node.localRotation.y = rawNode["localRotation"]["y"];
		node.localRotation.z = rawNode["localRotation"]["z"];
		node.localRotation.w = rawNode["localRotation"]["w"];

		node.localScale.x = rawNode["localScale"]["x"];
		node.localScale.y = rawNode["localScale"]["y"];
		node.localScale.z = rawNode["localScale"]["z"];

		nodes.push_back(node);
	}
}

std::string Scene::toJson() 
{
	json output;

	std::vector<json> rawNodes;
	for (const auto& node : nodes)
	{
		json rawNode;
		rawNode["id"] = node.id;
		rawNode["meshAsset"] = node.meshAsset;

		rawNode["localPosition"]["x"] = node.localPosition.x;
		rawNode["localPosition"]["y"] = node.localPosition.y;
		rawNode["localPosition"]["z"] = node.localPosition.z;

		rawNode["localRotation"]["x"] = node.localRotation.x;
		rawNode["localRotation"]["y"] = node.localRotation.y;
		rawNode["localRotation"]["z"] = node.localRotation.z;
		rawNode["localRotation"]["w"] = node.localRotation.w;

		rawNode["localScale"]["x"] = node.localScale.x;
		rawNode["localScale"]["y"] = node.localScale.y;
		rawNode["localScale"]["z"] = node.localScale.z;

		rawNodes.push_back(rawNode);
	}

	output["nodes"] = rawNodes;
	return output.dump();
}