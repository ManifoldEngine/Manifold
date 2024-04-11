
#include "simpleTests.h"
#include <Core/World/World.h>
#include <Core/ConfigSystem.h>
#include <string>
#include <filesystem>

using namespace Mani;

ST_SECTION_BEGIN(ConfigSystemSection, "ConfigSystem")
{
	struct SomeConfig : IBaseConfig
	{
		virtual ~SomeConfig() override {};

		float configValue = 50.0f;
		std::string configPath = "/Some/Path/";
		uint32_t configAmount = 10'000;
	};

	void from_json(const nlohmann::json & json, SomeConfig & object)
	{
		json.at("configValue").get_to(object.configValue);
		json.at("configPath").get_to(object.configPath);
		json.at("configAmount").get_to(object.configAmount);
	}

	ST_TEST(GetDefaultConfigObject, "Should get a default config object")
	{
	
		World world;
		std::shared_ptr<ConfigSystem> configSystem = world.getSystemContainer().initializeDependency<ConfigSystem>().lock();
		// No Config/SomeSystem.json is defined so the system returns a the default object.
		std::shared_ptr<SomeConfig> config = configSystem->getConfig<SomeConfig>("SomeSystem");

		if (config == nullptr)
		{
			ST_ASSERT(false, "config should not be null");
			return;
		}

		ST_ASSERT(std::abs(config->configValue - 50.0f) <= 1.192092896e-07F, "Should be equal to the default value.");
		ST_ASSERT(config->configPath == "/Some/Path/", "Should be equal to the default value.");
		ST_ASSERT(config->configAmount == 10'000, "Should be equal to the default value.");
	}

	ST_TEST(GetCustomConfigObject, "Should get a custom config object from a json config file")
	{

		World world;
		std::shared_ptr<ConfigSystem> configSystem = world.getSystemContainer().initializeDependency<ConfigSystem>().lock();
		std::filesystem::path path;
		if (!FileSystem::tryGetEnginePath(path))
		{
			ST_ASSERT(false, "should get engine path");
			return;
		}
		path = path.append("Modules/Core/Tests/Config/");
		// a [path]/SomeSystem.json file is, so the system should return the values defined by the file.
		std::shared_ptr<SomeConfig> config = configSystem->getConfig<SomeConfig>("SomeSystem", path);
		if (config == nullptr)
		{
			ST_ASSERT(false, "config should not be null");
			return;
		}

		ST_ASSERT(std::abs(config->configValue - 1000.0f) <= 1.192092896e-07F, "Should be equal to the custom value.");
		ST_ASSERT(config->configPath == "/Some/Path/SubPath/", "Should be equal to the custom value.");
		ST_ASSERT(config->configAmount == 10'000'000, "Should be equal to the custom value.");
	}
}
ST_SECTION_END(ConfigSystemSection)
