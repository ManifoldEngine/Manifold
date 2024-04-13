#pragma once

#include <Core/Core.h>
#include <Core/System/System.h>
#include <string_view>
#include <unordered_map>

namespace Mani
{
	enum class ELogLevel : uint8_t
	{
		Disabled,
		Verbose,
		Log,
		Warning,
		Error,
	};

	class LogSystem : public SystemBase
	{
	public:
		LogSystem();
		~LogSystem();

		std::string_view getName() const override { return "LogSystem"; }

		void log(const std::string_view& channel, ELogLevel level, const std::string_view& log);
		void setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel);

		static void s_log(const std::string_view& channel, ELogLevel level, const std::string_view& log);
		static void s_setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel);
		
	private:
		static LogSystem* s_logSystem;
		
		std::unordered_map<std::string_view, ELogLevel> channels;
		
		static void logImpl(const std::string_view& channel, ELogLevel level, const std::string_view& log);
	};
}
