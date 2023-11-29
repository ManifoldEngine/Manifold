#include "MeshImporter.h"

#include <Core/CoreAssert.h>

#include <RenderAPI/Mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <nlohmann/json.hpp>

using namespace ECSEngine;
using namespace nlohmann;

bool MeshImporter::importFromPath(const std::filesystem::path& path, std::vector<std::shared_ptr<Mesh>>& outMeshes)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == NULL)
	{
		ECSE_LOG_ERROR(LogMeshImporter, "Could not load scene: {}", std::string_view(importer.GetErrorString()));
		return false;
	}

	std::vector<const aiMesh*> meshes;
	processNode(scene->mRootNode, scene, meshes);

	for (const aiMesh* mesh : meshes)
	{
		std::shared_ptr<Mesh> loadedMesh = std::make_shared<Mesh>();
		processMesh(mesh, scene, loadedMesh);
		outMeshes.push_back(loadedMesh);
	}

	return true;
}

bool MeshImporter::exportToPath(const std::filesystem::path& path, const std::shared_ptr<Mesh>& mesh)
{
	if (mesh == nullptr)
	{
		return false;
	}

	json output;

	// export vertices
	std::vector<float> vertices;
	for (const Vertex& vertex : mesh->vertices)
	{
		vertices.push_back(vertex.position.x);
		vertices.push_back(vertex.position.y);
		vertices.push_back(vertex.position.z);

		vertices.push_back(vertex.normal.x);
		vertices.push_back(vertex.normal.y);
		vertices.push_back(vertex.normal.z);

		vertices.push_back(vertex.textureCoordinate.x);
		vertices.push_back(vertex.textureCoordinate.y);
	}

	output["vertices"] = vertices;
	
	std::vector<uint32_t> indices;
	for (const uint32_t index : indices)
	{
		indices.push_back(index);
	}
	
	output["indices"] = indices;


	
	return true;
}

void MeshImporter::processNode(aiNode* node, const aiScene* scene, std::vector<const aiMesh*>& meshesAccumulator)
{
	if (node == nullptr)
	{
		return;
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshesAccumulator.push_back(mesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshesAccumulator);
	}
}

void MeshImporter::processMesh(const aiMesh* mesh, const aiScene* scene, const std::shared_ptr<Mesh>& outMesh)
{
	ECSE_ASSERT(outMesh != nullptr, "Out pointer recipient cannot be null");

	// process vertices.
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		
		if (mesh->HasPositions())
		{
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
		}

		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTextureCoords(0))
		{
			vertex.textureCoordinate.x = mesh->mTextureCoords[0][i].x;
			vertex.textureCoordinate.y = mesh->mTextureCoords[0][i].y;
		}

		outMesh->vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			outMesh->indices.push_back(face.mIndices[j]);
		}
	}
}
