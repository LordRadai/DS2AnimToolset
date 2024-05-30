#pragma once

#include "RDebug/RDebug.h"
#include "TextLog/TextLog.h"

class RLog
{
public:
	RLog();
	RLog(MsgLevel level, std::string path, std::string log_name);
	~RLog();

	void CreateLog(std::string path);
	UINT GetLogLevel();
	void SetLogLevel(UINT level);

	void DebugMessage(MsgLevel level, const char* fmt, ...);
	void AlertMessage(MsgLevel level, const char* fmt, ...);
	void PanicMessage(const char* fmt, ...);

	void AddEntry(bool print_time, const char* fmt, ...);

private:
	UINT m_logLevel;
	TextLog* m_logFile;
	std::string m_logPath;
	std::string m_logName;
};