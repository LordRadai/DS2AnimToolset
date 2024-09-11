#pragma once

#include "RDebug/RDebug.h"
#include "TextLog/TextLog.h"

class RLog
{
public:
	RLog();
	RLog(MsgLevel level, std::string path, std::string log_name);
	~RLog();

	void createLog(std::string path);
	UINT getLogLevel();
	void setLogLevel(UINT level);

	void debugMessage(MsgLevel level, const char* fmt, ...);
	void alertMessage(MsgLevel level, const char* fmt, ...);
	void panicMessage(const char* fmt, ...);

	void addEntry(bool print_time, const char* fmt, ...);

private:
	UINT m_logLevel;
	TextLog* m_logFile;
	std::string m_logPath;
	std::string m_logName;
};