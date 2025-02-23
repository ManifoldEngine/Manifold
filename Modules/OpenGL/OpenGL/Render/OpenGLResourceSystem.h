#pragma once

#include <Core/CoreFwd.h>
#include <Events/Event.h>
#include <Assets/AssetDatabase.h>

#include <unordered_map>
#include <memory>
#include <filesystem>

namespace Mani
{
	class AssetSystem;
	class IAsset;

	class OpenGLVertexArray;
	class OpenGLTexture2D;
	struct OpenGLShader;
	struct OpenGLMaterial;
	struct OpenGLSprite;
	
	struct Mesh;
	struct Material;
	struct Shader;
	struct Sprite;

	const std::string QUAD_2D_NAME = "QUAD_2D";

	class OpenGLResourceSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const;

		const std::shared_ptr<OpenGLVertexArray>& getVertexArray(const std::string& name);
		const std::shared_ptr<OpenGLTexture2D>& getTexture(const std::string& name);
		const std::shared_ptr<OpenGLMaterial>& getMaterial(const std::string& name);
		const std::shared_ptr<OpenGLShader>& getShader(const std::string& name);
		const std::shared_ptr<OpenGLSprite>& getSprite(const std::string& name);
		const std::shared_ptr<OpenGLVertexArray>& getQuad(uint16_t repeatAmount);

		void onMeshLoaded(ECS::Registry& registry, const std::weak_ptr<Mesh>& mesh);
		void onMaterialLoaded(ECS::Registry& registry, const std::weak_ptr<Material>& material);
		void onSpriteLoaded(ECS::Registry& registry, const std::weak_ptr<Sprite>& sprite);


	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(ECS::Registry& registry);

	private:
		EventHandle onMeshLoadedHandle;
		EventHandle onMaterialLoadedHandle;
		EventHandle onSpriteLoadedHandle;
		
		std::unordered_map<std::string, std::shared_ptr<OpenGLVertexArray>> m_vertexArrays;
		std::unordered_map<uint16_t, std::shared_ptr<OpenGLVertexArray>> m_quadVertexArrays;
		std::unordered_map<std::string, std::shared_ptr<OpenGLTexture2D>> m_textures;
		std::unordered_map<std::string, std::shared_ptr<OpenGLMaterial>> m_materials;
		std::unordered_map<std::string, std::shared_ptr<OpenGLShader>> m_shaders;
		std::unordered_map<std::string, std::shared_ptr<OpenGLSprite>> m_sprites;

		const std::string getOrAddTextureName(const std::filesystem::path& path);
		const std::string getOrAddShaderName(ECS::Registry& registry, const std::filesystem::path& path);
		void onShaderLoaded(const std::shared_ptr<Shader>& shaderAsset);

		template<typename T>
		static AssetDatabase<T>* getOrAddDatabase(ECS::Registry& registry);
	};

	template<typename T>
	inline AssetDatabase<T>* OpenGLResourceSystem::getOrAddDatabase(ECS::Registry& registry)
	{
		if (registry.hasSingle<AssetDatabase<T>>())
		{
			return registry.getSingle<AssetDatabase<T>>();
		}
		else
		{
			return registry.addSingle<AssetDatabase<T>>();
		}
	}
}