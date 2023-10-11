#pragma once
#include <stdio.h>
#include <string>

namespace GL
{
	enum class LoggingLevel
	{
		ERROR = 0,
		WARNING = 1,
		INFO = 2,
		DEBUG = 3
	};

	class Logger
	{
	public:
		static void SetLoggingLevel(LoggingLevel level)
		{
			logging_level = level;
		}

		template<typename... Args>
		static void Log(LoggingLevel level, const std::string& msg, Args... args)
		{
			if (logging_level >= level)
			{
				printf(tags[(int)level].c_str());
				printf(msg.c_str(), args...);
				printf("\n");
			}
		}

	private:
		static std::string tags[4];
		static LoggingLevel logging_level;
	};

	inline LoggingLevel Logger::logging_level = LoggingLevel::INFO;

	inline std::string Logger::tags[4] = {
		"[ERROR]    ",
		"[WARNING]  ",
		"[INFO]     ",
		"[DEBUG]    "
	};
}

#define LOGERROR(msg, ...) GL::Logger::Log(GL::LoggingLevel::ERROR, msg,  __VA_ARGS__);
#define LOGWARNING(msg, ...) GL::Logger::Log(GL::LoggingLevel::WARNING, msg,  __VA_ARGS__);
#define LOGINFO(msg, ...) GL::Logger::Log(GL::LoggingLevel::INFO, msg,  __VA_ARGS__);
#define LOGDEBUG(msg, ...) GL::Logger::Log(GL::LoggingLevel::DEBUG, msg,  __VA_ARGS__);
