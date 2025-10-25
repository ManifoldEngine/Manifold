#include "MeshImporter.h"

#include <Core/ManiAssert.h>
#include <Core/FileSystem.h>
#include <Core/Containers/List.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ManiZ/ManiZ.h>

using namespace Mani;

bool MeshImporter::importFromPath(const std::filesystem::path& path, Mani::List<Mesh>& outMeshes)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == NULL)
	{
		MANI_LOG_ERROR(LogMeshImporter, "Could not load scene: {}", std::string_view(importer.GetErrorString()));
		return false;
	}

	Mani::List<const aiMesh*> meshes;
	processNode(scene->mRootNode, scene, meshes);

	for (const aiMesh* mesh : meshes)
	{
		Mesh loadedMesh;
		processMesh(mesh, scene, loadedMesh);
		outMeshes.add(loadedMesh);
		MANI_LOG(LogMeshImporter, "Imported {} with {} vertices from path {}", loadedMesh.name, loadedMesh.vertices.count(), path.string());
	}

	return true;
}

bool MeshImporter::exportToPath(const std::filesystem::path& path, const Mesh& mesh)
{
	return FileSystem::writeFile(path, ManiZ::to::json(mesh));
}

void MeshImporter::processNode(aiNode* node, const aiScene* scene, Mani::List<const aiMesh*>& meshesAccumulator)
{
	if (node == nullptr)
	{
		return;
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshesAccumulator.add(mesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshesAccumulator);
	}
}

void MeshImporter::processMesh(const aiMesh* mesh, const aiScene* scene, Mesh& outMesh)
{
	outMesh.name = mesh->mName.C_Str();

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

		outMesh.vertices.add(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			outMesh.indices.add(face.mIndices[j]);
		}
	}
}
