#pragma once

#include <Core/CoreFwd.h>

#include <string_view>
#include <filesystem>

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
        static bool importFromPath(const std::filesystem::path& path, List<Mesh>& outMeshes);
        static bool exportToPath(const std::filesystem::path& path, const Mesh& mesh);
    private:
        static void processNode(aiNode* node, const aiScene* scene, List<const aiMesh*>& meshesAccumulator);
        static void processMesh(const aiMesh* mesh, const aiScene* scene, Mesh& outMesh);
    };
}