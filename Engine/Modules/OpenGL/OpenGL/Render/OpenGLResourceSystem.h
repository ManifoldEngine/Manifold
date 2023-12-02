#pragma once

#include <Core/System/System.h>
#include <Events/Event.h>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace Mani
{
	class AssetSystem;
	class IJsonAsset;

	class OpenGLVertexArray;
	class OpenGLTexture2D;
	struct OpenGLShader;
	struct OpenGLMaterial;
	
	struct Mesh;
	struct Material;
	struct Shader;

	class OpenGLResourceSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const;

		const std::shared_ptr<OpenGLVertexArray>& getVertexArray(const std::string& name);
		const std::shared_ptr<OpenGLTexture2D>& getTexture(const std::string& name);
		const std::shared_ptr<OpenGLMaterial>& getMaterial(const std::string& name);
		const std::shared_ptr<OpenGLShader>& getShader(const std::string& name);

		void onMeshLoaded(const std::shared_ptr<Mesh>& mesh);
		void onMaterialLoaded(const std::shared_ptr<Material>& material);

	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& registry);

	private:
		EventHandle onAssetLoadedHandle;
		std::weak_ptr<AssetSystem> m_assetSystem;

		std::unordered_map<std::string, std::shared_ptr<OpenGLVertexArray>> m_vertexArrays;
		std::unordered_map<std::string, std::shared_ptr<OpenGLTexture2D>> m_textures;
		std::unordered_map<std::string, std::shared_ptr<OpenGLMaterial>> m_materials;
		std::unordered_map<std::string, std::shared_ptr<OpenGLShader>> m_shaders;

		void onJsonAssetLoaded(const std::shared_ptr<IJsonAsset>& jsonAsset);

		const std::string getOrAddTextureName(const std::filesystem::path& path);
		void onShaderLoaded(const std::shared_ptr<Shader>& shaderAsset);
	};
}