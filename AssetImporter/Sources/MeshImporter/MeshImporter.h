#pragma once

#include <string_view>
#include <filesystem>
#include <memory>
#include <vector>

struct aiNode;
struct aiMesh;
struct aiScene;

namespace Mani
{
    const std::string_view LogMeshImporter = "MeshImporter";

    struct Mesh;

    class MeshImporter
    {
    public:
        static bool importFromPath(const std::filesystem::path& path, std::vector<std::shared_ptr<Mesh>>& outMeshes);
        static bool exportToPath(const std::filesystem::path& path, const std::shared_ptr<Mesh>& mesh);
    private:
        static void processNode(aiNode* node, const aiScene* scene, std::vector<const aiMesh*>& meshesAccumulator);
        static void processMesh(const aiMesh* mesh, const aiScene* scene, const std::shared_ptr<Mesh>& outMesh);
    };
}