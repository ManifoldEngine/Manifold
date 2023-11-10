#pragma once

#include <OpenGL/OpenGL.h>
#include <Core/System/System.h>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace ECSEngine
{
	struct OpenGLShader;

	class OpenGLShaderSystem : public SystemBase
	{
	public:
		// todo #3: this will do for now, but we need a way to input configuration to the engine.
		struct Config
		{
			// defines the default relative path the ShaderSystem tries to get .glsl files from.
			std::vector<std::string> defaultShaderRelativePaths =
			{
				"/Engine/Assets/Shaders/",
			};

			// defines the shader extension to look for.
			std::string shaderExtension = ".glsl";
		};

		std::string_view getName() const override { return "OpenGLShaderSystem"; }

		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;

		bool loadShaderFromPath(const std::filesystem::path& filePath);
		bool loadShaderFromSource(const std::string& name, const std::string_view& vertexSource, const std::string_view& fragmentSource);

		std::shared_ptr<OpenGLShader> getShader(const std::string_view& name) const;

	private:
		std::unordered_map<std::string_view, std::shared_ptr<OpenGLShader>> m_shaders;
		Config config;
	};
}