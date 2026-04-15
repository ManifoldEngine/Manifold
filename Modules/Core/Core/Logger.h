#pragma once

#include <Core/ManiTypes.h>
#include <string_view>
#include <memory>
#include <filesystem>

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

	struct LogChannel
	{
		std::string_view name = "";
		ELogLevel level = ELogLevel::Log;
	};

	class Logger
	{
	public:
		static Logger& get()
		{
			static Logger instance;
			return instance;
		}

		Logger();
		~Logger();

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		void log(const LogChannel& channel, ELogLevel level, const std::string_view& log);
		void setFile(const Path& path);

		void suppress();
		void unsuppress();

	private:
		// we use pimpl here to avoid any chance of creating include loops since logging is so widespread
		struct State;
		std::unique_ptr<State> m_state = nullptr;
	};
}