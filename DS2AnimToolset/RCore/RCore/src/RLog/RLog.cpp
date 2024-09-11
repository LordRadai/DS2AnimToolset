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

void RLog::createLog(std::string path)
{
	this->m_logPath = path;

	this->m_logFile->setLogPath(path);
	this->m_logFile->createLogFile();
}

UINT RLog::getLogLevel()
{
	return this->m_logLevel;
}

void RLog::setLogLevel(UINT level)
{
	this->m_logLevel = level;
}

void RLog::debugMessage(MsgLevel level, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	RDebug::debuggerOut(this->m_logLevel, level, msg);

	if (this->m_logFile)
		this->m_logFile->addLog(true, msg);
}

void RLog::alertMessage(MsgLevel level, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	RDebug::systemAlert(this->m_logLevel, level, this->m_logName.c_str(), msg);

	if (this->m_logFile)
		this->m_logFile->addLog(true, msg);
}

void RLog::panicMessage(const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	RDebug::systemPanic(this->m_logName.c_str(), msg);

	if (this->m_logFile)
		this->m_logFile->addLog(true, msg);
}

void RLog::addEntry(bool print_time, const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);

	char msg[1000];
	vsprintf_s(msg, fmt, args);

	if (this->m_logFile)
		this->m_logFile->addLog(true, fmt, args);
	else
		RDebug::systemPanic(this->m_logName.c_str(), "m_logFile is nullptr");
}