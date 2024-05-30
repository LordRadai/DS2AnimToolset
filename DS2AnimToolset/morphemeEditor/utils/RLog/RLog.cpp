#include "RLog.h"

RLog::RLog()
{
	this->m_logLevel = MsgLevel_Debug;
	this->m_logFile = new TextLog();
	this->m_logPath = "";
}

RLog::RLog(MsgLevel level, std::string path, std::string log_name)
{
	this->m_logLevel = level;
	this->m_logFile = new TextLog(path);
	this->m_logName = log_name;

	this->m_logPath = path;
}

RLog::~RLog()
{
}

void RLog::CreateLog(std::string path)
{
	this->m_logPath = path;

	this->m_logFile->SetLogPath(path);
	this->m_logFile->CreateLogFile();
}

UINT RLog::GetLogLevel()
{
	return this->m_logLevel;
}

void RLog::SetLogLevel(UINT level)
{
	this->m_logLevel = level;
}

void RLog::DebugMessage(MsgLevel level, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	RDebug::DebuggerOut(this->m_logLevel, level, fmt, args);

	if (this->m_logFile != nullptr)
		this->m_logFile->AddLog(true, fmt, args);
}

void RLog::AlertMessage(MsgLevel level, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	RDebug::SystemAlert(this->m_logLevel, level, this->m_logName.c_str(), fmt, args);

	if (this->m_logFile != nullptr)
		this->m_logFile->AddLog(true, fmt, args);
}

void RLog::PanicMessage(const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	RDebug::SystemPanic(this->m_logName.c_str(), fmt, args);

	if (this->m_logFile != nullptr)
		this->m_logFile->AddLog(true, fmt, args);
}

void RLog::AddEntry(bool print_time, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	if (this->m_logFile != nullptr)
		this->m_logFile->AddLog(true, fmt, args);
	else
		RDebug::SystemPanic(this->m_logName.c_str(), "m_logFile is nullptr");
}