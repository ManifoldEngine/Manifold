#pragma once

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

		void log(const std::string_view& channel, ELogLevel level, const std::string_view& log);
		void setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel);
		void setFile(const std::filesystem::path& path);

		void suppress();
		void unsuppress();

	private:
		// we use pimpl here to avoid any chance of creating include loops since logging is so widespread
		struct State;
		std::unique_ptr<State> m_state = nullptr;
	};
}