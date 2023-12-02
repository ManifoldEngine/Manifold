#pragma once

#include <Core/Core.h>
#include <Core/Log.h>
#include <Core/System/System.h>
#include <Core/FileSystem.h>

#include <Utils/TemplateUtils.h>

#include <nlohmann/json.hpp>
#include <memory>
#include <unordered_map>

namespace Mani
{
	struct IBaseConfig
	{
		virtual ~IBaseConfig() {};
	};

	class ConfigSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;
		
		template<Derived<IBaseConfig> TConfig>
		std::shared_ptr<TConfig> getConfig(const std::string& configName, std::filesystem::path path = "");

	private:
		std::unordered_map<std::string, std::shared_ptr<IBaseConfig>> m_configs;
	};

	template<Derived<IBaseConfig> TConfig>
	inline std::shared_ptr<TConfig> ConfigSystem::getConfig(const std::string& configName, std::filesystem::path path)
	{
		namespace fs = std::filesystem;
		using namespace nlohmann;

		if (m_configs.contains(configName))
		{
			return dynamic_pointer_cast<TConfig>(m_configs[configName]);
		}

		std::shared_ptr<TConfig> config = nullptr;

		if (path.empty())
		{
			if (!FileSystem::tryGetRootPath(path))
			{
				MANI_LOG_ERROR(LogCore, "Could not get root path.");
				return nullptr;
			}

			path = path.append("Config");
		}

		path = path.append(std::format("{}.json", configName));

		std::string content;
		if (FileSystem::tryReadFile(path, content))
		{
			// read from file
			json json = json::parse(content);
			auto object = json.template get<TConfig>();
			config = std::make_shared<TConfig>(object);
		}
		else
		{
			// defautl config
			config = std::make_shared<TConfig>();
		}

		m_configs[configName] = config;
		return config;
	}
}