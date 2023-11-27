#pragma once

#include <string_view>
#include <filesystem>
#include <memory>
#include <vector>

struct aiNode;
struct aiMesh;
struct aiScene;

namespace ECSEngine
{
    const std::string_view LogMeshLoader = "MeshLoader";

    struct Mesh;

    class MeshLoader
    {
    public:
        static bool loadFromPath(const std::filesystem::path& path, std::vector<std::shared_ptr<Mesh>>& outMeshes);
    private:
        static void processNode(aiNode* node, const aiScene* scene, std::vector<const aiMesh*>& meshesAccumulator);
        static void processMesh(const aiMesh* mesh, const aiScene* scene, const std::shared_ptr<Mesh>& outMesh);
    };
}
