#pragma once
#include <string>

namespace mcu
{
	enum class LogLevel
	{
		kInfo,
		kWarn,
		kError,
		kDebug,

		kNumLogLevels
	};

	void setLogLevel(uint32_t level);
	LogLevel getLogLevel();

	void logDebug(const char* message);
	void logDebug(std::string);
	void logDebugf(const char* format, ...);

	void logInfo(const char* message);
	void logInfo(std::string);
	void logInfof(const char* format, ...);

	void logWarning(const char* message);
	void logWarning(std::string);
	void logWarningf(const char* format, ...);

	void logError(const char* message);
	void logError(std::string);
	void logErrorf(const char* format, ...);
}

#define LOG_NOT_IMPLEMENTED() mcu::logDebugf("Function %s is not implemented.", __FUNCTION__)
#define LOG_TODO(msg) mcu::logDebugf("TODO in %s: %s.", __FUNCTION__, msg)