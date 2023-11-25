#pragma once

#include <Core/System/System.h>
#include <RenderAPI/AssetId.h>
#include <unordered_map>
#include <memory>

namespace ECSEngine
{
	class OpenGLVertexArray;
	class OpenGLTexture2D;
	struct OpenGLShader;
	
	struct Mesh;
	struct Texture;
	struct Shader;

	class OpenGLResourceSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const;

		const std::shared_ptr<OpenGLVertexArray>& getVertexArray(AssetId assetId);
		const std::shared_ptr<OpenGLTexture2D>& getTexture(AssetId assetId);
		const std::shared_ptr<OpenGLShader>& getShader(AssetId assetId);

		void onMeshLoaded(const std::shared_ptr<Mesh>& mesh);
		void onTextureLoaded(const std::shared_ptr<Texture>& texture);
		void onShaderLoaded(const std::shared_ptr<Shader>& shader);

	private:
		std::unordered_map<AssetId, std::shared_ptr<OpenGLVertexArray>> m_vertexArrays;
		std::unordered_map<AssetId, std::shared_ptr<OpenGLTexture2D>> m_textures;
		std::unordered_map<AssetId, std::shared_ptr<OpenGLShader>> m_shaders;
	};
}