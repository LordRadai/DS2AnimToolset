#include "Log.h"
#include "mcu/OutputMessage.h"

#include <cstdarg>
#include <stdexcept>

#define LOG_BUFFER_SIZE 1024

#define WARNING_COLOR YELLOW
#define ERROR_COLOR RED
#define INFO_COLOR WHITE
#define DEBUG_COLOR GRAY

namespace mcu
{
	void logMessage(LogLevel level, const char* message)
	{
		const char* levelStr = nullptr;
		const char* color = nullptr;

		switch (level)
		{
		case LogLevel::kDebug:
			color = DEBUG_COLOR;
			levelStr = "[DEBUG]";
			break;
		case LogLevel::kInfo:
			color = INFO_COLOR;
			levelStr = "[INFO]";
			break;
		case LogLevel::kWarn:
			color = WARNING_COLOR;
			levelStr = "[WARNING]";
			break;
		case LogLevel::kError:
			color = ERROR_COLOR;
			levelStr = "[ERROR]";
			break;
		default:
			throw std::invalid_argument("Invalid log level");
			break;
		}

		char buffer[LOG_BUFFER_SIZE];
		snprintf(buffer, sizeof(buffer), "%-10s %s", levelStr, message);

		PRINT_MESSAGE(color, buffer);
	}

	void logDebug(const char* message)
	{
		logMessage(LogLevel::kDebug, message);
	}

	void logDebug(std::string message)
	{
		logDebug(message.c_str());
	}

	void logDebugf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		
		char buffer[LOG_BUFFER_SIZE];
		vsnprintf(buffer, sizeof(buffer), format, args);
		logDebug(buffer);

		va_end(args);
	}

	void logInfo(const char* message)
	{
		logMessage(LogLevel::kInfo, message);
	}

	void logInfo(std::string message)
	{
		logInfo(message.c_str());
	}

	void logInfof(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		
		char buffer[LOG_BUFFER_SIZE];
		vsnprintf(buffer, sizeof(buffer), format, args);
		logInfo(buffer);
		va_end(args);
	}

	void logWarning(const char* message)
	{
		logMessage(LogLevel::kWarn, message);
	}

	void logWarning(std::string message)
	{
		logWarning(message.c_str());
	}

	void logWarningf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		
		char buffer[LOG_BUFFER_SIZE];
		vsnprintf(buffer, sizeof(buffer), format, args);
		logWarning(buffer);
		va_end(args);
	}

	void logError(const char* message)
	{
		logMessage(LogLevel::kError, message);
	}

	void logError(std::string message)
	{
		logError(message.c_str());
	}

	void logErrorf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		
		char buffer[LOG_BUFFER_SIZE];
		vsnprintf(buffer, sizeof(buffer), format, args);
		logError(buffer);
		va_end(args);
	}
}